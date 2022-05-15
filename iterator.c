#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// signals
#include <signal.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "header_files/objects.h"
#include "header_files/palette.h"

int iterator(Object obj, int pid) {

    Object *shmem;
    key_t key = 9999;
    int shmid = shmget(key, sizeof(Object) * sizeof(obj), 0666 | IPC_CREAT);
    shmem = shmat(shmid, NULL, 0);

    obj.image_data = calloc(1, sizeof(char) * obj.winattr->width * obj.winattr->height * 4);
    if (obj.image_data == NULL) {
        printf("Not Enough memory for the image_data to be allocated...Exit status 1\n");
        free(obj.image_data);
        exit(1);
    }

    *shmem = obj;
 
    printf("Iterator, preparing to send signal\n");

    // thats the signal for the child process to write in the shared memory...
    kill(pid, SIGUSR1); ////////////////////////////////////////////////////
    // here probably i must wait for the child process to write the data before paint the pixmap.
    printf("Iterator, signal has been sended\n");

    free(obj.image_data);
    return 0;
}
