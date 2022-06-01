// general headers
#include <stdio.h>

// multiprocessing includes
#include <unistd.h>
#include <sys/wait.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>

// object specific headers
#include "header_files/locale.h"
#include "header_files/objects.h"
#include "header_files/global_vars.h"
#include "header_files/board.h"

int main(int argc, char *argv[]) {
    
    locale_init();

    // object to transfer between processes which contains the integer variables for each process calculations.
    key_t knot_key = ftok("./keys/knot_key.txt", 9988);
    if (knot_key == -1) {
        perror("Main - knot_key ftok()");
        return 1;
    }
    int shknot_id = shmget(knot_key, sizeof(KNOT), 0666 | IPC_CREAT);
    if (shknot_id == -1) {
        perror("Main - shknot_id shmget()");
        return 1;
    }

    int pids[PROC_NUM];
    char *process_num[10] = { "process_1", "process_2", "process_3", "process_4", "process_5", "process_6", "process_7", "process_8", "process_9", "process_10" };
    for (int i = 0; i < PROC_NUM; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork()");
            return 1;
        } else if (pids[i] == 0) {
            // child process
            execlp("./main2", process_num[i], NULL);
        }
    }

    board(pids);
    wait(NULL);
    
    return 0;
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

    