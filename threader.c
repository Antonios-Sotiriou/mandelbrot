#include <stdio.h>
#include <stdlib.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "header_files/objects.h"
#include "header_files/painter.h"
#include "header_files/threader.h"

int threader() {

    Object *shmem;
    key_t key = 9999;
    int shmid = shmget(key, sizeof(Object), 0666 | IPC_CREAT);
    shmem = shmat(shmid, NULL, 0);

    shmctl(shmid, IPC_RMID, 0);

    // shmem->image_data = calloc(1, sizeof(char) * 800 * 800 * 4);

    int counter = 0;
    int x = 0;
    int y = 0;

    for (int i = 0; i < 800 * 800; i++) {

        if (x == 800) {
            y += 1;
            x = 0;
        }

        shmem->x = x;
        shmem->y = y;
        shmem->counter = counter;
        painter(*shmem);
        counter += 4;
        x++;
    }

    XImage *image = XCreateImage(shmem->displ, shmem->winattr->visual, shmem->winattr->depth, ZPixmap, 0, shmem->image_data, shmem->winattr->width, shmem->winattr->height, 32, 0);

    Pixmap pixmap = XCreatePixmap(shmem->displ, shmem->win, shmem->winattr->width, shmem->winattr->height, shmem->winattr->depth);

    XPutImage(shmem->displ, pixmap, shmem->gc, image, 0, 0, 0, 0, shmem->winattr->width, shmem->winattr->height);

    XCopyArea(shmem->displ, pixmap, shmem->win, shmem->gc, 0, 0, shmem->winattr->width, shmem->winattr->height, 0, 0);

    // free(shmem->image_data);
    XFree(image);
    
    shmdt(shmem);

    return 0;
}

