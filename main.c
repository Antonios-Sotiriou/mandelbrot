// general headers
#include <stdio.h>

// multiprocessing includes
#include <unistd.h>
#include <sys/wait.h>

// object specific headers
#include "header_files/locale.h"
#include "header_files/objects.h"
#include "header_files/board.h"

int main(int argc, char *argv[]) {
    
    locale_init();

    int pids[10];
    char *process_num[10] = { "process_1", "process_2", "process_3", "process_4", "process_5", "process_6", "process_7", "process_8", "process_9", "process_10" };
    for (int i = 0; i < 10; i++) {
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