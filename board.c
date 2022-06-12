// general headers
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// multiprocessing includes
#include <unistd.h>
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

#define EXIT_FAILURE_NEGATIVE -1
#define POINTERMASKS ( ButtonPressMask )
#define KEYBOARDMASKS ( KeyPressMask )
#define EXPOSEMASKS ( StructureNotifyMask | SubstructureNotifyMask | VisibilityChangeMask | ExposureMask )

enum { App_Close, App_Name, App_Iconify, Atom_Type, Atom_Last};

typedef struct {
    
    unsigned int width;
    unsigned int height;
    int iterations;
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

} Mandelbrot;

static void mandelbrot_init(Mandelbrot *md);
void painter(const Mandelbrot md, char *image_data);
void receiver(Mandelbrot md);
void *oscillator(void *args);
const int transmitter(void);
const void clientmessage(XEvent *event);
void visibilitychange(XEvent *event);
const void mapnotify(XEvent *event);
void unmapnotify(XEvent *event);
void expose(XEvent *event);
void buttonpress(XEvent *event);
int board();

// Global Variables
Display *displ;
XWindowAttributes stat_root, stat_app;
XSetWindowAttributes set_app;
Window app;
static int screen;
static int RUNNING = 1; 
static const int THREADS_NUM = 7;                      // The number of Threads.
static const int WIDTH = 800;                          // App starting window width.
static const int HEIGHT = 800;                         // App starting window height.
static const int ITERATIONS = 1000;                    // Number of mandelbrot iterations.
static const double HORIZONTAL = 2.00;                 // Horizontal scale.
static const double VERTICAL = 2.00;                   // Vetical scale.
static const double ZOOM = 4.00;                       // Mandelbrot starting zoom.
static void (*handler[LASTEvent]) (XEvent *event) = {
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
static void mandelbrot_init(Mandelbrot *md) {
    md->width = stat_app.width;
    md->height = stat_app.height;
    md->iterations = ITERATIONS;
    md->horiz = HORIZONTAL; 
    md->vert = VERTICAL;
    md->zoom = ZOOM;
    md->init_x = 0.00;
    md->init_y = 0.00;
}
/* ##################################################################################################################### */
void painter(const Mandelbrot md, char *image_data) {

    double a = (md.x - (md.width / md.horiz)) / (md.width / md.zoom) + md.init_x;
    double b = (md.y - (md.height / md.vert)) / (md.height / md.zoom) + md.init_y;
    double curr_a = a;
    double curr_b = b;

    int n = 0;
    while (n < md.iterations) {
        double iter_a = (a * a) - (b * b);
        double iter_b = 2 * a * b;
        a = iter_a + curr_a;
        b = iter_b + curr_b;

        if (abs(a + b) > 4) {
            break;
        }
        n++;
    }
    if (n < md.iterations && n >= 255)
        n = n / 100;
    if (n < md.iterations && n < 255) {
        image_data[md.counter] =  n + 10;
        image_data[md.counter + 1] = n + n;
        image_data[md.counter + 2] =  n * 20;
    } else {
        if (image_data[md.counter] != 0x00 || image_data[md.counter + 1] != 0x00 || image_data[md.counter + 2] != 0x00) {
            image_data[md.counter] =  0x00;
            image_data[md.counter + 1] = 0x00;
            image_data[md.counter + 2] = 0x00;
        }
    }
}
/* ##################################################################################################################### */
void receiver(Mandelbrot md) {

    XImage *image;
    char image_data[((md.width * md.height) / THREADS_NUM) * 4];

    /* Add grafical context */
    XGCValues gc_values;
    gc_values.foreground = 0xffffff;
    GC graphics = XCreateGC(displ, app, GCForeground, &gc_values);

    int counter = md.step_counter;
    int x = md.step_x;
    int y = md.step_y;

    for (int i = 0; i < (md.width * md.height) / THREADS_NUM; i++) {

        if (x == md.width) {
            y += 1;
            x = 0;
        }

        md.x = x;
        md.y = y;
        md.counter = counter;
        painter(md, image_data);
        counter += 4;
        x++;
    }

    image = XCreateImage(displ, stat_app.visual, stat_app.depth, ZPixmap, 0, image_data, md.width, md.height / THREADS_NUM, 32, 0);
    XPutImage(displ, app, graphics, image, 0, 0, 0, md.step_y, md.width, md.height / THREADS_NUM);
    XFreeGC(displ, graphics);
    XFree(image);
}
/* ##################################################################################################################### */
void *oscillator(void *args) {

    Mandelbrot md;
    mandelbrot_init(&md);

    int thread_id = *(int*)args;
    
    md.step_counter = 0;
    md.step_y = (md.height / THREADS_NUM) * thread_id;

    receiver(md);

    return (void*) args;
}
/* ##################################################################################################################### */
const int transmitter(void) {

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
const void clientmessage(XEvent *event) {
    
    if (event->xclient.data.l[0] == wmatom[App_Iconify]) {
        printf("Iconify message received!\n");
    }
    // printf("client message received   ******\n");
    if (event->xclient.data.l[0] == wmatom[App_Close]) {
        printf("WM_DELETE_WINDOW\n");
        XDestroyWindow(displ, app);
        XCloseDisplay(displ);
        RUNNING = 0;
    }
}
/* ##################################################################################################################### */
void visibilitychange(XEvent *event) {

    printf("visibilitychange event received   ******\n");
    // transmitter();

}
/* ##################################################################################################################### */
const void mapnotify(XEvent *event) {

    printf("mapnotify event received   ******\n");
    XGetWindowAttributes(displ, app, &stat_app);

    if (transmitter())
        perror("mapnotify() --- transmitter()");

    // XSync(displ, True);
}
/* ##################################################################################################################### */
void unmapnotify(XEvent *event) {

    printf("unmapnotify event received   ******\n");
    // XSync(displ, True);

}
/* ##################################################################################################################### */
void expose(XEvent *event) {

    printf("expose event received   ******\n");
    // transmitter();

}
void buttonpress(XEvent *event) {

    printf("buttonpress event received   ******\n");
    // transmitter();

}
/* ##################################################################################################################### */
// General initialization and event handling.
const int board() {

    // printf("board starting initiallization   ******\n");
    // Add some randomness
    // srand(time(NULL));
    XInitThreads();
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
    
    if (board())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
/* ##################################################################################################################### */

