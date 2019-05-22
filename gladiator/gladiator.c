/***************
 *
 * gladiator.c
 *
 * Eric McGregor
 ****************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sem_helper.h"

int main(int argc, char* argv[])
{
        int semid;
        int gladiator_id;
        int arena_len;

        if (argc != 4) {
                printf("usage: %s semid gladiator_id arena_len\n", argv[0]);
                return 0;
        }

        semid = atoi(argv[1]);
        gladiator_id = atoi(argv[2]);
        arena_len = atoi(argv[3]);

        /* Battle */

        return 0;
}

//  END OF FILE
