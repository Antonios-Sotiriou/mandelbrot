#ifndef _OBJECTS_H
#define _OBJECTS_H 1

#ifndef _X11_XLIB_H_
    #include <X11/Xlib.h>
#endif

typedef struct {
    Display *displ;
    Window win;
    XWindowAttributes *winattr;
    int screen;
    unsigned char *image_data;
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
    unsigned int width;
    unsigned int height;
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

#endif /* _OBJECTS_H */

