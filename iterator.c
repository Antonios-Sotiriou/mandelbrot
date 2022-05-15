#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlocale.h>

// signals
#include <signal.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "header_files/objects.h"
#include "header_files/semaphores.h"
#include "header_files/sem_var.h"

int iterator(Object obj, int pid) {

    sem_init(&sem, 1, sem_var);

    Object *shmem;
    key_t key = 9999;
    int shmid = shmget(key, sizeof(Object), 0666 | IPC_CREAT);
    shmem = shmat(shmid, NULL, 0);

    *shmem = obj;
    shmctl(shmid, IPC_RMID, 0);

    printf("Iterator, preparing to send signal\n");

    // thats the signal for the child process to write in the shared memory...
    kill(pid, SIGUSR1); ////////////////////////////////////////////////////
    // here probably i must wait for the child process to write the data before paint the pixmap.
    // sem_wait(&sem);
    printf("Iterator, signal has been sended\n");

    sem_close(&sem);
    shmdt(shmem);

    return 0;
}
