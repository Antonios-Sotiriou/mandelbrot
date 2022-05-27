#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

extern double XCONST;                 
extern double YCONST;
static const int PROC_NUM = 10;                        // Number of Processes.Value can vary from 1 to 10 Excluding(3, 6, 7, 9).More than 10 is overkill.
static const int WIDTH = 800;                          // General window width.
static const int HEIGHT = 800;                         // General window height.
static const int ITERATIONS = 1000;                    // Number of mandelbrot iterations.
static const double HORIZONTAL = 2.00;                 // Horizontal scale.
static const double VERTICAL = 2.00;                   // Vetical scale.
static const double ZOOM = 4.00;                       // Mandelbrot starting zoom.

#endif /* GLOBAL_VARS_H */