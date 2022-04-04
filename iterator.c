#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header_files/objects.h"
#include "header_files/painter.h"
#include "header_files/palette.h"

int iterator(Object obj) {

    unsigned char *image_data;
    image_data = calloc(1, sizeof(char) * obj.winattr->width * obj.winattr->height * 4);
    if (image_data == NULL) {
        printf("Not Enough memory for the image_data to be allocated...Exit status 1");
        free(image_data);
        exit(1);
    }

    int counter = 0;
    int y = 0;
    int x = 0;
    for (int i = 0; i <= obj.winattr->width * obj.winattr->height; i++) {

        if (x == obj.winattr->width) {
            y += 1;
            x = 0;
        }

        double a = (x - (obj.winattr->width / obj.horiz)) / (obj.winattr->width / obj.zoom) + obj.init_x;
        double b = (y - (obj.winattr->height / obj.vert)) / (obj.winattr->height / obj.zoom) + obj.init_y;
        double curr_a = a;
        double curr_b = b;

        int n = 0;
        while (n < obj.max_iter) {
            double iter_a = (a * a) - (b * b);
            double iter_b = 2 * a * b;
            a = iter_a + curr_a;
            b = iter_b + curr_b;

            if (abs(a + b) > 4) {
                break;
            }
            n++;
        }
        if (n < obj.max_iter) {
            image_data[counter] =  n + n;
            image_data[counter + 1] = n * 2;
            image_data[counter + 2] = 0;
            // printf("Color : %f\n", (n + 1) - log(log2(abs(a + b))));
        } else if (n == obj.max_iter) {
            image_data[counter] = 0;
            image_data[counter + 1] = 0;
            image_data[counter + 2] = 0;
        }
        counter += 4;
        x++;
    }

    XImage *image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, (char*)image_data, obj.winattr->width, obj.winattr->height, 32, 0);

    Pixmap pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);

    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);

    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    free(image_data);
    XFree(image);

    return 0;
}


