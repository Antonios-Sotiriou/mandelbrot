#include <stdlib.h>

#include "header_files/palette.h"
#include "header_files/objects.h"
//#include "header_files/mutexes.h"

void painter(Object obj) {

    double a = (obj.x - (obj.winattr->width / obj.horiz)) / (obj.winattr->width / obj.zoom) + obj.init_x;
    double b = (obj.y - (obj.winattr->height / obj.vert)) / (obj.winattr->height / obj.zoom) + obj.init_y;
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
        //pthread_mutex_lock(&iterMutex);
        obj.image_data[obj.counter] =  1000 - n;
        obj.image_data[obj.counter + 1] = (1000 - n) * 2;
        obj.image_data[obj.counter + 2] = 0;
        //pthread_mutex_unlock(&iterMutex);
    } //else if (n == obj.max_iter) {
    //     obj.image_data[obj.counter] = 0;
    //     obj.image_data[obj.counter + 1] = 0;
    //     obj.image_data[obj.counter + 2] = 0;
    // }
}

