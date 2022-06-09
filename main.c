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
    char *process_num[10] = { "process_1", "process_2", "process_3", "process_4", "process_5", "process_6", "process_7", "process_8", "process_9", "process_10" };
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

        // FIFO IMPLEMENTATION

    // fifo headers for write only
    // #include <sys/stat.h>
    // error simillar headers
    // #include <errno.h>
    // if (mkfifo("./mdfifo", 0666) == -1) {
    //     if (errno != EEXIST) {
    //         perror("mkfifo()");
    //         return 1;
    //     }
    // }

    // fifo headers for read only
    // #include <fcntl.h>
    // int fd = open("./mdfifo", O_WRONLY);
    // if (write(fd, &obj, sizeof(Object)) == -1) {
    //     perror("write() fifo...");
    // }
    // close(fd);

    