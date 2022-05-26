// general headers
#include <stdio.h>
#include <errno.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
/* semaphore specific includes */
#include <fcntl.h>
#include <sys/stat.h>

// signals
#include <signal.h>

// object specific headers
#include "header_files/objects.h"
#include "header_files/global_vars.h"
#include "header_files/threader.h"
#include "header_files/transem.h"

// this global variable is used only from this file and only from transmitter function.
int WAIT_CON = 0;

void transmitter_handler(int sig);

int transmitter(Object obj, int pids[]) {

    XImage *image;
    Pixmap pixmap;
    
    // semaphores initialization
    //sem_init(&transem, 1, 10);
    if (sem_unlink("/transem") == -1){
        if (errno != ENOENT) {
            perror("Transmitter - sem_unlink()");
            return 1;
        }
    }
    transem = sem_open("/transem", O_CREAT, 0666, 10);
    if (transem == SEM_FAILED) {
        perror("transmitter - sem_open()");
        return 1;
    }

    // signal register area to receive signal from child processes.
    struct sigaction sig = { 0 };
    sig.sa_handler = &transmitter_handler;;
    if (sigaction(SIGRTMIN, &sig, NULL) == -1) {
        perror("Transmitter - sigaction()");
        return 1;
    }

    // image data pointer
    char *shmem_2;
    key_t key_2 = 9998;
    int shmid_2 = shmget(key_2, obj.winattr->width * obj.winattr->height * 4, 0666);
    if (shmid_2 == -1) {
        perror("Transmitter - shmget()");
        return 1;
    }
    shmem_2 = shmat(shmid_2, NULL, 0);
    if (shmem_2 == NULL) {
        perror("Transmitter - shmat()");
        return 1;
    }

    // thats the signal for the child process to write in the shared memory...
    for (int i = 0; i < PROC_NUM; i++) {
        kill(pids[i], SIGUSR1);
    }

    // waiting for the threader signal to break out of this waiting loop.
    while (1) {
        if (WAIT_CON == PROC_NUM) {
            WAIT_CON = 0;
            break;
        }
    }

    image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, shmem_2, obj.winattr->width, obj.winattr->height, 32, 0);
    pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);
    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);
    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    XFree(image);

    // closing the semaphore which used in main2.c because we can't close it there.
    if (sem_close(transem) == -1) {
        perror("transmitter - sem_close()");
        return 1;
    }
    
    if(shmdt(shmem_2) == -1) {
        perror("transmitter - shmdt()");
        return 1;
    }
    
    return 0;
}

void transmitter_handler(int sig) {
    sem_wait(transem);
    WAIT_CON++;
    sem_post(transem);
}

