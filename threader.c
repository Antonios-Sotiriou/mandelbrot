// general headers
#ifndef _STDIO_H
    #include <stdio.h>
#endif
#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

// multiprocessing includes
#ifndef _UNISTD_H
    #include <unistd.h>
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
// Macro to help us with the computations in painter function.Thats why we define it before the painter include and under the object include.
#ifndef SCALES
    #define SCALES 1
    #define XSCALE (knot.x - (knot.width / knot.horiz)) / (knot.width / knot.zoom) + knot.init_x;
    #define YSCALE (knot.y - (knot.height / knot.vert)) / (knot.height / knot.zoom) + knot.init_y;
#endif

#ifndef _PAINTER_H
    #include "header_files/painter.h"
#endif
// some usefull Macros
#ifndef EMVADON
   #define EMVADON (knot.width * knot.height)
#endif

int threader(KNOT knot) {

    char *sh_image;
    key_t image_key = gorckey("./keys/image_key.txt", 8899);
    int shimage_id = crshmem(image_key, EMVADON * 4, SHM_RDONLY);
    if (shimage_id == -1)
        fprintf(stderr, "Warning: Threader - shimage_id - crshmem()\n");

    sh_image = attshmem(shimage_id, NULL, SHM_RND);
    if (sh_image == NULL)
        fprintf(stderr, "Warning: Threader - sh_image - attshmem()\n");

    int counter = knot.step_counter;
    int x = knot.step_x;
    int y = knot.step_y;

    for (int i = 0; i < EMVADON / PROC_NUM; i++) {

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

    if (dtshmem(sh_image) == -1)
        fprintf(stderr, "Warning: Threader - sh_image - dtshmem()\n");

    return EXIT_SUCCESS;
}

