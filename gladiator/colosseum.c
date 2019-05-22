/****************
 * colosseum.c
 * Eric McGregor
 ****************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "sem_helper.h"

#define ARENA_LEN 8

void printArena();

int semid;

static void handler (int signo)
{
        printf("\n[MCG] Battle concluded.\n");
        freeSemaphores(semid);
        exit(1);
}

int main(int argc, char* argv[])
{
        /**************************************************************
         * Check that two gladiators were entered on the command line *
         **************************************************************/

        if (argc != 3) {
                printf("usage: %s gladiator1 gladiator2\n", argv[0]);
                exit(1);
        }

        /************************************************************
         * Set up a signal handler to free the semaphores on SIGINT *
         ************************************************************/

	struct sigaction act, oact;

        act.sa_handler = handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        if (sigaction(SIGINT, &act, &oact) == -1) {
                print_error("Error registering signal handler", errno);
        }

        /******************************
         * Set up the semaphore arena *
         ******************************/

        int size = ARENA_LEN * ARENA_LEN;

        srand(time(NULL));
        int token_id = rand();

        semid = getSemaphoreID(size, token_id);
        setAllSemaphores(semid, size, 0);

        printf("[MCG] Arena ready ...\n");
        sleep(1);

        printArena();
        sleep(1);

        /**********************************************
         * Selecting a random gladiator to exec first *
         **********************************************/

        char* gladiator1 = argv[1];
        char* gladiator2 = argv[2];

        int coin = rand() % 2;

        if (coin == 1) {
                char* temp = gladiator1;
		gladiator1 = gladiator2;
                gladiator2 = temp;
        }

        printf("[MCG] %s(1) VS. %s(2)\n", gladiator1, gladiator2);
        sleep(2);

        /**********************************************
         * Placing the gladiators at random positions *
         **********************************************/

        int loc1 = rand() % size;
        int loc2 = loc1;
        while (loc2 == loc1) {
                loc2 = rand() % size;
        }

        printf("[MCG] putting gladiators on the field ...\n");
        sleep(1);

        setSemaphore(semid, loc1, 1);
        setSemaphore(semid, loc2, 2);

        printArena();
        sleep(2);

        /********************************
         * Creating gladiator processes *
         ********************************/

        char path[1024];
        char sem[16];
        char id[8];
        char len[8];

        memset(path, 0, 1024);
        strncpy(path, "./", 2);
        strncat(path, gladiator1, 1021);
        sprintf(sem, "%d", semid);
        sprintf(id, "%d", 1);
        sprintf(len, "%d", ARENA_LEN);

	pid_t pid;
        if ((pid = fork()) == 0) {
                printf("[MCG] executing %s %s %s %s\n", path, sem, id, len);
                if (execl(path, gladiator1, sem, id, len, NULL) == -1) {
                        printf("%s failed to fight (%s)\n", gladiator1, strerror(errno));
                        exit(1);
                }
        }
        printf("[MCG] %s is in the arena\n", gladiator1);

        memset(path, 0, 1024);
        strncpy(path, "./", 2);
        strncat(path, gladiator2, 1021);
        sprintf(sem, "%d", semid);
        sprintf(id, "%d", 2);

        if ((pid = fork()) == 0) {
                printf("[MCG] executing %s %s %s %s\n", path, sem, id, len);
                if (execl(path, gladiator2, sem, id, len, NULL) == -1) {
                        printf("%s failed to fight (%s)\n", gladiator2, strerror(errno));
                        exit(1);
                }
        }

        printf("[MCG] %s is in the arena\n", gladiator2);

        printf("[MCG] Let the games begin ...\n");

        /**********************************************
         * Display the arena until the battle is over *
         **********************************************/

        struct timespec req = {0};
        req.tv_sec = 0;
        req.tv_nsec = 500000000L;

        while(1) {
                printArena();
                nanosleep(&req,&req);
	}

        exit(0);
}

void printArena()
{
        int i, j, k;

        int size = ARENA_LEN * ARENA_LEN;
        unsigned short arr[size];

        getAllSemaphores(semid, size, arr);

        printf("*\n");
        for (i = 0; i < ARENA_LEN; i++) {
                for (j = 0; j < ARENA_LEN; j++) {
                        k = arr[(i * ARENA_LEN) + j];
                        if (k == 1 || k == 2) {
                                printf("%u ", k);
                        } else {
                                printf(". ");
                        }
                }
                printf("\n");
        }
        printf("\n");
}

//  END OF FILE
