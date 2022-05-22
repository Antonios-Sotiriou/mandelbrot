#include <stdio.h>
#include <string.h>

// multiprocessing includes
#include <unistd.h>
#include <semaphore.h>

// signal
#include <signal.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>

// Time included for testing execution time
#include <time.h>

#include "header_files/objects.h"
#include "header_files/threader.h"
#include "header_files/sem.h"
#include "header_files/sem_th.h"

int LOOP_CON = 0;

void signal_handler(int sig);

int main(int argc, char *argv[]) {

    sem_init(&sem, 1, 10);

    struct sigaction sig = { 0 };
    sig.sa_handler = &signal_handler;
    int sig_val = sigaction(SIGUSR1, &sig, NULL);

    KNOT knot, *shmem;
    key_t key = ftok("./knot_key", 9988);
    int shmid = shmget(key, sizeof(KNOT), 0666);
    shmem = shmat(shmid, NULL, 0);
    
    while (!sig_val) {
        sem_wait(&sem);
        if (LOOP_CON) {

            knot = *shmem;

            if (strcmp(argv[0], "process_1") == 0) {
                // printf("Process_1 ##################################################: %s\n", argv[0]);
                knot.step_counter = 0;
                knot.step_x = 0;
                knot.step_y = 0;
            } else if (strcmp(argv[0], "process_2") == 0) {
                // printf("Process_2 ##########: %s\n", argv[0]);
                knot.step_counter = (((knot.width * knot.height) / 10) * 4);
                knot.step_x = 0;
                knot.step_y = knot.height / 10;
            } else if (strcmp(argv[0], "process_3") == 0) {
                // printf("Process_3 ##########: %s\n", argv[0]);
                knot.step_counter = ((((knot.width * knot.height) / 10) * 2) * 4);
                knot.step_x = 0;
                knot.step_y = (knot.height / 10) * 2;
            } else if (strcmp(argv[0], "process_4") == 0) {
                // printf("Process_4 ##########: %s\n", argv[0]);
                knot.step_counter = ((((knot.width * knot.height) / 10) * 3) * 4);
                knot.step_x = 0;
                knot.step_y = (knot.height / 10) * 3;
            } else if (strcmp(argv[0], "process_5") == 0) {
                // printf("Process_5 ##########: %s\n", argv[0]);
                knot.step_counter = ((((knot.width * knot.height) / 10) * 4) * 4);
                knot.step_x = 0;
                knot.step_y = (knot.height / 10) * 4;
            } else if (strcmp(argv[0], "process_6") == 0) {
                // printf("Process_6 ##########: %s\n", argv[0]);
                knot.step_counter = ((((knot.width * knot.height) / 10) * 5) * 4);
                knot.step_x = 0;
                knot.step_y = (knot.height / 10) * 5;
            } else if (strcmp(argv[0], "process_7") == 0) {
                // printf("Process_7 ##########: %s\n", argv[0]);
                knot.step_counter = ((((knot.width * knot.height) / 10) * 6) * 4);
                knot.step_x = 0;
                knot.step_y = (knot.height / 10) * 6;
            } else if (strcmp(argv[0], "process_8") == 0) {
                // printf("Process_8 ##########: %s\n", argv[0]);
                knot.step_counter = ((((knot.width * knot.height) / 10) * 7) *4);
                knot.step_x = 0;
                knot.step_y = (knot.height / 10) * 7;
            } else if (strcmp(argv[0], "process_9") == 0) {
                // printf("Process_9 ##########: %s\n", argv[0]);
                knot.step_counter = ((((knot.width * knot.height) / 10) * 8) * 4);
                knot.step_x = 0;
                knot.step_y = (knot.height / 10) * 8;
            } else if (strcmp(argv[0], "process_10") == 0) {
                // printf("Process_10 ##########: %s\n", argv[0]);
                knot.step_counter = ((((knot.width * knot.height) / 10) * 9) * 4);
                knot.step_x = 0;
                knot.step_y = (knot.height / 10) * 9;
            }

            threader(knot);
            LOOP_CON = 0;
            sem_post(&sem);
        }
    }

    return 0;
}

void signal_handler(int sig) {
    LOOP_CON = 1;
}

