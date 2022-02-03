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
    XSelectInput(displ, win, ExposureMask | KeyPressMask | ButtonPressMask /*| PointerMotionMask*/);
    XMapWindow(displ, win);

    /* Delete window initializer area */
    Atom wm_delete_window = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, win, &wm_delete_window, 1);

    /* Change main window Title */
    Atom new_attr = XInternAtom(displ, "WM_NAME", False);
    Atom type =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, win, new_attr, type, 8, PropModeReplace, (unsigned char*)"Mandelbrot Set", 14);

    /* Get user text input *******************************************************************/
    XIM xim;
    XIC xic;
    char *failed_arg;
    XIMStyles *styles;
    //XIMStyle xim_requested_style;
    xim = XOpenIM(displ, NULL, NULL, NULL);
    if (xim == NULL) {
        fprintf(stderr, "Failed to open Input Method.\n");
        exit(2);
    }
    failed_arg = XGetIMValues(xim, XNQueryInputStyle, &styles, NULL);
    if (failed_arg != NULL) {
        fprintf(stderr, "Failed to obtain input method's styles.\n");
        exit(3);
    }
    for (int i = 0; i < styles->count_styles; i++) {
        printf("Styles supported %lu.\n", styles->supported_styles[i]);
    }
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, win, NULL);
    if (xic == NULL) {
        fprintf(stderr, "Could not open xic.\n");
        exit(4);
    }
    XSetICFocus(xic);

    /* Add grafical context to window */
    XGCValues values;
    values.line_width = 0;
    values.line_style = LineSolid;
    values.fill_style = FillSolid;
    values.fill_rule =  WindingRule;
    GC gc = XCreateGC(displ, win, GCLineWidth | GCLineStyle | GCFillStyle | GCFillRule, &values);

    float horiz = 1.30;
    float vert = 2.00;
    float zoom = 2.5;
    while (1) {
        while (XPending(displ) > 0) {
            XNextEvent(displ, &event);
            
            if (event.type == ClientMessage) {
                if (event.xclient.data.l[0] == wm_delete_window) {
                    printf("WM_DELETE_WINDOW");
                    if (gc != NULL) {
                        XFreeGC(displ, gc);
                    }
                    XCloseDisplay(displ);
                    return 0;
                }
            } else if (event.type == Expose && event.xclient.window == win) {
                /* Get window attributes */
                XGetWindowAttributes(displ, win, &winattr);
                printf("Expose Event occured.\n");
            } else if (event.type == ButtonPress && event.xclient.window == win) {
                printf("Mouse button clicked: %d\n", event.xbutton.button);
                printf("Mouse button x position: %d\n", event.xbutton.x);
                printf("Mouse button y position: %d\n", event.xbutton.y);
                for (int x = 0; x <= winattr.width; x++) {
                    for (int y = 0; y <= winattr.height; y++) {

                        float a = (float)(x - (winattr.width / horiz)) / (float)(winattr.width / zoom);
                        float b = (float)(y - (winattr.height / vert)) / (float)(winattr.height / zoom);;
                        float curr_a = a;
                        float curr_b = b;
                        int n = 0;

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
                            values.foreground = 3517575;
                            XChangeGC(displ, gc, GCForeground, &values);
                            XDrawPoint(displ, win, gc, x, y);                                   
                        } else if (n < 100 && n >= 10) {
                            values.foreground = n * n;
                            XChangeGC(displ, gc, GCForeground, &values);
                            XDrawPoint(displ, win, gc, x, y);                          
                        } else if (n < 100 && n < 10) {
                            values.foreground = n * n;
                            XChangeGC(displ, gc, GCForeground, &values);
                            XDrawPoint(displ, win, gc, x, y);                              
                        } else {
                            values.foreground = 0;
                            XChangeGC(displ, gc, GCForeground, &values);
                            XDrawPoint(displ, win, gc, x, y);  
                        }
                    }
                }
            } else if (event.type == KeyPress && event.xclient.window == win) {
                int count = 0;  
                KeySym keysym = 0;
                char buffer[32];
                Status status = 0;   
                count = Xutf8LookupString(xic, &event.xkey, buffer, 32, &keysym, &status);
                printf("Button pressed.\n");
                printf("Count %d.\n", count);
                if (status == XBufferOverflow) {
                    printf("Buffer Overflow...\n");
                }
                if (count) {
                    printf("The Button that was pressed is %s.\n", buffer);
                }
                if (status == XLookupKeySym || status == XLookupBoth) {
                    printf("Status: %d\n", status);
                }
                printf("Pressed key: %lu.\n", keysym);
                if (keysym == 65361) {
                    horiz += 0.50;
                } else if (keysym == 65363) {
                    horiz -= 0.50;
                } else if (keysym == 65362) {
                    vert += 0.50; 
                } else if (keysym == 65364) {
                    vert -= 0.50;
                } else if (keysym == 65293) {
                    zoom -= 0.50;
                }
                int testing_1 = 0;
                int testing_2 = 0;
                int testing_3 = 0;
                int testing_4 = 0;

                for (int x = 0; x <= winattr.width; x++) {
                    for (int y = 0; y <= winattr.height; y++) {

                        float a = (float)(x - (winattr.width / horiz)) / (float)(winattr.width / zoom);
                        float b = (float)(y - (winattr.height / vert)) / (float)(winattr.height / zoom);;
                        float curr_a = a;
                        float curr_b = b;
                        int n = 0;

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
                            values.foreground = 0;
                            XChangeGC(displ, gc, GCForeground, &values);
                            XDrawPoint(displ, win, gc, x, y);
                            testing_1++;                                     
                        } else if (n < 100 && n >= 10) {
                            values.foreground = n * n;
                            XChangeGC(displ, gc, GCForeground, &values);
                            XDrawPoint(displ, win, gc, x, y);
                            testing_2++;                          
                        } else if (n < 100 && n < 10) {
                            values.foreground = n * n;
                            XChangeGC(displ, gc, GCForeground, &values);
                            XDrawPoint(displ, win, gc, x, y);
                            testing_3++;                                
                        } else {
                            values.foreground = 0;
                            XChangeGC(displ, gc, GCForeground, &values);
                            XDrawPoint(displ, win, gc, x, y);  
                            testing_4++;
                        }
                    }
                }
                printf("Testing 1: %d\nTesting 2: %d\nTesting 3: %d\nTesting 4: %d\n", testing_1, testing_2, testing_3, testing_4);
            } else {
                printf("Main Window Event.\n");
                printf("Event Type: %d\n", event.type);
            }
        }
    }

    return 0;
}

