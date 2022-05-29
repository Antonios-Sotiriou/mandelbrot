#include <stdio.h>
#include <unistd.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

// signals
#include <signal.h>

// object specific headers
#include "header_files/objects.h"
#include "header_files/global_vars.h"
#include "header_files/painter.h"

int threader(KNOT knot) {

    char *sh_image;
    key_t image_key = ftok("./keys/image_key.txt", 8899);
    int shimage_id = shmget(image_key, knot.width * knot.height * 4, 0666);
    if (shimage_id == -1) {
        perror("Threader - shimage_id shmget()");
        return 1;
    }
    sh_image = shmat(shimage_id, NULL, 0);
    if (sh_image == NULL) {
        perror("Threader - sh_image shmat()");
        return 1;
    }

    int counter = knot.step_counter;
    int x = knot.step_x;
    int y = knot.step_y;

    for (int i = 0; i < (knot.width * knot.height) / PROC_NUM; i++) {

        if (x == knot.width) {
            y += 1;
            x = 0;
        }

        knot.x = x;
        knot.y = y;
        knot.counter = counter;
        painter(knot, sh_image);
        counter += 4;
        x++;
    }

    kill(getppid(), SIGRTMIN);
    if(shmdt(sh_image) == -1) {
        perror("Threader - sh_image shmdt()");
        return 1;
    }
    return 0;
}

