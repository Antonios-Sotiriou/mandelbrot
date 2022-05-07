#include "header_files/objects.h"
#include "header_files/mutexes.h"
#include "header_files/painter.h"
#include "header_files/threader.h"
#include <stdio.h>
#include <stdlib.h>

void *threader(void *object) {

    pthread_mutex_lock(&iterMutex);

    Object *obj = object;
    int counter = obj->step_counter; // pixels counter.Its incremented by four to include rgba pixel values
    int x = obj->step_x;
    int y = obj->step_y;
    printf("Start_point: %d\n", obj->start_point);
    for (int i = obj->start_point; i < obj->step_point; i++) {

        if (x == obj->winattr->width) {
            y += 1;
            x = 0;
        }

        obj->x = x;
        obj->y = y;
        obj->counter = counter;
        painter(*obj);
        counter += 4;
        x++;
        printf("Value of x: %d\n", i);
    }
    //printf("Value of x: %d\nValue of y: %d\n", x, y);

    obj->step_x = x;
    obj->step_y = y;
    obj->step_counter = counter;

    pthread_mutex_unlock(&iterMutex);

    return 0;
}

