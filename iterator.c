#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header_files/objects.h"
#include "header_files/painter.h"

int iterator(Object obj) {

    for (int x = 0; x <= obj.winattr->width; x++) {
        for (int y = 0; y <= obj.winattr->height; y++) {

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

                if (abs(a + b) > 16) {
                    break;
                }
                n++;
            }
            painter(obj, n, x, y);
        }
    }
    return 0;
}

