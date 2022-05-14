#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header_files/threader.h"
#include <signal.h>
#include "header_files/objects.h"
#include "header_files/palette.h"

int iterator(Object obj, int pids[]) {

    obj.image_data = calloc(1, sizeof(char) * obj.winattr->width * obj.winattr->height * 4);
    if (obj.image_data == NULL) {
        printf("Not Enough memory for the image_data to be allocated...Exit status 1\n");
        free(obj.image_data);
        exit(1);
    }
    kill(pids[0], SIGUSR1); ////////////////////////////////////////////////////
    kill(pids[1], SIGUSR1); ////////////////////////////////////////////////////
    kill(pids[2], SIGUSR1); ////////////////////////////////////////////////////
    kill(pids[3], SIGUSR1); ////////////////////////////////////////////////////
    threader(&obj);

    XImage *image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, obj.image_data, obj.winattr->width, obj.winattr->height, 32, 0);

    Pixmap pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);

    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);

    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    free(obj.image_data);
    XFree(image);

    return 0;
}
