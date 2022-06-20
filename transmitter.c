#ifndef _TRANSMITTER_H
    #include "header_files/transmitter.h"
#endif

// Global Variables
sem_t *transem;
extern int shimage_id;

// this global variable is used only from this file and only from transmitter function.
static int WAIT_CON = 0;

static const void transmitter_handler(int sig);

const int transmitter(const Object obj, const int pids[]) {

    XImage *image;
    XGCValues gcvals;
    gcvals.graphics_exposures = False;
    GC gc = XCreateGC(obj.displ, obj.win, GCGraphicsExposures, &gcvals);

    // semaphores initialization
    if (unlinksem("/transem"))
        fprintf(stderr, "Warning: Transmitter - /transem - unlinksem()\n");

    transem = opensem("/transem", O_CREAT, 0666, PROC_NUM);
    if (transem == SEM_FAILED)
        fprintf(stderr, "Warning: Transmitter - /transem - opensem()\n");

    // signal register area to receive signal from child processes.
    struct sigaction sig = { 0 };
    sig.sa_handler = &transmitter_handler;;
    if (sigaction(SIGRTMIN, &sig, NULL) == -1) {
        fprintf(stderr, "Warning: transmitter - sigaction()\n");
        return EXIT_FAILURE;
    }

    // image data pointer
    char *sh_image;
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
    XPutImage(obj.displ, obj.win, gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);
    XFree(image);
    XFreeGC(obj.displ, gc);

    // closing the semaphore which used in main2.c because we can't close it there.
    if (closesem(transem))
        fprintf(stderr, "Warning: Transmitter - transem - closesem()\n");
    
    if (dtshmem(sh_image))
        fprintf(stderr, "Warning: Transmitter - sh_image - dtshmem()\n");

    return EXIT_SUCCESS;
}

static const void transmitter_handler(int sig) {

    if (waitsem(transem))
        fprintf(stderr, "Warning: Main2 - mainsem - waitsem()\n");

    WAIT_CON++;

    if (postsem(transem))
        fprintf(stderr, "Warning: Main2 - mainsem - waitsem()\n");
}

