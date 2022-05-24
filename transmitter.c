// general headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

// signals
#include <signal.h>

// object specific headers
#include "header_files/objects.h"
#include "header_files/threader.h"
#include "header_files/sem.h"

// headers to be deleted
#include <unistd.h>
#include "header_files/sem_th.h"

int WAIT_CON = 0;

void transmitter_handler(int sig);

int transmitter(Object obj, int pids[]) {

    XImage *image;
    Pixmap pixmap;
    
    sem_init(&sem_th, 1, 10);
    // signal register area to receive signal from child processes.
    struct sigaction sig = { 0 };
    sig.sa_handler = &transmitter_handler;
    sigaction(SIGRTMIN, &sig, NULL);

    // image data pointer
    char *shmem_2;
    // char image_data[800 * 800 * 4];
    key_t key_2 = 9998;
    int shmid_2 = shmget(key_2, obj.winattr->width * obj.winattr->height * 4, 0666);
    shmem_2 = shmat(shmid_2, NULL, 0);


    // thats the signal for the child process to write in the shared memory...
    for (int i = 0; i < 10; i++) {
        kill(pids[i], SIGUSR1);
    }
    printf("Condition BEFORE loop: %d\n", WAIT_CON);
    // waiting for the threader signal to break out of this waiting loop.
    while (1) {
        // sleep(1);
        if (WAIT_CON < 10 && WAIT_CON > 0) {
            // printf("Condition IN loop: %d\n", WAIT_CON);
            image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, shmem_2, obj.winattr->width, obj.winattr->height, 32, 0);
            pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);
            XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);
            XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
        } else if (WAIT_CON == 10) {
            XFree(image);
            // printf("Condition IS 10 IN loop: %d\n", WAIT_CON);
            WAIT_CON = 0;
            break;
        }
    }
    printf("Condition AFTER loop: %d\n", WAIT_CON);

    // closing the semaphore which used in main2.c because we can't close it there.
    sem_close(&sem);
    sem_close(&sem_th);
    
    shmdt(&shmid_2);
    
    return 0;
}

void transmitter_handler(int sig) {
    sem_wait(&sem_th);
    WAIT_CON++;
    sem_post(&sem_th);
}

