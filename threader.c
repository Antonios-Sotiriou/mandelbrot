#include <stdio.h>
#include <unistd.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

// signals
#include <signal.h>

// object specific headers
#include "header_files/objects.h"
#include "header_files/global_vars.h"
#include "header_files/painter.h"

int threader(KNOT knot) {

    char *shmem_2;
    key_t key_2 = 9998;
    int shmid_2 = shmget(key_2, knot.width * knot.height * 4, 0666);
    if (shmid_2 == -1) {
        perror("Threader - shmget()");
        return 1;
    }
    shmem_2 = shmat(shmid_2, NULL, 0);
    if (shmem_2 == NULL) {
        perror("Threader - shmat()");
        return 1;
    }

    int counter = knot.step_counter;
    int x = knot.step_x;
    int y = knot.step_y;

    for (int i = 0; i < (knot.width * knot.height) / PROC_NUM; i++) {

        if (x == knot.width) {
            y += 1;
            x = 0;
        }

        knot.x = x;
        knot.y = y;
        knot.counter = counter;
        painter(knot, shmem_2);
        counter += 4;
        x++;
    }

    kill(getppid(), SIGRTMIN);
    if(shmdt(shmem_2) == -1) {
        perror("threader - shmdt()");
        return 1;
    }
    return 0;
}

