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
    #define YPOLOIPON (obj.winattr->width * obj.winattr->height % PROC_NUM)
#endif

// initialize the knot object to be transfered because we can't transfer pointers to pointers through shared memory.
void init_knot(KNOT *knot, const Object obj);
// static Bool predicate(Display *displ, XEvent *event, XPointer arg) {
//     if (event->type == Expose)
//         return True;
//     return False;
// }
// printf("XCheckIfEvent : %s\n", XCheckIfEvent(displ, &ev, predicate, NULL) ? "True" : "False");

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

    int screen = DefaultScreen(displ);

    /*  Root main Window */
    XWindowAttributes winattr;
    XSetWindowAttributes set_attr;
    set_attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;
    set_attr.background_pixel = 0x000000;
    win = XCreateWindow(displ, XRootWindow(displ, screen), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &set_attr);
    XMapWindow(displ, win);
    obj.win = win;

    /* Delete window initializer area */
    Atom wm_delete_window = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, win, &wm_delete_window, 1);

    /* Change main window Title */
    Atom new_attr = XInternAtom(displ, "WM_NAME", False);
    Atom type =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, win, new_attr, type, 8, PropModeReplace, (unsigned char*)"Mandelbrot Set", 14);

    /* Add grafical context*/
    XGCValues values;
    values.foreground = XWhitePixel(displ, screen);
    values.background = XBlackPixel(displ, screen);
    GC gc = XCreateGC(obj.displ, obj.win, GCBackground | GCForeground, &values);
    obj.gc = gc;

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
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, win, NULL);
    if (xic == NULL) {
        perror("Board - XreateIC()");
        return EXIT_FAILURE;;
    }
    XSetICFocus(xic);
    
    obj.max_iter = ITERATIONS;
    obj.horiz = HORIZONTAL;
    obj.vert = VERTICAL;
    obj.zoom = ZOOM;
    obj.init_x = 0;
    obj.init_y = 0;

    Pixmap pixmap;

    int expose_counter = 0;
    while (1) {

        XNextEvent(displ, &event);

        if (event.type == ClientMessage) {
            if (event.xclient.data.l[0] == wm_delete_window) {
                printf("WM_DELETE_WINDOW");

                for (int i = 0; i < PROC_NUM; i++) {
                    kill(pids[i], SIGKILL);
                }
                if (dtshmem(sh_knot))
                    fprintf(stderr, "Warning: Board - ClientMessage Event - sh_knot - dtshmem()\n");
                if (destshmem(shknot_id, IPC_RMID, 0))
                    fprintf(stderr, "Warning: Board - ClientMessage Event - shknot_id - destshmem()\n");
                if (destshmem(shimage_id, IPC_RMID, 0))
                    fprintf(stderr, "Warning: Board - ClientMessage Event - shimage_id - destshmem()\n");

                if (gc != NULL) {
                    XFreeGC(displ, gc);
                }
                XFreePixmap(displ, pixmap);
                XDestroyWindow(displ, win);
                XCloseDisplay(displ);
                return EXIT_SUCCESS;
            }

        } else if (event.type == Expose) {

            printf("Expose events count: %d\n", event.xexpose.count);
            printf("Expose width: %d\n", event.xexpose.width);
            printf("Expose height: %d\n", event.xexpose.height);

            if (expose_counter) {
                if (destshmem(shimage_id, IPC_RMID, 0))
                    fprintf(stderr, "Warning: Board - Expose Event - shimage_id - destshmem()\n");
                XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
                // Here i must find a way to discard the resizing event until the last one.
            }

            /* Get window attributes */
            XGetWindowAttributes(displ, win, &winattr);
            obj.winattr = &winattr;


            if (YPOLOIPON && obj.winattr->height != 883) {
                if (PROC_NUM == 7)
                    obj.winattr->height += YPOLOIPON + 1;
                obj.winattr->height += YPOLOIPON;
            } else if (obj.winattr->height == 883 && (obj.winattr->height % PROC_NUM != 0)) {
                    obj.winattr->height -= obj.winattr->height % PROC_NUM;
            }

            // At expose event we create the shared image data memory.We do it here because we need to recreate it if user resizes the window.
            shimage_id = crshmem(image_key, sizeof(char) * EMVADON * 4, 0666 | IPC_CREAT);
            if (shimage_id == -1)
                fprintf(stderr, "Warning: Board - Expose Event - crshmem()\n");

            // initialize knot object and set shared memory vaules equal to knot.
            init_knot(sh_knot, obj);
            transmitter(obj, pids);
            pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);
            XCopyArea(obj.displ, obj.win, pixmap, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);

            expose_counter += 1;

        } else if (event.type == ButtonPress) {
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
            transmitter(obj, pids);

        } else if (event.type == KeyPress) {
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

