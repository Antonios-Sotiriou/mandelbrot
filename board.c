// general headers
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// multiprocessing includes
#include <unistd.h>
#include <sys/wait.h>

// semaphores and synchronization
#include <semaphore.h>

// signals
#include <signal.h>

// Usefull headers for testing
#include <time.h>

// Project specific headers
#include "header_files/locale.h"
#include "header_files/objects.h"

int painter();
int receiver();
int oscillator();
int transmitter();
int clientmessage(XEvent *event);
int expose(XEvent *event);
int buttonpress(XEvent *event);
int board();

#define EXIT_FAILURE_NEGATIVE -1

enum { App_Close, App_Name, Atom_Type, Atom_Last};

// Global Variables
Display *displ;
XWindowAttributes stat_root, stat_app;
XSetWindowAttributes set_app;
Window app;
static int screen;
static int RUNNING = 1; 
static const int PROC_NUM = 7;                         // Number of Processes.Value can vary from 1 to 10 Excluding(3, 6, 7, 9).More than 10 is overkill.
static const int WIDTH = 800;                          // General window width.
static const int HEIGHT = 800;                         // General window height.
// static const int ITERATIONS = 1000;                    // Number of mandelbrot iterations.
// static const double HORIZONTAL = 2.00;                 // Horizontal scale.
// static const double VERTICAL = 2.00;                   // Vetical scale.
// static const double ZOOM = 4.00;                       // Mandelbrot starting zoom.
static int (*handler[LASTEvent]) (XEvent *event) = {
	[ButtonPress] = buttonpress,
	[ClientMessage] = clientmessage,
	// [ConfigureRequest] = configurerequest,
	// [ConfigureNotify] = configurenotify,
	// [DestroyNotify] = destroynotify,
	// [EnterNotify] = enternotify,
	[Expose] = expose,
	// [FocusIn] = focusin,
	// [KeyPress] = keypress,
	// [MappingNotify] = mappingnotify,
	// [MapRequest] = maprequest,
	// [MotionNotify] = motionnotify,
	// [PropertyNotify] = propertynotify,
	// [UnmapNotify] = unmapnotify
};
static Atom wmatom[Atom_Last];

/* ##################################################################################################################### */
int painter() {

    // Add some randomness
    srand(time(NULL));
    
    /* Add grafical context */
    XGCValues gc_values;
    gc_values.foreground = 0xffffff;
    GC rectangle = XCreateGC(displ, app, GCForeground, &gc_values);

    printf("Execution reached painter ##########################\n");
    XDrawRectangle(displ, app, rectangle, rand() % 400, rand() % 400, rand() % 200, rand() % 200);
    XFreeGC(displ, rectangle);

    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
int receiver() {

    printf("Execution reached receiver ##########################\n");
    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
int oscillator() {

    int WAITING = 1;

    printf("Execution reached oscillator ##########################\n");

    if (WAITING) {
        printf("LOOPING\n");
        painter();
        WAITING = 0;
    }

    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
const int transmitter() {

    printf("Execution reached transmitter ##########################\n");

    int pids[PROC_NUM];
    for (int i = 0; i < PROC_NUM; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("Main - fork()");
            return EXIT_FAILURE;
        } else if (pids[i] == 0) {
            // child process
            oscillator();
            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
int clientmessage(XEvent *event) {

    printf("client message received   ******\n");
    if (event->xclient.data.l[0] == wmatom[App_Close]) {
        printf("WM_DELETE_WINDOW\n");
        XDestroyWindow(displ, app);
        XCloseDisplay(displ);
        RUNNING = 0;
        printf("client message processed  ******\n");
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
/* ##################################################################################################################### */
int expose(XEvent *event) {

    printf("expose event received   ******\n");
    transmitter();
    return EXIT_SUCCESS;
}
int buttonpress(XEvent *event) {

    printf("buttonpress event received   ******\n");
    transmitter();
    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
// General initialization and event handling.
const int board() {

    printf("board starting initiallization   ******\n");
    XEvent event;
    
    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        perror("Board - XOpenDisplay()");
        return EXIT_FAILURE;;
    }
    screen = XDefaultScreen(displ);
    XGetWindowAttributes(displ, XDefaultRootWindow(displ), &stat_root);
    
    /*  Root main Window */
    set_app.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;
    set_app.background_pixel = 0x000000;
    app = XCreateWindow(displ, XDefaultRootWindow(displ), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &set_app);
    XMapWindow(displ, app);

    /* Delete window initializer area */
    wmatom[App_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, app, &wmatom[App_Close], 1);

    /* Change main window Title */
    wmatom[App_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[Atom_Type] =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, app, wmatom[App_Name], wmatom[Atom_Type], 8, PropModeReplace, (unsigned char*)"Mandelbrot Set", 14);

    /* Get user text input */
    XIM xim;
    XIC xic;
    char *failed_arg;
    XIMStyles *styles;
    //XIMStyle xim_requested_style;
    xim = XOpenIM(displ, NULL, NULL, NULL);
    if (xim == NULL) {
        perror("Board - XOpenIM()");
        return EXIT_FAILURE;
    }
    failed_arg = XGetIMValues(xim, XNQueryInputStyle, &styles, NULL);
    if (failed_arg != NULL) {
        perror("Board - XGetIMValues()");
        return EXIT_FAILURE;
    }
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, app, NULL);
    if (xic == NULL) {
        perror("Board - XreateIC()");
        return EXIT_FAILURE;
    }
    XSetICFocus(xic);

    while (RUNNING) {

        printf("board waiting for events         ******\n");
        XNextEvent(displ, &event);

		if (handler[event.type])
			handler[event.type](&event);
    }
    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
int main(int argc, char *argv[]) {

    if (locale_init())
        fprintf(stderr, "Warning: Main -locale()\n");

    board();
    
    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */

