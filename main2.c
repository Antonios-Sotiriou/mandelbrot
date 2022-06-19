#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

#include "header_files/shmem.h"
#include "header_files/objects.h"
#include "header_files/global_constants.h"
#include "header_files/threader.h"
#include "header_files/procsync.h"

// some usefull Macros
#define EMVADON (knot.width * knot.height)

// this global variable is used only from this file and only from main function.
int LOOP_CON = 0;

void signal_handler(int sig);

int main(int argc, char *argv[]) {

    // semaphores initialization
    sem_t *mainsem;
    if (unlinksem("/mainsem"))
        fprintf(stderr, "Warning: Transmitter - /mainsem - unlinkif()\n");

    mainsem = opensem("/mainsem", O_CREAT, 0666, PROC_NUM);
    if (mainsem == SEM_FAILED)
        fprintf(stderr, "Warning: Transmitter - /mainsem - opensem()\n");

    struct sigaction sig = { 0 };
    sig.sa_handler = &signal_handler;
    int sig_val = sigaction(SIGUSR1, &sig, NULL);
    if (sig_val == -1) {
        perror("Main2 - sigaction()");
        return EXIT_FAILURE;
    }
    // Grab the shared memory to retrieve the knot was putted there by main.c and modified by board.c.
    KNOT knot, *sh_knot;
    key_t knot_key = gorckey("./keys/knot_key.txt", 9988);
    int shknot_id = crshmem(knot_key, sizeof(KNOT), SHM_RDONLY);
    if (shknot_id == -1)
        fprintf(stderr, "Warning: Main2 - shknot_id - crshmem()\n");

    int proc_id = atoi(argv[0]);
    printf("Process_id : %d\n", proc_id);
    
    while (!sig_val) {

        if (waitsem(mainsem))
            fprintf(stderr, "Warning: Main2 - mainsem - waitsem()\n");

        if (LOOP_CON) {

            sh_knot = attshmem(shknot_id, NULL, SHM_RND);
            if (sh_knot == NULL)
                fprintf(stderr, "Warning: Main2 - sh_knot - attshmem()\n");

            knot = *sh_knot;
            knot.step_counter = ((EMVADON / PROC_NUM) * 4) * proc_id;
            knot.step_y = (knot.height / PROC_NUM) * proc_id;

            if(threader(knot) != 0) {
                perror("Main2.c - threader()");
                return EXIT_FAILURE;
            }
            LOOP_CON = 0;
            if (postsem(mainsem))
                fprintf(stderr, "Warning: Main2 - maisem - postsem()\n");

            if (dtshmem(sh_knot))
                fprintf(stderr, "Warning: Main2 - sh_knot - dtshmem()\n");
        }
    }

    return EXIT_SUCCESS;
}

void signal_handler(int sig) {
    LOOP_CON = 1;
}

