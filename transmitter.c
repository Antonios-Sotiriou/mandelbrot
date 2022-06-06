// general headers
#ifndef _STDIO_H
    #include <stdio.h>
#endif
#ifndef _STDLIB_H
    #include <stdlib.h>
#endif
#ifndef _ERRNO_H
    #include <errno.h>
#endif

// shared memory
#ifndef _SYS_IPC_H
    #include <sys/ipc.h>
#endif
#ifndef _SYS_SHM_H
    #include <sys/shm.h>
#endif
#ifndef _SEMAPHORE_H
    #include <semaphore.h>
#endif

/* semaphore specific includes */
#ifndef _FCNTL_H
    #include <fcntl.h>
#endif
#ifndef _SYS_STAT_H
    #include <sys/stat.h>
#endif

// signals
#ifndef _SIGNAL_H
    #include <signal.h>
#endif

// Project specific headers
#ifndef _SHMEM_H
    #include "header_files/shmem.h"
#endif
#ifndef _OBJECTS_H
    #include "header_files/objects.h"
#endif
#ifndef _GLOBAL_VARS_H
    #include "header_files/global_vars.h"
#endif
#ifndef _THREADER_H
    #include "header_files/threader.h"
#endif
#ifndef _TRANSEM_H
    #include "header_files/transem.h"
#endif

// this global variable is used only from this file and only from transmitter function.
int WAIT_CON = 0;

void transmitter_handler(int sig);

const int transmitter(const Object obj, const int pids[]) {

    XImage *image;
    Pixmap pixmap;
    
    // semaphores initialization
    if (sem_unlink("/transem") == -1){
        if (errno != ENOENT) {
            perror("Transmitter - sem_unlink()");
            return EXIT_FAILURE;
        }
    }
    transem = sem_open("/transem", O_CREAT, 0666, PROC_NUM);
    if (transem == SEM_FAILED) {
        perror("Transmitter - sem_open()");
        return EXIT_FAILURE;
    }

    // signal register area to receive signal from child processes.
    struct sigaction sig = { 0 };
    sig.sa_handler = &transmitter_handler;;
    if (sigaction(SIGRTMIN, &sig, NULL) == -1) {
        perror("Transmitter - sigaction()");
        return EXIT_FAILURE;
    }

    // image data pointer
    char *sh_image;
    key_t image_key = gorckey("./keys/image_key.txt", 8899);
    int shimage_id = crshmem(image_key, obj.winattr->width * obj.winattr->height * 4, SHM_RDONLY);
    if (shimage_id == -1)
        fprintf(stderr, "Warning: Transmitter - shimage_id - crshmem()\n");

    sh_image = attshmem(shimage_id, NULL, SHM_RND);
    if (sh_image == NULL)
        fprintf(stderr, "Warning: Transmitter - sh_image - attshmem()\n");

    // thats the signal for the child process to write in the shared memory...
    for (int i = 0; i < PROC_NUM; i++) {
        kill(pids[i], SIGUSR1);
    }

    // waiting for the threader signal to break out of this waiting loop.
    while (1) {
        if (WAIT_CON == PROC_NUM) {
            WAIT_CON = 0;
            break;
        }
    }

    image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, sh_image, obj.winattr->width, obj.winattr->height, 32, 0);
    pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);
    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);
    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    XFree(image);
    XFreePixmap(obj.displ, pixmap);

    // closing the semaphore which used in main2.c because we can't close it there.
    if (sem_close(transem) == -1) {
        perror("Transmitter - sem_close()");
        return EXIT_FAILURE;
    }
    
    if (dtshmem(sh_image) == -1)
        fprintf(stderr, "Warning: Transmitter - sh_image - dtshmem()\n");

    return EXIT_SUCCESS;
}

void transmitter_handler(int sig) {
    sem_wait(transem);
    WAIT_CON++;
    sem_post(transem);
}

