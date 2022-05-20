// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "header_files/objects.h"
#include "header_files/threader.h"
#include "header_files/painter.h"

int threader(Object obj) {

    int counter = 0;
    int x = 0;
    int y = 0;

    for (int i = 0; i < obj.winattr->width * obj.winattr->height; i++) {

        if (x == obj.winattr->width) {
            y += 1;
            x = 0;
        }

        obj.x = x;
        obj.y = y;
        obj.counter = counter;
        painter(obj);
        counter += 4;
        x++;
    }

    // shmdt(shmem);

    return 0;
}

