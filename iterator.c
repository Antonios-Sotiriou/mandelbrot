// general headers
#include <stdio.h>
#include <stdlib.h>

// signals
#include <signal.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

// object specific headers
#include "header_files/objects.h"
#include "header_files/threader.h"
#include "header_files/semaphores.h"
// #include "header_files/sem_var.h"

int iterator(Object obj, int pids[]) {
    
    // sem_init(&sem, 1, 1);

    Object *shmem;
    key_t key = 9999;
    int shmid = shmget(key, sizeof(Object), 0666 | IPC_CREAT);
    shmem = shmat(shmid, NULL, 0);

    *shmem = obj;

    shmem->init_x = obj.init_x;
    shmem->init_y = obj.init_y;

    obj.image_data = calloc(1, sizeof(char) * obj.winattr->width * obj.winattr->height * 4);
    if (obj.image_data == NULL) {
        printf("Not Enough memory for the image_data to be allocated...Exit status 1");
        free(obj.image_data);
        exit(1);
    }

    // thats the signal for the child process to write in the shared memory...
    for (int i = 0; i < 10; i++) {
        // sem_wait(&sem);
        shmem->counter = i;
        kill(pids[i], SIGUSR1); //////////////////////////////////////////////////// 
        // sem_post(&sem);  
    }

    threader(obj);
    //shmctl(shmid, IPC_RMID, 0);
    // here probably i must wait for the child process to write the data before paint the pixmap.
    // sem_wait(&sem);
    printf("Iterator, signal has been sended\n");

    // sem_close(&sem);
    //shmdt(shmem);

    XImage *image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, obj.image_data, obj.winattr->width, obj.winattr->height, 32, 0);

    Pixmap pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);

    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);

    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    free(obj.image_data);
    XFree(image);

    return 0;
}
