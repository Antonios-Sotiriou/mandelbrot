// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "header_files/objects.h"
#include "header_files/painter.h"

// headers to be deleted
#include <stdio.h>
#include "header_files/sem_th.h"

int threader(KNOT knot) {
    
    // sem_init(&sem_th, 1, 1);

    char *shmem_2;
    key_t key_2 = 9998;
    int shmid_2 = shmget(key_2, knot.width * knot.height * 4, 0666);
    shmem_2 = shmat(shmid_2, NULL, 0);

    printf("Image data: %s", shmem_2);
    printf("Object data: %d **************************\n", knot.width);

    // sem_wait(&sem_th);

    int counter = knot.step_counter;
    int x = knot.step_x;
    int y = knot.step_y;

    for (int i = 0; i < knot.width * knot.height; i++) {

        if (x == knot.width) {
            y += 1;
            x = 0;
        }

        knot.x = x;
        knot.y = y;
        knot.counter = counter;
        painter(knot, (char*)&shmem_2);
        counter += 4;
        x++;
    }
    sem_post(&sem_th);
    printf("Exiting Threader ******************************************************\n");
    return 0;
}

