#include "header_files/objects.h"
#include "header_files/painter.h"
#include "header_files/threader.h"

void *threader(void *object) {

    Object *obj = object;
    int counter = 0; // obj->step_counter // pixels counter.Its incremented by four to include rgba pixel values
    int x = 0; // obj->step_x
    int y = 0; // obj->step_y
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
    return 0;
}

