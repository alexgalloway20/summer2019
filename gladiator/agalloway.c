#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include "sem_helper.h"
void battle(int semID, int k, int i, int id);

int main(int argc, char* argv[]) {
        int semid;
        int gladiator_id;
        int arena_len;
        int index = 0;

        if (argc != 4) {
                printf("usage: %s semid gladiator_id arena_len\n", argv[0]);
                return 0;
        }
        semid = atoi(argv[1]);
        gladiator_id = atoi(argv[2]);
        arena_len = atoi(argv[3]);
        short arenaSize[(arena_len * arena_len) - 1];
        short *arr = arenaSize;
        getAllSemaphores(semid, arena_len, arr);
        /* Battle */
        while((arr[index] != gladiator_id)){
                index++;
        }

        battle(semid,index,arena_len,gladiator_id);
        return 0;
}

void battle(int semID, int i, int k, int id){

        while(1){
		if(i > 0){
                        if((i + k) < 0){
                                setSemaphore(semID, i + k, id);
                                i = i + k;
                        }else if((i - k) > 0){
                                setSemaphore(semID, i - k, id);
                                i = i - k;
                        }else{
                                setSemaphore(semID, i - 1, id);
                                i = i - 1;
                        }

                        int a = 0;
                        for(a; a < k * k; a++){
                                setSemaphore(semID, a, id);
                                i + 1;
                        }
                }
        }
        printf("I submit: \n");
}
