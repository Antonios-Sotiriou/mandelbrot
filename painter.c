// general headers
#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

// Project specific headers
#ifndef _PALETTE_H
    #include "header_files/palette.h"
#endif
#ifndef _OBJECTS_H
    #include "header_files/objects.h"
#endif

// Macro to help us with the computations
#ifndef SCALES
    #define SCALES 1
    #define XSCALE (knot.x - (knot.width / knot.horiz)) / (knot.width / knot.zoom) + knot.init_x;
    #define YSCALE (knot.y - (knot.height / knot.vert)) / (knot.height / knot.zoom) + knot.init_y;
#endif

void painter(const KNOT knot, char *image_data) {

    double a = XSCALE;
    double b = YSCALE;
    double curr_a = a;
    double curr_b = b;

    int n = 0;
    while (n < knot.max_iter) {
        double iter_a = (a * a) - (b * b);
        double iter_b = 2 * a * b;
        a = iter_a + curr_a;
        b = iter_b + curr_b;

        if (abs(a + b) > 4) {
            break;
        }
        n++;
    }
    if (n < knot.max_iter && n >= 255)
        n = n / 100;
    if (n < knot.max_iter && n < 255) {
        image_data[knot.counter] =  n + 10;
        image_data[knot.counter + 1] = n + n;
        image_data[knot.counter + 2] =  n * 10;
    } else {
        if (image_data[knot.counter] != 0x00 || image_data[knot.counter + 1] != 0x00 || image_data[knot.counter + 2] != 0x00) {
            image_data[knot.counter] =  0x00;
            image_data[knot.counter + 1] = 0x00;
            image_data[knot.counter + 2] = 0x00;
        }
    }
}

