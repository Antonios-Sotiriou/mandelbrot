#include <stdio.h>
#include <string.h>
#include <errno.h>

// multiprocessing includes
#include <unistd.h>
#include <semaphore.h>
/* semaphore specific includes */
#include <fcntl.h>
#include <sys/stat.h>

// signal
#include <signal.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>

#include "header_files/objects.h"
#include "header_files/global_vars.h"
#include "header_files/threader.h"
#include "header_files/mainsem.h"

// this global variable is used only from this file and only from main function.
int LOOP_CON = 0;

void signal_handler(int sig);

int main(int argc, char *argv[]) {

    // semaphore initialization
    if (sem_unlink("/mainsem") == -1){
        if (errno != ENOENT) {
            perror("Main2 - sem_unlink()");
            return 1;
        }
    }
    mainsem = sem_open("/mainsem", O_CREAT, 0666, PROC_NUM);
    if (mainsem == SEM_FAILED) {
        perror("Main2 - sem_open()");
        return 1;
    }

    struct sigaction sig = { 0 };
    sig.sa_handler = &signal_handler;
    int sig_val = sigaction(SIGUSR1, &sig, NULL);
    if (sig_val == -1) {
        perror("Main2 - sigaction()");
        return 1;
    }

    KNOT knot, *sh_knot;
    key_t knot_key = ftok("./keys/knot_key.txt", 9988);
    int shknot_id = shmget(knot_key, sizeof(KNOT), 0666);
    if (shknot_id == -1) {
        perror("Main2 - shknot_id shmget()");
        return 1;
    }
    
    while (!sig_val) {

        if (sem_wait(mainsem) == -1) {
            if (errno != EINTR) {
                perror("Main2 - sem_wait()");
                return 1;
            }
        }
        if (LOOP_CON) {

            sh_knot = shmat(shknot_id, NULL, 0);
            if (sh_knot == NULL) {
                perror("Main2 - sh_knot shmat()");
                return 1;
            }

            knot = *sh_knot;
            knot.step_x = 0;

            if (strcmp(argv[0], "process_1") == 0) {
                knot.step_counter = 0;
                knot.step_y = 0;
            } else if (strcmp(argv[0], "process_2") == 0) {
                knot.step_counter = (((knot.width * knot.height) / PROC_NUM) * 4);
                knot.step_y = knot.height / PROC_NUM;
            } else if (strcmp(argv[0], "process_3") == 0) {
                knot.step_counter = ((((knot.width * knot.height) / PROC_NUM) * 2) * 4);
                knot.step_y = (knot.height / PROC_NUM) * 2;
            } else if (strcmp(argv[0], "process_4") == 0) {
                knot.step_counter = ((((knot.width * knot.height) / PROC_NUM) * 3) * 4);
                knot.step_y = (knot.height / PROC_NUM) * 3;
            } else if (strcmp(argv[0], "process_5") == 0) {
                knot.step_counter = ((((knot.width * knot.height) / PROC_NUM) * 4) * 4);
                knot.step_y = (knot.height / PROC_NUM) * 4;
            } else if (strcmp(argv[0], "process_6") == 0) {
                knot.step_counter = ((((knot.width * knot.height) / PROC_NUM) * 5) * 4);
                knot.step_y = (knot.height / PROC_NUM) * 5;
            } else if (strcmp(argv[0], "process_7") == 0) {
                knot.step_counter = ((((knot.width * knot.height) / PROC_NUM) * 6) * 4);
                knot.step_y = (knot.height / PROC_NUM) * 6;
            } else if (strcmp(argv[0], "process_8") == 0) {
                knot.step_counter = ((((knot.width * knot.height) / PROC_NUM) * 7) *4);
                knot.step_y = (knot.height / PROC_NUM) * 7;
            } else if (strcmp(argv[0], "process_9") == 0) {
                knot.step_counter = ((((knot.width * knot.height) / PROC_NUM) * 8) * 4);
                knot.step_y = (knot.height / PROC_NUM) * 8;
            } else if (strcmp(argv[0], "process_10") == 0) {
                knot.step_counter = ((((knot.width * knot.height) / PROC_NUM) * 9) * 4);
                knot.step_y = (knot.height / PROC_NUM) * 9;
            }

            if(threader(knot) != 0) {
                perror("Main2.c - threader()");
                return 1;
            }
            LOOP_CON = 0;
            if (sem_post(mainsem) == -1) {
                perror("Main2 - sem_post()");
                return 1;
            }
            if(shmdt(sh_knot) == -1) {
                perror("Main2 - sh_knot shmdt()");
                return 1;
            }
        }
    }

    return 0;
}

void signal_handler(int sig) {
    LOOP_CON = 1;
}

