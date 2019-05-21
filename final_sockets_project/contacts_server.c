/*
 * server.c
 * derived from Advanced Programming in the UNIX Environment, 3rd Edition
 *
 * Eric McGregor 02/27/15
 */

#include <sys/types.h>  // getaddrinfo()
#include <netdb.h>      // getaddrinfo()
#include <sys/socket.h> // getaddrinfo()
#include <stdio.h>      // printf()
#include <stdlib.h>     // exit()
#include <unistd.h>     // gethostname()
#include <arpa/inet.h>  // inet_ntop()
#include <string.h>     // strerror()
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>
#include <mysql/mysql.h>

#define QLEN 128

#ifndef HOST_NAME_MAX
        #define HOST_NAME_MAX 256
#endif

extern int errno;

void print_ip_addresses(struct addrinfo *ai);
void print_error(char *);
void insertContact(int clfd);
int main(int argc, char *argv[]){
	char *host_name;

        if (argc == 2) {
                host_name = argv[1];
        }
        else {
                host_name = malloc(HOST_NAME_MAX);
                memset(host_name, 0, HOST_NAME_MAX);

                if (gethostname(host_name, HOST_NAME_MAX) < 0) {
                        print_error("gethostname error");
                        return(-1);
                }
        }

        printf("host name: %s\n", host_name);

        struct addrinfo         *host_ai;
        struct addrinfo         hint;

        hint.ai_flags = 0;
        hint.ai_family = 0;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_protocol = 0;
        hint.ai_addrlen = 0;
        hint.ai_canonname = NULL;
        hint.ai_addr = NULL;
        hint.ai_next = NULL;

        if ((getaddrinfo(host_name, "tokenserver", &hint, &host_ai)) != 0) {
                print_error("getaddrinfo error");
                exit(1);
        }

        print_ip_addresses(host_ai);

        int host_fd;
        if ((host_fd = socket(host_ai->ai_addr->sa_family, SOCK_STREAM, 0)) < 0) {
                print_error("unable to create socket");
                return(-1);
        }
        printf("socket created [%d]\n", host_fd);

        if (bind(host_fd, host_ai->ai_addr, host_ai->ai_addrlen) < 0) {
                print_error("unable to bind to socket");
                exit(1);
        }
        printf("bind returned success\n");

        freeaddrinfo(host_ai);

        if (listen(host_fd, QLEN) < 0) {
                print_error("listen failed");
                exit(1);
        }
        printf("listen returned success\n");

        int last_client_ip_int = 0;

        struct sockaddr client_sockaddr;
        socklen_t client_sockaddr_len = sizeof(client_sockaddr);
        int token = 0;
        for (;;) {
                printf("waiting for connection ...\n");
                int clfd = accept(host_fd, &client_sockaddr, &client_sockaddr_len);
                if (clfd < 0) {
                        print_error("accept error");
                        exit(1);
                }
                printf("accepted connection, socket [%d]\n", clfd);

                if (client_sockaddr.sa_family != AF_INET) {
                        printf("Can not connect with IPv6 addresses\n");
                        printf("Sending -1\n");

                        int mssg = -1;
                        int len = send(clfd, &mssg, 4, 0);
                        if (len < 0) {
                                print_error("error sending data");
                        }
                        printf("sent %d bytes\n", len);

                        close(clfd);
                        continue;
                }
        }
        close(host_fd);

        FILE *fptr = fopen("contacts.csv", "w");
        if(fptr == NULL) {
                printf("Could not open file\n");
        }
        for(;;) {
		int clfd = accept(host_fd, &client_sockaddr, &client_sockaddr_len);
                int len;
                int command;
                char firstName[24];
                char lastName[48];
                char email[36];
                long long phoneNumber;
                char success[4] = "0x00";
                char firstNull[4] = "0x01";
                char lastNull[4] = "0x02";
                char repeat[4] = "0x03";
                read(clfd, &command, 4);
                if(command == 1) {
                                printf("\nAdding new contact...\n");

                                bzero(firstName, sizeof(firstName));
                                read(clfd, firstName, sizeof(firstName));
                                len = strlen(firstName);
                                if(firstName[len-1] == '\n') {
                                        firstName[len-1] = 0;
                                }
                                if(firstName[0] == '\0') {
                                        printf("First name is null\n");
                                        send(clfd, firstNull, sizeof(firstNull), 0);
                                }

                                bzero(lastName, sizeof(lastName));
                                read(clfd, lastName, sizeof(lastName));
                                len = strlen(lastName);
                                if(lastName[len-1] == '\n') {
                                        lastName[len-1] = 0;
                                }
                                if(lastName[0] == '\0') {
                                        printf("Last name is null\n");
                                        send(clfd, lastNull, sizeof(lastNull), 0);
                                }

                                bzero(email, sizeof(email));
                                read(clfd, email, sizeof(email));
                                len = strlen(email);
                                if(email[len-1] == '\n') {
                                        email[len-1] = 0;
                                }

                                read(clfd, &phoneNumber, sizeof(long long));

                                if(firstName[0] != '\0' && lastName[0] != '\0') {
                                        send(clfd, success, sizeof(success), 0);
                                        fprintf(fptr, "%s,%s,%s,%lld\n", firstName, lastName, email, phoneNumber);
                                }

                                bzero(firstName, sizeof(firstName));
                                bzero(lastName, sizeof(lastName));
                                bzero(email, sizeof(email));

				MYSQL *con = mysql_init(NULL);
				char *hostname = "rds-mysql-test.czwjuefjbmhj.us-east-2.rds.amazonaws.com";
				char *username = "alexgalloway20";
				char *password = "1997!((&Ag";
				char *schema = "contacts";

				if (con == NULL){
					fprintf(stderr, "%s\n", mysql_error(con));
					exit(1);
				}

				if (mysql_real_connect(con, hostname, username, password, schema, 0, NULL, 0) == NULL){
					fprintf(stderr, "%s\n", mysql_error(con));
					mysql_close(con);
					exit(1);
				}

				char *query = "CREATE TABLE Contacts_T ( "
				"first_name VARCHAR(24), "
				"last_name VARCHAR(48), "
				"email VARCHAR(36), "
				"phone BIGINT, "
				"PRIMARY KEY(first_name, last_name))";

				if (mysql_query(con, query)){
					fprintf(stderr, "%s\n", mysql_error(con));
					mysql_close(con);
					exit(1);
				}

				mysql_close(con);
				exit(0);
                }

                if(command == 2) {
			/*MySQL Login*/
			MYSQL *con = mysql_init(NULL);
                        char *hostname = "rds-mysql-test.czwjuefjbmhj.us-east-2.rds.amazonaws.com";
                        char *username = "alexgalloway20";
                        char *password = "1997!((&Ag";
                        char *schema = "contacts";

                        if (con == NULL){
                         	fprintf(stderr, "%s\n", mysql_error(con));
                         	exit(1);
                        }

                        if (mysql_real_connect(con, hostname, username, password, schema, 0, NULL, 0) == NULL){
                         	fprintf(stderr, "%s\n", mysql_error(con));
                        	mysql_close(con);
                       		exit(1);
                        }

			/*Data retrieval*/
			if (mysql_query(con, "SELECT * FROM Contacts_T")){
				fprintf(stderr, "%s\n", mysql_error(con));
				mysql_close(con);
				exit(1);
			}

			MYSQL_RES *result = mysql_store_result(con);

			if (result == NULL) {
				fprintf(stderr, "%s\n", mysql_error(con));
				mysql_close(con);
				exit(1);
			}

			int num_fields = mysql_num_fields(result);

			MYSQL_ROW row;

			while ((row = mysql_fetch_row(result))) {
				for(int i = 0; i < num_fields; i++) {
					printf("%s ", row[i] ? row[i] : "NULL");
				}
				printf("\n");
			}

			mysql_close(con);		
		}
        }
}                      

void print_ip_addresses(struct addrinfo *host_ai)
{
        struct addrinfo *next_ai;
        unsigned long *ipv4_addr;
        unsigned char *ipv6_addr;

        char ip_str[INET6_ADDRSTRLEN];

        for (next_ai = host_ai; next_ai != NULL; next_ai = next_ai->ai_next) {

                memset(ip_str, '\0', INET6_ADDRSTRLEN);

                if (next_ai->ai_addr->sa_family == AF_INET) {
                        printf("IPv4 ");
                        ipv4_addr  = (unsigned long*) &(((struct sockaddr_in*)next_ai->ai_addr)->sin_addr);
                        inet_ntop(AF_INET, ipv4_addr, ip_str, sizeof(ip_str));
                }
                else {
                        printf("IPv6 ");
                        ipv6_addr = (unsigned char*) &(((struct sockaddr_in6*)next_ai->ai_addr)->sin6_addr);
                        inet_ntop(AF_INET6, ipv6_addr, ip_str, sizeof(ip_str));
                }

                printf("IP address: %s\n", ip_str);
        }
}

void print_error(char *str) {
        printf("%s: %s\n", str, strerror(errno));
        exit(1);
}
