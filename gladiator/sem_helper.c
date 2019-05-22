/***
 * sem_helper.c
 *
 * Eric McGregor
 */

#include <stdio.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "sem_helper.h"

void print_error(const char* str, int code)
{
        printf("%s: %s\n",str, strerror(code));
        exit(-1);
}

int getSemaphoreID(int size, int tokid)
{
        char *filepath = "/tmp";
        key_t key;
        int semid;

        if ((key = ftok(filepath, tokid)) == -1) {
                print_error("Can not create token", errno);
        }

        if ((semid = semget(key, size, 0666 | IPC_CREAT)) == -1) {
                print_error("Can not create semaphore", errno);
        }

        return semid;
}

void freeSemaphores(int semid)
{
	union semun {
                int val;
                struct semid_ds* buf;
                ushort* array;
        } arg;

        arg.val = 0;

        if ((semctl(semid, 0, IPC_RMID, arg)) == -1) {
                print_error("Error freeing semaphors", errno);
        }
}

void setAllSemaphores(int semid, int size, int value)
{
        unsigned short arr[size];
        int i;
        for (i = 0; i< size;i++){
                arr[i] = value;
        }

        union semun {
                int val;
                struct semid_ds* buf;
                ushort* array;
        } arg;

        arg.array = arr;

        if ((semctl(semid, 0, SETALL, arg)) == -1) {
                print_error("Error setting all semaphore to 0", errno);
        }
}

void getAllSemaphores(int semid, int size, unsigned short* arr)
{
        union semun {
                int val;
                struct semid_ds* buf;
                ushort* array;
	} arg;

        arg.array = arr;

        if ((semctl(semid, 0, GETALL, arg)) == -1) {
                print_error("Error getting all semaphores 0", errno);
        }
}

void setSemaphore(int semid, int index, int value) {

        union semun {
                int val;
                struct semid_ds* buf;
                ushort* array;
        } arg;

        arg.val = value;

        if ((semctl(semid, index, SETVAL, arg)) == -1) {
                print_error("Error setting semaphore to 0", errno);
        }
}

int getSemaphore(int semid, int index) {

        int semValue;
        if ((semValue = semctl(semid, index, GETVAL)) == -1) {
                print_error("Error getting semaphore value", errno);
        }

        return semValue;
}

void incrementSemaphore(int semid, int index) {
        struct sembuf op[1];
        int retval;

        op[0].sem_num = index;
        op[0].sem_op = 1;
	op[0].sem_flg = 0;

        if ((retval = semop(semid, op, 1)) == -1) {
                printf("Error incrementing semaphore", errno);
        }
}

void decrementSemaphore(int semid, int index) {
        struct sembuf op[1];
        int retval;

        op[0].sem_num = index;
        op[0].sem_op = -1;
        op[0].sem_flg = 0;

        if ((retval = semop(semid, op, 1)) == -1) {
                printf("Error decrementing semaphore", errno);
        }
}

// END OF FILE
