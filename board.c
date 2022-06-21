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
#include <X11/Xlib.h>
#include "header_files/locale.h"

#define EXIT_FAILURE_NEGATIVE     -1                                              // Some functions need to return a negative value;
#define THREADS_NUM               7                                               // The number of Threads.
#define WIDTH                     800                                             // App starting window width.
#define HEIGHT                    800                                             // App starting window height.
#define ITERATIONS                1000                                            // Number of mandelbrot iterations.
#define HORIZONTAL                2.00                                            // Horizontal scale.
#define VERTICAL                  2.00                                            // Vetical scale.
#define ZOOM                      4.00                                            // Mandelbrot starting zoom.
#define POINTERMASKS              ( ButtonPressMask )
#define KEYBOARDMASKS             ( KeyPressMask )
#define EXPOSEMASKS               ( StructureNotifyMask )

enum { App_Close, App_Name, Atom_Type, Atom_Last};

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

Bool predicate(Display *displ, XEvent *event, XPointer args);
static void mandelbrot_init(void);
static void painter(const Mandelbrot md, char *image_data);
void receiver(Mandelbrot md);
void *oscillator(void *args);
const int transmitter(void);
const void clientmessage(XEvent *event);
void reparentnotify(XEvent *event);
const void mapnotify(XEvent *event);
void resizerequest(XEvent *event);
void configurenotify(XEvent *event);
void buttonpress(XEvent *event);
void keypress(XEvent *event);
const void pixmapupdate(void);
const void pixmapdisplay(void);
const void atomsinit(void);
const int board();

// Global Variables
Display *displ;
XWindowAttributes stat_root, stat_app;
XSetWindowAttributes set_app;
Window app;
Pixmap pixmap;
static int screen;
static int MAPCOUNT = 0;
static int FULLSCREEN = 0;
static int OLDWIDTH = WIDTH;
static int OLDHEIGHT = HEIGHT;
static int RUNNING = 1; 
static void (*handler[LASTEvent]) (XEvent *event) = {
    [ClientMessage] = clientmessage,
    [ReparentNotify] = reparentnotify,
    [MapNotify] = mapnotify,
    [ResizeRequest] = resizerequest,
    [ConfigureNotify] = configurenotify,
    [ButtonPress] = buttonpress,
    [KeyPress] = keypress,
};
static Atom wmatom[Atom_Last];
Mandelbrot md_init;

/* ##################################################################################################################### */
Bool predicate(Display *displ, XEvent *event, XPointer args) {

    if (event->type == ConfigureNotify)
        return True;
    return False;
}
/* ##################################################################################################################### */
static void mandelbrot_init(void) {
    md_init.width = stat_app.width;
    md_init.height = stat_app.height;
    md_init.iterations = ITERATIONS;
    md_init.counter = 0;
    md_init.x = 0;
    md_init.y = 0;
    md_init.horiz = HORIZONTAL;
    md_init.vert = VERTICAL;
    md_init.zoom = ZOOM;
    md_init.init_x = 0.00;
    md_init.init_y = 0.00;
    md_init.step_counter = 0;
    md_init.step_x = 0;
    md_init.step_y = 0;
}
/* ##################################################################################################################### */
static void painter(const Mandelbrot md, char *image_data) {

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
    XGCValues gc_vals;
    gc_vals.graphics_exposures = False;
    GC gc = XCreateGC(displ, app, GCGraphicsExposures, &gc_vals);

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
    XPutImage(displ, app, gc, image, 0, 0, 0, md.step_y, md.width, md.height / THREADS_NUM);
    XFreeGC(displ, gc);
    XFree(image);
}
/* ##################################################################################################################### */
void *oscillator(void *args) {

    Mandelbrot md = md_init;

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

    printf("clientmessage event received\n");
    
    if (event->xclient.data.l[0] == wmatom[App_Close]) {
        printf("WM_DELETE_WINDOW\n");
        XFreePixmap(displ, pixmap);
        XDestroyWindow(displ, app);
        XCloseDisplay(displ);

        RUNNING = 0;
    }
}
/* ##################################################################################################################### */
void reparentnotify(XEvent *event) {

    printf("reparentnotify event received\n");
    if (event->xreparent.parent != app) {

        XGetWindowAttributes(displ, app, &stat_app);
        mandelbrot_init();

        if (transmitter())
            fprintf(stderr, "mapnotify() - transmitter()");
    }
}
/* ##################################################################################################################### */
const void mapnotify(XEvent *event) {

    printf("mapnotify event received\n");

    if (MAPCOUNT) {
        pixmapdisplay();
    } else {
        MAPCOUNT = 1;
    }
}
/* ##################################################################################################################### */
void resizerequest(XEvent *event) {

    if (FULLSCREEN) {
        md_init.width = stat_app.width = event->xconfigure.width;
        md_init.height = stat_app.height = event->xconfigure.height;
    } else {
        md_init.width = stat_app.width = OLDWIDTH;
        md_init.height = stat_app.height = OLDHEIGHT;
    }

    if (!XPending(displ)) {
        if (transmitter())
            fprintf(stderr, "resizerequest() - transmitter()");

        pixmapupdate();
    } else {
        return;
    }
}
/* ##################################################################################################################### */
void configurenotify(XEvent *event) {

    if ((event->xconfigure.width == stat_root.width && event->xconfigure.height == (stat_root.height - (event->xconfigure.y * 2)))) {
        FULLSCREEN = 1;
    } else if ((event->xconfigure.width == OLDWIDTH && event->xconfigure.height == OLDHEIGHT) && FULLSCREEN) {
        FULLSCREEN = 0;
    } else {
        OLDWIDTH = event->xconfigure.width;
        OLDHEIGHT = event->xconfigure.height;
    }
    event->type = ResizeRequest;
    XSendEvent(displ, app, False, StructureNotifyMask, event);
}
/* ##################################################################################################################### */
void buttonpress(XEvent *event) {

    printf("buttonpress event received\n");
    if (md_init.init_x == 0.00 && md_init.init_y == 0.00) {
        md_init.init_x = (((double)event->xbutton.x - (md_init.width / md_init.horiz)) / (md_init.width / md_init.zoom));
        md_init.init_y = (((double)event->xbutton.y - (md_init.height / md_init.vert)) / (md_init.height / md_init.zoom));
    } else {
        md_init.init_x = md_init.init_x + (((double)event->xbutton.x - (md_init.width / md_init.horiz)) / (md_init.width / md_init.zoom));
        md_init.init_y = md_init.init_y + (((double)event->xbutton.y - (md_init.height / md_init.vert)) / (md_init.height / md_init.zoom));
    }

    if (event->xkey.keycode == 1) {
        md_init.zoom *= 0.50;
    } else if (event->xkey.keycode == 3) {
        md_init.zoom /= 0.50;
    }

    if (transmitter())
        fprintf(stderr, "buttonpress() - transmitter()");

    pixmapupdate();
}
/* ##################################################################################################################### */
void keypress(XEvent *event) {

    /* Get user text input */
    XIM xim;
    XIC xic;
    char *failed_arg;
    XIMStyles *styles;
    //XIMStyle xim_requested_style;
    xim = XOpenIM(displ, NULL, NULL, NULL);
    if (xim == NULL) {
        fprintf(stderr, "keypress() - XOpenIM()");
    }
    failed_arg = XGetIMValues(xim, XNQueryInputStyle, &styles, NULL);
    if (failed_arg != NULL) {
        fprintf(stderr, "keypress() - XGetIMValues()");
    }
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, app, NULL);
    if (xic == NULL) {
        fprintf(stderr, "keypress() - XreateIC()");
    }
    XSetICFocus(xic);

    int count = 0;
    KeySym keysym = 0;
    char buffer[32];
    Status status = 0;
    count = Xutf8LookupString(xic, &event->xkey, buffer, 32, &keysym, &status);
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
        md_init.horiz += 0.01;
    } else if (keysym == 65363) {
        md_init.horiz -= 0.01;
    } else if (keysym == 65362) {
        md_init.vert += 0.01;
    } else if (keysym == 65364) {
        md_init.vert -= 0.01;
    } else if (keysym == 65293) {
        md_init.zoom *= 0.50;
    }

    if (transmitter())
        fprintf(stderr, "keypress() - transmitter()");

    pixmapupdate();
}
/* ##################################################################################################################### */
const void pixmapupdate(void) {

    XGCValues gc_vals;
    gc_vals.graphics_exposures = False;
    GC pix = XCreateGC(displ, app, GCGraphicsExposures, &gc_vals);

    pixmap = XCreatePixmap(displ, app, stat_app.width, stat_app.height, stat_app.depth);
    XCopyArea(displ, app, pixmap, pix, 0, 0, stat_app.width, stat_app.height, 0, 0);
    XFreeGC(displ, pix);
}
/* ##################################################################################################################### */
const void pixmapdisplay(void) {

    XGCValues gc_vals;
    gc_vals.graphics_exposures = False;
    GC pix = XCreateGC(displ, app, GCGraphicsExposures, &gc_vals);

    XCopyArea(displ, pixmap, app, pix, 0, 0, stat_app.width, stat_app.height, 0, 0);
    XFreeGC(displ, pix);
}
/* ##################################################################################################################### */
const void atomsinit(void) {

    /* Delete window initializer area */
    wmatom[App_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, app, &wmatom[App_Close], 1);

    /* Change main window Title */
    wmatom[App_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[Atom_Type] =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, app, wmatom[App_Name], wmatom[Atom_Type], 8, PropModeReplace, (unsigned char*)"Mandelbrot Set", 14);
}
/* ##################################################################################################################### */
// General initialization and event handling.
const int board() {

    XInitThreads();
    XEvent event;
    
    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        fprintf(stderr, "Board - XOpenDisplay()");
        return EXIT_FAILURE;
    }
    screen = XDefaultScreen(displ);
    XGetWindowAttributes(displ, XDefaultRootWindow(displ), &stat_root);
    
    /*  Root main Window */
    set_app.event_mask = EXPOSEMASKS | KEYBOARDMASKS | POINTERMASKS;
    set_app.background_pixel = 0x000000;
    app = XCreateWindow(displ, XDefaultRootWindow(displ), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &set_app);
    XMapWindow(displ, app);

    atomsinit();

    while (RUNNING) {

        XNextEvent(displ, &event);

        if (event.type == ConfigureNotify && event.xconfigure.send_event)
            continue;
        else 
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

