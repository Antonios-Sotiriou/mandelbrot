#include "header_files/objects.h"
#include "header_files/painter.h"
#include "header_files/threader.h"
#include "header_files/mutexes.h"

void *threader(void *object) {

    pthread_mutex_lock(&mutex);

    Object *obj = object;
    int counter = obj->step_counter; // pixels counter.Its incremented by four to include rgba pixel values
    int x = obj->step_x;
    int y = obj->step_y;
    for (int i = obj->start_point; i <= obj->step_point; i++) {

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

    obj->step_x = x;
    obj->step_y = y;
    obj->step_counter = counter;

    pthread_mutex_unlock(&mutex);

    return 0;
}

