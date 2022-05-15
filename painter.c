#include <stdio.h>
#include <stdlib.h>

#include "header_files/palette.h"
#include "header_files/objects.h"

void painter(Object obj) {

    double a = (obj.x - (800 / obj.horiz)) / (800 / obj.zoom) + obj.init_x;
    double b = (obj.y - (800 / obj.vert)) / (800 / obj.zoom) + obj.init_y;
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
        obj.image_data[obj.counter] =  n + n;
        printf("pAINTING IMAGE\n");
        obj.image_data[obj.counter + 1] = n * 2;
        obj.image_data[obj.counter + 2] = 0;
    } else if (n == obj.max_iter) {
        obj.image_data[obj.counter] = 0;
        obj.image_data[obj.counter + 1] = 0;
        obj.image_data[obj.counter + 2] = 0;
        printf("pAINTING IMAGE\n");
    }
}

