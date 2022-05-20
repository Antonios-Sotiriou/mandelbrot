#include <stdio.h>
#include <string.h>

// multiprocessing includes
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>

// Time included for testing execution time
#include <time.h>

#include "header_files/threader.h"

#include "header_files/objects.h"
#include "header_files/semaphores.h"

int LOOP_CON = 0;
int PROC_NUM = 0;

void signal_handler(int sig);

int main(int argc, char *argv[]) {

    sem_init(&sem, 1, 10);
    int sig_val = 1;

    struct sigaction sig = { 0 };
    sig.sa_handler = &signal_handler;
    sig_val = sigaction(SIGUSR1, &sig, NULL);

    while (!sig_val) {
        sem_wait(&sem);
        if (LOOP_CON) {

            Object obj, *shmem;
            key_t key = 9999;
            int shmid = shmget(key, sizeof(Object), 0666);
            shmem = shmat(shmid, NULL, 0);
            obj = *shmem;

            if (strcmp(argv[0], "process_1") == 0) {
                printf("Process_1 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 1;
            } else if (strcmp(argv[0], "process_2") == 0) {
                printf("Process_2 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 2;
            } else if (strcmp(argv[0], "process_3") == 0) {
                printf("Process_3 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 3;
            } else if (strcmp(argv[0], "process_4") == 0) {
                printf("Process_4 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 4;
            } else if (strcmp(argv[0], "process_5") == 0) {
                printf("Process_5 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 5;
            } else if (strcmp(argv[0], "process_6") == 0) {
                printf("Process_6 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 6;
            } else if (strcmp(argv[0], "process_7") == 0) {
                printf("Process_7 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 7;
            } else if (strcmp(argv[0], "process_8") == 0) {
                printf("Process_8 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 8;
            } else if (strcmp(argv[0], "process_9") == 0) {
                printf("Process_9 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 9;
            } else if (strcmp(argv[0], "process_10") == 0) {
                printf("Process_10 identified.Printing process argv[0] : %s\n", argv[0]);
                PROC_NUM = 10;
            }
            printf("Signal received from the child process!\n");

            obj.counter = PROC_NUM;

            printf("Received counter: %d\n", obj.counter);
            printf("Received init_x: %f\n", obj.init_x);
            printf("Received init_y: %f\n", obj.init_y);
            LOOP_CON = 0;
            sem_post(&sem);
        }
    }
    // sem_close(&sem);
    return 0;
}

void signal_handler(int sig) {
    LOOP_CON = 1;
}

