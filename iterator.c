#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "header_files/objects.h"
#include "header_files/painter.h"
#include "header_files/palette.h"

int iterator(Object obj) {

    obj.image_data = calloc(1, sizeof(char) * obj.winattr->width * obj.winattr->height * 4);
    if (obj.image_data == NULL) {
        printf("Not Enough memory for the image_data to be allocated...Exit status 1");
        free(obj.image_data);
        exit(1);
    }

    int counter = 0; // pixels counter.Its incremented by four to include rgba pixel values
    int x = 0;
    int y = 0;
    for (int i = 0; i <= obj.winattr->width * obj.winattr->height; i++) {

        if (x == obj.winattr->width) {
            y += 1;
            x = 0;
        }

        obj.x = x;
        obj.y = y;
        obj.counter = counter;
        painter(obj);
        counter += 4;
        x++;
    }

    XImage *image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, obj.image_data, obj.winattr->width, obj.winattr->height, 32, 0);

    Pixmap pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);

    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);

    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    free(obj.image_data);
    XFree(image);

    return 0;
}


