#include <stdio.h>
#include <X11/Xlib.h>
#include <pthread.h>

#include "header_files/palette.h"
#include "header_files/objects.h"
#include "header_files/mutexes.h"

int *painter(void *object) {

    pthread_mutex_lock(&mutex);
    
    Object *obj = object;
    double a = (obj->x - (obj->winattr->width / obj->horiz)) / (obj->winattr->width / obj->zoom) + obj->init_x;
    double b = (obj->y - (obj->winattr->height / obj->vert)) / (obj->winattr->height / obj->zoom) + obj->init_y;
    double curr_a = a;
    double curr_b = b;

    int n = 0;
    while (n < obj->max_iter) {
        double iter_a = (a * a) - (b * b);
        double iter_b = 2 * a * b;
        a = iter_a + curr_a;
        b = iter_b + curr_b;

        if (a + b > 4.00) {
            break;
        }
        n++;
    }
    if (n == obj->max_iter) {
        obj->values.foreground = pallete[n];
        XChangeGC(obj->displ, obj->gc, GCForeground, &obj->values);
        XDrawPoint(obj->displ, obj->win, obj->gc, obj->x, obj->y);                                
    } else {
        obj->values.foreground = pallete[n];
        XChangeGC(obj->displ, obj->gc, GCForeground, &obj->values);
        XDrawPoint(obj->displ, obj->win, obj->gc, obj->x, obj->y);                                                    
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

