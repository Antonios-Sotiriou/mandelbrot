#include <stdlib.h>

#include "header_files/palette.h"
#include "header_files/objects.h"

// headers to be deleted
#include <stdio.h>

void painter(const KNOT knot, char *shmem) {

    double a = (knot.x - (knot.width / knot.horiz)) / (knot.width / knot.zoom) + knot.init_x;
    double b = (knot.y - (knot.height / knot.vert)) / (knot.height / knot.zoom) + knot.init_y;
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
    
    if (n < knot.max_iter) {
        shmem[knot.counter] =  n + n;
        shmem[knot.counter + 1] = n * 2;
        shmem[knot.counter + 2] = 0;
    }
}

