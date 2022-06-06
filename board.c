// general headers
#ifndef _STDIO_H
    #include <stdio.h>
#endif
#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

// multiprocessing includes
#ifndef _UNISTD_H
    #include <unistd.h>
#endif
#ifndef _SYS_WAIT_H
    #include <sys/wait.h>
#endif

// shared memory
#ifndef _SYS_IPC_H
    #include <sys/ipc.h>
#endif
#ifndef _SYS_SHM_H
    #include <sys/shm.h>
#endif
#ifndef _SEMAPHORE_H
    #include <semaphore.h>
#endif

// signals
#ifndef _SIGNAL_H
    #include <signal.h>
#endif

// HEADERS INCLUDED FOR TESTING
#include <time.h>

// Project specific headers
#ifndef _SHMEM_H
    #include "header_files/shmem.h"
#endif
#ifndef _OBJECTS_H
    #include "header_files/objects.h"
#endif
#ifndef _GLOBAL_VARS_H
    #include "header_files/global_vars.h"
#endif
#ifndef _TRANSMITTER_H
    #include "header_files/transmitter.h"
#endif

// some usefull Macros
#ifndef EMVADON 
    #define EMVADON (obj.winattr->width * obj.winattr->height)
#endif

// initialize the knot object to be transfered because we can't transfer pointers to pointers through shared memory.
void init_knot(KNOT *knot, const Object obj);

// General initialization and event handling.
const int board(const int pids[]) {

    // shared memory
    // object to transfer between processes the integer variables for each process calculations.
    KNOT *sh_knot;
    key_t knot_key = gorckey("./keys/knot_key.txt", 9988);
    int shknot_id = crshmem(knot_key, sizeof(KNOT), SHM_RDONLY);
    if (shknot_id == -1)
        fprintf(stderr, "Warning: Board - shknot_id - crshmem()\n");

    sh_knot = attshmem(shknot_id, NULL, SHM_RND);
    if (sh_knot == NULL)
        fprintf(stderr, "Warning: Board - sh_knot - attshmem()\n");

    // The shared image data key.
    key_t image_key = gorckey("./keys/image_key.txt", 8899);
    if (image_key == -1)
        fprintf(stderr, "Warning: Board - image_key - gorckey()\n");
    // The shared image data memory id.We define it here so it is available in the event loop further down.
    int shimage_id;

    Display *displ;
    int screen;
    Window win;
    XEvent event;
    Object obj;

    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        perror("Board - XOpenDisplay()");
        return EXIT_FAILURE;;
    } else {
        obj.displ = displ;
    }

    screen = DefaultScreen(displ);

    /*  Root main Window */
    XWindowAttributes winattr;
    XSetWindowAttributes set_attr;
    win = XCreateWindow(displ, XRootWindow(displ, screen), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, 0, &set_attr);
    XSelectInput(displ, win, SubstructureRedirectMask | ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);
    XMapWindow(displ, win);
    obj.win = win;

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
        perror("Board - XOpenIM()");
        return EXIT_FAILURE;;
    }
    failed_arg = XGetIMValues(xim, XNQueryInputStyle, &styles, NULL);
    if (failed_arg != NULL) {
        perror("Board - XGetIMValues()");
        return EXIT_FAILURE;;
    }
    // for (int i = 0; i < styles->count_styles; i++) {
    //     printf("Styles supported %lu.\n", styles->supported_styles[i]);
    // }
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, win, NULL);
    if (xic == NULL) {
        perror("Board - XreateIC()");
        return EXIT_FAILURE;;
    }
    XSetICFocus(xic);

    /* Add grafical context to window */
    XGCValues values;
    values.foreground = XWhitePixel(displ, screen);
    values.background = XBlackPixel(displ, screen);
    GC gc = XCreateGC(displ, win, GCForeground | GCBackground, &values);
    obj.gc = gc;
    
    obj.max_iter = ITERATIONS;
    obj.horiz = HORIZONTAL;
    obj.vert = VERTICAL;
    obj.zoom = ZOOM;
    obj.init_x = 0;
    obj.init_y = 0;

    clock_t begin;
    clock_t end;
    double exec_time;

    int expose_counter = 0;
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
                    for (int i = 0; i < PROC_NUM; i++) {
                        kill(pids[i], SIGKILL);
                    }
                    if (dtshmem(sh_knot) == -1)
                        fprintf(stderr, "Warning: Board - ClientMessage Event - sh_knot - dtshmem()\n");
                    if (destshmem(shknot_id, IPC_RMID, 0) == -1)
                        fprintf(stderr, "Warning: Board - ClientMessage Event - shknot_id - destshmem()\n");
                    if (destshmem(shimage_id, IPC_RMID, 0) == -1)
                        fprintf(stderr, "Warning: Board - ClientMessage Event - shimage_id - destshmem()\n");
                    return EXIT_SUCCESS;
                }
            } else if (event.type == Expose && event.xclient.window == win) {
                if (event.xresizerequest.window == win && expose_counter != 0) {
                    printf("Window resized\n");
                    if (destshmem(shimage_id, IPC_RMID, 0) == -1)
                        fprintf(stderr, "Warning: Board - Expose Event - shimage_id - destshmem()\n");
                    // Here i must find a way to discard the resizing event until the last one.
                }
                /* Get window attributes */
                XGetWindowAttributes(displ, win, &winattr);
                obj.winattr = &winattr;
                // time count...
                begin = clock();
                // At expose event we create the shared image data memory.We do it here because we need to recreate it if user resizes the window.
                shimage_id = crshmem(image_key, sizeof(char) * EMVADON * 4, 0666 | IPC_CREAT);
                if (shimage_id == -1)
                    fprintf(stderr, "Warning: Board - Expose Event - crshmem()\n");
                // initialize knot object and set shared memory vaules equal to knot.
                init_knot(sh_knot, obj);
                transmitter(obj, pids);
                expose_counter += 1;
                end = clock();
                exec_time = (double)(end - begin) / CLOCKS_PER_SEC;
                printf("Iterator Execution Time : %f\n", exec_time);
            } else if (event.type == ButtonPress && event.xclient.window == win) {

                if (obj.init_x == 0.00 && obj.init_y == 0.00) {
                    obj.init_x = (((double)event.xbutton.x - (obj.winattr->width / obj.horiz)) / (obj.winattr->width / obj.zoom));
                    obj.init_y = (((double)event.xbutton.y - (obj.winattr->height / obj.vert)) / (obj.winattr->height / obj.zoom));
                } else {
                    obj.init_x = obj.init_x + (((double)event.xbutton.x - (obj.winattr->width / obj.horiz)) / (obj.winattr->width / obj.zoom));
                    obj.init_y = obj.init_y + (((double)event.xbutton.y - (obj.winattr->height / obj.vert)) / (obj.winattr->height / obj.zoom));
                }
                
                if (event.xkey.keycode == 1) {
                    obj.zoom *= 0.50;
                } else if (event.xkey.keycode == 3) {
                    obj.zoom /= 0.50;
                }
                init_knot(sh_knot, obj);
                // time count...
                begin = clock();
                transmitter(obj, pids);
                end = clock();
                exec_time = (double)(end - begin) / CLOCKS_PER_SEC;
                printf("Iterator Execution Time : %f\n", exec_time);
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
                    obj.horiz += 0.01;
                } else if (keysym == 65363) {
                    obj.horiz -= 0.01;
                } else if (keysym == 65362) {
                    obj.vert += 0.01; 
                } else if (keysym == 65364) {
                    obj.vert -= 0.01;
                } else if (keysym == 65293) {
                    obj.zoom *= 0.50;
                }
                init_knot(sh_knot, obj);
                transmitter(obj, pids);
            }
        }
    }
}

void init_knot(KNOT *knot, const Object obj) {

    knot->width = obj.winattr->width;
    knot->height = obj.winattr->height;
    knot->horiz = obj.horiz;
    knot->vert = obj.vert;
    knot->max_iter = obj.max_iter;
    knot->zoom = obj.zoom;
    knot->x = obj.x;
    knot->y = obj.y;
    knot->init_x = obj.init_x;
    knot->init_y = obj.init_y;
}

