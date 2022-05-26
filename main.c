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

    // object to transfer between processes the integer variables for each process calculations.
    KNOT *shknot;
    key_t key = ftok("./knot_key", 9988);
    int shknotid = shmget(key, sizeof(KNOT), 0666 | IPC_CREAT);
    if (shknotid == -1) {
        perror("Main - shmget()");
        return 1;
    }
    shknot = shmat(shknotid, NULL, 0);
    if (shknot == NULL) {
        perror("Main - shmat()");
        return 1;
    }

    // image data pointer
    char *shmem_2;
    key_t key_2 = 9998;                        //  The calculations here are not dynamic.
    int shmid_2 = shmget(key_2, sizeof(char) * WIDTH * HEIGHT * 4, 0666 | IPC_CREAT);
    if (shmid_2 == -1) {
        perror("Main - shmget()");
        return 1;
    }
    shmem_2 = shmat(shmid_2, NULL, 0);
    if (shmem_2 == NULL) {
        perror("Main - shmat()");
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

    