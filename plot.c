#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xlocale.h>

int main(int argc, char *argv[]) {

    Display *displ;
    int screen;
    Window win;
    XWindowAttributes winattr;
    XEvent event;

    // Locale optimisation.
    if (setlocale(LC_ALL, "") == NULL) {
        fprintf(stderr, "setlocale(LC_ALL, "") is NULL.\n");
        exit(1);
    }
    if (!XSupportsLocale()) {
        fprintf(stderr, "Locale is not supported.Exiting.\n");
        exit(1);
    }
    if (XSetLocaleModifiers("") == NULL) {
        fprintf(stderr, "XSetLocaleModifiers is NULL.\n");
        exit(1);
    }

    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        fprintf(stderr, "Failed to open Display.\n");
        exit(1);
    }

    screen = DefaultScreen(displ);
    printf("Default screen value: %d\n", screen);

    /*  Root main Window */
    win = XCreateSimpleWindow(displ, XRootWindow(displ, screen), 0, 0, 800, 800, 0, XWhitePixel(displ, screen), XBlackPixel(displ, screen));
    XSelectInput(displ, win, ExposureMask | KeyPressMask /*| PointerMotionMask*/);
    XMapWindow(displ, win);

    /* Delete window initializer area */
    Atom wm_delete_window = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, win, &wm_delete_window, 1);

    /* Change main window Title */
    Atom new_attr = XInternAtom(displ, "WM_NAME", False);
    Atom type =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, win, new_attr, type, 8, PropModeReplace, (unsigned char*)"mandelbrot set", 14);

    /* Add grafical context to window */
    XGCValues values;
    values.line_width = 2;
    values.line_style = LineSolid;
    values.fill_rule = WindingRule;
    values.foreground = XWhitePixel(displ, screen);
    GC gc = XCreateGC(displ, win, GCForeground | GCLineWidth | GCLineStyle | GCFillRule, &values);

    while (1) {
        while (XPending(displ) > 0) {
            XNextEvent(displ, &event);
            
            if (event.type == ClientMessage) {
                if (event.xclient.data.l[0] == wm_delete_window) {
                    printf("WM_DELETE_WINDOW");
                    XFreeGC(displ, gc);
                    XCloseDisplay(displ);
                    return 0;
                }
            } else if (event.type == Expose && event.xclient.window == win) {
                /* Get window attributes */
                XGetWindowAttributes(displ, win, &winattr);
                printf("Expose Event occured.\n");
                
            } else if (event.type == KeyPress && event.xclient.window == win) {
                for (int x = 0; x <= winattr.width; x++) {
                    for (int y = 0; y <= winattr.height; y++) {

                        float a = (float)(x - (winattr.width / 2)) / (float)(winattr.width / 4);
                        float b = (float)(y - (winattr.height / 2)) / (float)(winattr.height / 4);;
                        float curr_a = a;
                        float curr_b = b;
                        float n = 0;

                        while (n < 100) {
                            float iter_a = (a * a) - (b * b);
                            float iter_b = 2 * a * b;
                            a = iter_a + curr_a;
                            b = iter_b + curr_b;
                            
                            if (abs(a + b) > 16) {
                                break;
                            }
                            n++;
                        }
                        if (n == 100) {
                            values.foreground = rand() % 255;
                            values.background = rand() % 255;
                            GC gc = XCreateGC(displ, win, GCBackground | GCForeground | GCLineWidth | GCLineStyle | GCFillRule, &values);
                            XDrawPoint(displ, win, gc, x, y);                           
                        }
                    }
                }
            } else {
                printf("Main Window Event.\n");
                printf("Event Type: %d\n", event.type);
            }
        }
    }

    return 0;
}

