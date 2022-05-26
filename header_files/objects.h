#ifndef OBJECTS_H
#define OBJECTS_H

#include <X11/Xlib.h>

typedef struct {
    Display *displ;
    Window win;
    GC gc;
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

typedef struct {
    int proc_num;
    int width;
    int height;
    int max_iter;
    int counter;
    int x;
    int y;
    double horiz; 
    double vert;
    double zoom; 
    double init_x; 
    double init_y;
    int step_counter;
    int step_x;
    int step_y;
} KNOT;

#endif /* OBJECTS_H */

