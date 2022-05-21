// general headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// signals
#include <signal.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

// object specific headers
#include "header_files/objects.h"
#include "header_files/threader.h"
#include "header_files/sem.h"

// headers to be deleted
#include <unistd.h>
#include "header_files/sem_th.h"

int transmitter(Object obj, int pids[]) {
    
    sem_init(&sem_th, 1, 1);
    // signal register area to receive signal from child processes.

    // object to transfer between processes the integer variables for each process calculations.
    KNOT knot, *shmem;
    key_t key = 9999;
    int shmid = shmget(key, sizeof(KNOT), 0666 | IPC_CREAT);
    shmem = shmat(shmid, NULL, 0);

    // initialize the knot object to be transfered because we can't transfer pointers to pointers through shared memory.
    knot.width = obj.winattr->width;
    knot.height = obj.winattr->height;
    knot.horiz = obj.horiz;
    knot.vert = obj.vert;
    knot.max_iter = obj.max_iter;
    knot.zoom = obj.zoom;
    knot.x = obj.x;
    knot.y = obj.y;
    knot.init_x = obj.init_x;
    knot.init_y = obj.init_y;

    // image data pointer
    char *shmem_2;
    key_t key_2 = 9998;
    int shmid_2 = shmget(key_2, knot.width * knot.height * 4, 0666 | IPC_CREAT);
    shmem_2 = shmat(shmid_2, NULL, 0);

    // trying to pass data duplex
    char image_data[800 * 800 * 4];
    shmem_2 = image_data;
    
    strcpy(shmem_2, "Test Hello World\n");

    *shmem = knot;


    // thats the signal for the child process to write in the shared memory...
    for (int i = 0; i < 10; i++) {
        kill(pids[i], SIGUSR1);
    }
    
    sem_wait(&sem_th);
    printf("Transmitter finish before Threader************* %s\n", shmem_2);


    // closing the semaphore which used in main2.c because we can't close it there.
    sem_close(&sem);

    sem_close(&sem_th);

    XImage *image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, shmem_2, obj.winattr->width, obj.winattr->height, 32, 0);

    Pixmap pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);

    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);

    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    //free(obj.image_data);
    XFree(image);

    return 0;
}
