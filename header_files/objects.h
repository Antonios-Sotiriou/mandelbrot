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
    int x;
    int y;
    double horiz; 
    double vert;
    double zoom; 
    double init_x; 
    double init_y;
} Object;

#endif /* OBJECTS_H */

