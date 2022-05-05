#ifndef OBJECTS_H
#define OBJECTS_H

#include <X11/Xlib.h>

typedef struct {
    Display *displ;
    Window win;
    GC gc;
    XGCValues values;
    XWindowAttributes *winattr;
    char *image_data;
    int max_iter;
    int counter;
    int step_counter;
    int x;
    int step_x;
    int y;
    int step_y;
    int start_point;
    int step_point;
    double horiz; 
    double vert;
    double zoom; 
    double init_x; 
    double init_y;
} Object;

#endif /* OBJECTS_H */

