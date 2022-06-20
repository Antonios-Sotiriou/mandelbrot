#ifndef _THREADER_H
    #include "header_files/threader.h"
#endif
#ifndef _PAINTER_H
    #include "header_files/painter.h"
#endif

// some usefull Macros
#ifndef EMVADON
   #define EMVADON (knot.width * knot.height)
#endif
    #include <string.h>

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

    if (dtshmem(sh_image))
        fprintf(stderr, "Warning: Threader - sh_image - dtshmem()\n");

    return EXIT_SUCCESS;
}

