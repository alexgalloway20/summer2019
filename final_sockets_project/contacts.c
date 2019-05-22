#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mysql/mysql.h>

#define MAXADDRLEN 256
#define BUFLEN 128

extern int errno;

void print_error(char *);

int main(int argc, char *argv[]) {
        char firstName[24] = "";
        char lastName[48] = "";
        char email[36] = "";
        long long phoneNumber;
        char resultCode[4];
	struct addrinfo *host_ai;
        struct addrinfo hint;
        int sockfd, err;

        if (argc != 2)
                print_error("usage: client hostname");

        for(;;) {
		int command, rec;
		printf("\n1. Insert New Contact\n");
		printf("2. List contacts\n");
		printf("3. Exit\n\n");
		scanf("%d", &command);
		while ((getchar()) != '\n');
		if(command == 1) {
			printf("\nAdding a new contact...\n\n");
			send(sockfd, &command, 4, 0);

			bzero(firstName, sizeof(firstName));
			printf("Enter first name: ");
			fgets(firstName, 24, stdin);
			send(sockfd, firstName, sizeof(firstName), 0);

			bzero(lastName, sizeof(lastName));
			printf("Enter last name: ");
			fgets(lastName, 48, stdin);
			send(sockfd, lastName, sizeof(lastName), 0);

			bzero(email, sizeof(email));
			printf("Enter email: ");
			fgets(email, 36, stdin);
			send(sockfd, email, sizeof(email), 0);

			printf("Enter phone number: ");
			scanf(" %lld", &phoneNumber);
			send(sockfd, &phoneNumber, sizeof(long long), 0);

			read(sockfd, resultCode, sizeof(resultCode));
			printf("\n%s\n", resultCode);
			close(sockfd);
			break;

                }

		if(command == 2){
			sockfd = server_connect(argv[1]);
			if(send(sockfd, &command, 4, 0) < 0){
				printf("Error sending data: %s\n", strerror(errno));
			}
			
			char *receiver = "0x02";
			if(send(sockfd, receiver, 4, 0) < 0){
				printf("Error sending data: %s\n", strerror(errno));
			}

			int count;
			if(recv(sockfd, &count, 4, 0) < 0){
				printf("Error receiving data: %s\n", strerror(errno));
			}

			char *ret;
			int req = 116 * count;
			ret = malloc(req);
			memset(ret, 0, req);

			int count1 = 0;
			while(count1 < 4 * count){
				if(recv(sockfd, ret, req, 0) < 0){
					printf("Error receiving data: %s\n", strerror(errno));
				}
			
				if(count1 % 4 == 0){
					printf("\n");
				}
			}
			printf("%s ", ret);
			count1++;
		}
		close(sockfd);
		break;

                if(command == 3) {
                        printf("\nExiting...\n\n");
                        exit(1);
                }
        }
}

int server_connect(char *iphost){
        struct addrinfo *host_ai;
        struct addrinfo hint;
        int sockfd, err;

        hint.ai_flags = 0;
        hint.ai_family = AF_INET;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_protocol = 0;
        hint.ai_addrlen = 0;
        hint.ai_canonname = NULL;
        hint.ai_addr = NULL;
        hint.ai_next = NULL;

        if ((err = getaddrinfo(iphost, "contacts", &hint, &host_ai)) != 0) {
                printf("getaddrinfo error: %s\n", gai_strerror(err));
                return 0;
        }

        printf("creating socket\n");
        if ((sockfd = socket(host_ai->ai_addr->sa_family, SOCK_STREAM, 0)) < 0) {
                print_error("Error creating socket");
        }
        printf("socket created\n");

        printf("attempting Connection\n");
        if (connect(sockfd, host_ai->ai_addr, host_ai->ai_addrlen) != 0) {
                printf("can't connect to %s\n", iphost);
                print_error("Error connecting to server");
        }
        printf("connection made...\n");

	return(sockfd);
}


void print_error(char *str) {
        printf("%s: %s\n", str, strerror(errno));
        exit(1);
}

// END OF FILE
