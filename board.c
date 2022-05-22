// general headers
#include <stdio.h>
#include <stdlib.h>

// multiprocessing includes
#include <unistd.h>
#include <sys/wait.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

// signal
#include <signal.h>

// Time included for testing execution time
#include <time.h>

// object specific headers
#include "header_files/locale.h"
#include "header_files/objects.h"
#include "header_files/transmitter.h"

// initialize the knot object to be transfered because we can't transfer pointers to pointers through shared memory.
KNOT init_knot(KNOT *knot, const Object obj) {

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

    return *knot;
}

// General initialization and event handling.
int board(int pids[]) {

    // object to transfer between processes the integer variables for each process calculations.
    KNOT knot, *shknot;
    key_t key = ftok("./knot_key", 9988);
    int shknotid = shmget(key, sizeof(KNOT), 0666);
    shknot = shmat(shknotid, NULL, 0);

    if (shknot == NULL) {
        perror("shmat()");
        return 1;
    }

    char *shmem_2;
    key_t key_2 = 9998;
    int shmid_2 = shmget(key_2, sizeof(char) * 800 * 800 * 4, 0666);
    shmem_2 = shmat(shmid_2, NULL, 0);

    if (shmem_2 == NULL) {
        perror("shmat()");
        return 1;
    }

    Display *displ;
    int screen;
    Window win;
    XWindowAttributes winattr;
    XEvent event;
    Object obj;

    // Global window constants.
    obj.winattr = &winattr;
    obj.winattr->width = 800;
    obj.winattr->height = 800;


    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        fprintf(stderr, "Failed to open Display.\n");
        exit(1);
    } else {
        obj.displ = displ;
    }

    screen = DefaultScreen(displ);

    /*  Root main Window */
    win = XCreateSimpleWindow(displ, XRootWindow(displ, screen), 0, 0, obj.winattr->width, obj.winattr->height, 0, XWhitePixel(displ, screen), XBlackPixel(displ, screen));
    XSelectInput(displ, win, ExposureMask | KeyPressMask | ButtonPressMask /*| PointerMotionMask*/);
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
        fprintf(stderr, "Failed to open Input Method.\n");
        exit(2);
    }
    failed_arg = XGetIMValues(xim, XNQueryInputStyle, &styles, NULL);
    if (failed_arg != NULL) {
        fprintf(stderr, "Failed to obtain input method's styles.\n");
        exit(3);
    }
    // for (int i = 0; i < styles->count_styles; i++) {
    //     printf("Styles supported %lu.\n", styles->supported_styles[i]);
    // }
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, win, NULL);
    if (xic == NULL) {
        fprintf(stderr, "Could not open xic.\n");
        exit(4);
    }
    XSetICFocus(xic);

    /* Add grafical context to window */
    XGCValues values;
    values.foreground = XWhitePixel(displ, screen);
    values.background = XBlackPixel(displ, screen);
    GC gc = XCreateGC(displ, win, GCForeground | GCBackground, &values);

    obj.gc = gc;
    obj.values = values;
    obj.max_iter = 1000;
    obj.horiz = 2.00;
    obj.vert = 2.00;
    obj.zoom = 4.00;
    obj.init_x = 0;
    obj.init_y = 0;

    clock_t begin;
    clock_t end;
    double exec_time;

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
                    for (int i = 0; i < 10; i++) {
                        kill(pids[i], SIGKILL); ////////////////////////////////////////////////////   
                    }
                    shmdt(&shknotid);
                    shmdt(&shmid_2);
                    shmctl(shknotid, IPC_RMID, 0);
                    shmctl(shmid_2, IPC_RMID, 0);
                    return 0;
                }
            } else if (event.type == Expose && event.xclient.window == win) {
                /* Get window attributes */
                XGetWindowAttributes(displ, win, &winattr);
                obj.winattr = &winattr;
                // initialize knot object and set shared memory vaues equal to knot.
                *shknot = init_knot(&knot, obj);
                // time count...
                begin = clock();
                transmitter(obj, pids);
                end = clock();
                exec_time = (double)(end - begin) / CLOCKS_PER_SEC;
                printf("Iterator Execution Time : %f\n", exec_time);
                // shmdt(&shknotid);
                // shmctl(shknotid, IPC_RMID, 0);
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
                *shknot = init_knot(&knot, obj);
                // time count...
                begin = clock();
                transmitter(obj, pids);
                end = clock();
                exec_time = (double)(end - begin) / CLOCKS_PER_SEC;
                printf("Iterator Execution Time : %f\n", exec_time);
                // shmdt(&shknotid);
                // shmctl(shknotid, IPC_RMID, 0);
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
                transmitter(obj, pids);
            } else {
                //printf("Main Window Event.\n");
                //printf("Event Type: %d\n", event.type);
            }
        }
    }

    return 0;
}

