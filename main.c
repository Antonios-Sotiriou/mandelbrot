// general headers
#include <stdio.h>
#include <stdlib.h>

// multiprocessing includes
#include <unistd.h>
#include <sys/wait.h>

// // shared memory
#include <sys/ipc.h>
#include <sys/shm.h>

// Project specific headers
#include "header_files/locale.h"
#include "header_files/shmem.h"
#include "header_files/objects.h"
#include "header_files/global_vars.h"
#include "header_files/board.h"

int main(int argc, char *argv[]) {

    if (locale_init())
        fprintf(stderr, "Warning: Main -locale()\n");

    // object to transfer between processes which contains the integer variables for each process calculations.
    key_t knot_key = gorckey("./keys/knot_key.txt", 9988);
    if (knot_key == -1)
        fprintf(stderr, "Warning: Main - knot_key - gorckey()\n");

    if (crshmem(knot_key, sizeof(KNOT), 0666 | IPC_CREAT) == -1)
        fprintf(stderr, "Warning: Main - crshmem()\n");

    int pids[PROC_NUM];
    char *process_num[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    for (int i = 0; i < PROC_NUM; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("Main - fork()");
            return EXIT_FAILURE;
        } else if (pids[i] == 0) {
            // child process
            execlp("./main2", process_num[i], NULL);
        }
    }

    board(pids);
    wait(NULL);
    
    return EXIT_SUCCESS;
}

    