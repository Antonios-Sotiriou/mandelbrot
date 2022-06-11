// general headers
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// multiprocessing includes
// #include <unistd.h>
// #include <sys/wait.h>
#include <pthread.h>

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
void *oscillator();
int transmitter();
int clientmessage(XEvent *event);
int visibilitychange(XEvent *event);
int mapnotify(XEvent *event);
int unmapnotify(XEvent *event);
int expose(XEvent *event);
int buttonpress(XEvent *event);
int board();

#define EXIT_FAILURE_NEGATIVE -1
#define POINTERMASKS ( ButtonPressMask )
#define KEYBOARDMASKS ( KeyPressMask )
#define EXPOSEMASKS ( StructureNotifyMask | SubstructureNotifyMask | VisibilityChangeMask | ExposureMask )

enum { App_Close, App_Name, App_Iconify, Atom_Type, Atom_Last};

// Global Variables
Display *displ;
XWindowAttributes stat_root, stat_app;
XSetWindowAttributes set_app;
Window app;
static int screen;
static int RUNNING = 1; 
static const int THREADS_NUM = 7;                      // Number of Processes.Value can vary from 1 to 10 Excluding(3, 6, 7, 9).More than 10 is overkill.
static const int WIDTH = 800;                          // App starting window width.
static const int HEIGHT = 800;                         // App starting window height.
// static const int ITERATIONS = 1000;                    // Number of mandelbrot iterations.
// static const double HORIZONTAL = 2.00;                 // Horizontal scale.
// static const double VERTICAL = 2.00;                   // Vetical scale.
// static const double ZOOM = 4.00;                       // Mandelbrot starting zoom.
static int (*handler[LASTEvent]) (XEvent *event) = {
	[ClientMessage] = clientmessage,
	// [ConfigureRequest] = configurerequest,
	// [ConfigureNotify] = configurenotify,
	// [DestroyNotify] = destroynotify,
	// [EnterNotify] = enternotify,
    [VisibilityNotify] = visibilitychange,
	[MapNotify] = mapnotify,
	[UnmapNotify] = unmapnotify,
	[Expose] = expose,
	// [FocusIn] = focusin,
	// [KeyPress] = keypress,
	// [MappingNotify] = mappingnotify,
	[ButtonPress] = buttonpress,
	// [MotionNotify] = motionnotify,
	// [PropertyNotify] = propertynotify
};
static Atom wmatom[Atom_Last];

/* ##################################################################################################################### */
int painter() {
    
    /* Add grafical context */
    XGCValues gc_values;
    gc_values.foreground = 0xffffff;
    GC rectangle = XCreateGC(displ, app, GCForeground, &gc_values);

    // printf("Execution reached painter ##########################\n");
    XDrawRectangle(displ, app, rectangle, rand() % 400, rand() % 400, rand() % 200, rand() % 200);
    XFreeGC(displ, rectangle);

    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
int receiver() {

    // printf("Execution reached receiver ##########################\n");
    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
void *oscillator(void *args) {

    int WAITING = 1;
    int id = *(int*)args;

    printf("Execution reached oscillator ########################## args: %d\n", id);

    if (WAITING) {
        printf("LOOPING\n");
        painter();
        WAITING = 0;
    }

    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
const int transmitter() {

    // printf("Execution reached transmitter ##########################\n");
    pthread_t threads[THREADS_NUM];
    
    // Dynamically initiallize the thread_ids array;
    int thread_ids[THREADS_NUM];
    for (int i = 0; i < THREADS_NUM; i++) {
        thread_ids[i] = i;
    }
    for (int i = 0; i < THREADS_NUM; i++) {
        if (pthread_create(&threads[i], NULL, &oscillator, &thread_ids[i]))
            return EXIT_FAILURE;
    }
    for (int i = 0; i < THREADS_NUM; i++) {
        if (pthread_join(threads[i], NULL))
            return EXIT_FAILURE;
    }    

    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
int clientmessage(XEvent *event) {
    
    if (event->xclient.data.l[0] == wmatom[App_Iconify]) {
        printf("Iconify message received!\n");
    }
    // printf("client message received   ******\n");
    if (event->xclient.data.l[0] == wmatom[App_Close]) {
        printf("WM_DELETE_WINDOW\n");
        XDestroyWindow(displ, app);
        XCloseDisplay(displ);
        RUNNING = 0;
        // printf("client message processed  ******\n");
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
/* ##################################################################################################################### */
int visibilitychange(XEvent *event) {

    printf("visibilitychange event received   ******\n");
    // transmitter();
    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
int mapnotify(XEvent *event) {

    printf("mapnotify event received   ******\n");
    // XSync(displ, True);
    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */
int unmapnotify(XEvent *event) {

    printf("unmapnotify event received   ******\n");
    // XSync(displ, True);
    return EXIT_SUCCESS;
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

    // printf("board starting initiallization   ******\n");
    // Add some randomness
    srand(time(NULL));
    XEvent event;
    
    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        perror("Board - XOpenDisplay()");
        return EXIT_FAILURE;;
    }
    screen = XDefaultScreen(displ);
    XGetWindowAttributes(displ, XDefaultRootWindow(displ), &stat_root);
    
    /*  Root main Window */
    set_app.event_mask = EXPOSEMASKS | KEYBOARDMASKS | POINTERMASKS;
    set_app.background_pixel = 0x000000;
    app = XCreateWindow(displ, XDefaultRootWindow(displ), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &set_app);
    XMapWindow(displ, app);

    /* Delete window initializer area */
    wmatom[App_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, app, &wmatom[App_Close], 1);

    /* Change main window Title */
    wmatom[App_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[App_Iconify] = XInternAtom(displ, "WM_STATE", False);
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

