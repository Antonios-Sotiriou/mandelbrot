#include "header_files/objects.h"
// #include "header_files/mutexes.h"
#include "header_files/painter.h"
#include "header_files/threader.h"
#include <stdio.h>
#include <stdlib.h>

void *threader(void *object) {

    // pthread_mutex_lock(&iterMutex);

    Object *obj = object;
    int counter = obj->step_counter;
    printf("Step counter: %d\n", obj->step_counter);
    int x = 0;
    int y = 0;

    for (int i = 0; i < obj->winattr->width * obj->winattr->height / 10; i++) {

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
    }

    // pthread_mutex_unlock(&iterMutex);

    return 0;
}

