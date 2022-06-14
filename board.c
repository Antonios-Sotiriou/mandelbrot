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
#include <X11/Xlib.h>
#include "header_files/locale.h"

#define EXIT_FAILURE_NEGATIVE     -1                                              // Some functions need to return a negative value;
#define THREADS_NUM               7                                               // The number of Threads.
#define WIDTH                     800                                             // App starting window width.
#define HEIGHT                    800                                             // App starting window height.
#define POINTERMASKS              ( ButtonPressMask )
#define KEYBOARDMASKS             ( KeyPressMask )
#define EXPOSEMASKS               ( StructureNotifyMask | SubstructureNotifyMask | VisibilityChangeMask | ExposureMask )

enum { App_Close, App_Name, App_Maximized, App_Iconified, App_NormalState, Atom_Type, Atom_Last};

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

static void mandelbrot_init(void);
void painter(const Mandelbrot md, char *image_data);
void receiver(Mandelbrot md);
void *oscillator(void *args);
const int transmitter(void);
const void clientmessage(XEvent *event);
void configurerequest(XEvent *event);
void configurenotify(XEvent *event);
void reparentnotify(XEvent *event);
void visibilitychange(XEvent *event);
const void mapnotify(XEvent *event);
void resizerequest(XEvent *event);
void unmapnotify(XEvent *event);
void expose(XEvent *event);
void noexpose(XEvent *event);
void buttonpress(XEvent *event);
void keypress(XEvent *event);
const void pixmapupdate(void);
const void pixmapdisplay(void);
const void atomsinit(void);
int board();

// Global Variables
Display *displ;
XWindowAttributes stat_root, stat_app;
XSetWindowAttributes set_app;
Window app;
Pixmap pixmap;
static int screen;
static int MAPCOUNT = 0;
static int ICONIFIED = 0;
static int FULLSCREEN = 0;
static int OLDWIDTH = WIDTH;
static int OLDHEIGHT = HEIGHT;
static int RUNNING = 1; 
static const int ITERATIONS = 1000;                                       // Number of mandelbrot iterations.
static const double HORIZONTAL = 2.00;                                    // Horizontal scale.
static const double VERTICAL = 2.00;                                      // Vetical scale.
static const double ZOOM = 4.00;                                          // Mandelbrot starting zoom.
static void (*handler[LASTEvent]) (XEvent *event) = {
	[ClientMessage] = clientmessage,
	// [DestroyNotify] = destroynotify,
	// [EnterNotify] = enternotify,
    [ReparentNotify] = reparentnotify,
	[MapNotify] = mapnotify,
    [VisibilityNotify] = visibilitychange,
	[Expose] = expose,
	[NoExpose] = noexpose,
	[ResizeRequest] = resizerequest,
	[ConfigureRequest] = configurerequest,
	[ConfigureNotify] = configurenotify,
	[UnmapNotify] = unmapnotify,
	// [FocusIn] = focusin,
	// [MappingNotify] = mappingnotify,
	[ButtonPress] = buttonpress,
	[KeyPress] = keypress,
	// [MotionNotify] = motionnotify,
	// [PropertyNotify] = propertynotify
};
static Atom wmatom[Atom_Last];
Mandelbrot md_init;
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
    GC graphics = XCreateGC(displ, app, 0, NULL);

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
    printf("Atom message_type: %lu\n", event->xclient.data.l[0]);
    
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
            perror("mapnotify() - transmitter()");
    }
}
/* ##################################################################################################################### */
const void mapnotify(XEvent *event) {

    printf("mapnotify event received\n");

    if (MAPCOUNT) {
        printf("Other Mapnotify\n");
        pixmapdisplay();
    } else {
        printf("1st Mapnotify\n");
        pixmapupdate();
        MAPCOUNT = 1;
        XSync(displ, True);
    }
}
/* ##################################################################################################################### */
void visibilitychange(XEvent *event) {

    printf("visibilitychange event received\n");
}
/* ##################################################################################################################### */
void expose(XEvent *event) {

    printf("expose event received.Count: %d\n", event->xexpose.count);
}
/* ##################################################################################################################### */
void noexpose(XEvent *event) {

    printf("noexpose event received\n");
}
/* ##################################################################################################################### */
void resizerequest(XEvent *event) {

    printf("resizerequest event received\n");
    if (FULLSCREEN) {
        md_init.width = stat_app.width = event->xresizerequest.width;
        md_init.height = stat_app.height = event->xresizerequest.height;
    } else {
        md_init.width = stat_app.width = OLDWIDTH;
        md_init.height = stat_app.height = OLDHEIGHT;
    }

    if (transmitter())
        perror("resizerequest() - transmitter()");

    pixmapupdate();
}
/* ##################################################################################################################### */
void configurerequest(XEvent *event) {

    printf("configurerequest event received\n");
}
/* ##################################################################################################################### */
void configurenotify(XEvent *event) {

    printf("configurenotify event received\n");
    if (event->xconfigure.width == stat_root.width && event->xconfigure.height == (stat_root.height - event->xconfigure.y)) {
        event->type = ResizeRequest;
        event->xresizerequest.width = event->xconfigure.width;
        event->xresizerequest.height = event->xconfigure.height;
        FULLSCREEN = 1;
        XSync(displ, True);
    } else if (event->xconfigure.width == OLDWIDTH && event->xconfigure.height == OLDHEIGHT && FULLSCREEN) {
        event->type = ResizeRequest;
        event->xresizerequest.width = event->xconfigure.width;
        event->xresizerequest.height = event->xconfigure.height;
        FULLSCREEN = 0;
        XSync(displ, True);
    } else {
        event->type = ResizeRequest;
        event->xresizerequest.width = event->xconfigure.width;
        event->xresizerequest.height = event->xconfigure.height;
    }
    XSendEvent(displ, app, False, SubstructureNotifyMask, event);
}
/* ##################################################################################################################### */
void unmapnotify(XEvent *event) {

    printf("unmapnotify event received\n");
    ICONIFIED = 1;
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
        perror("buttonpress() - transmitter()");

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
        perror("keypress() - XOpenIM()");
    }
    failed_arg = XGetIMValues(xim, XNQueryInputStyle, &styles, NULL);
    if (failed_arg != NULL) {
        perror("keypress() - XGetIMValues()");
    }
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, app, NULL);
    if (xic == NULL) {
        perror("keypress() - XreateIC()");
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
        perror("keypress() - transmitter()");

    pixmapupdate();
}
/* ##################################################################################################################### */
const void pixmapupdate(void) {

    GC pix = XCreateGC(displ, app, 0, NULL);

    pixmap = XCreatePixmap(displ, app, stat_app.width, stat_app.height, stat_app.depth);
    XCopyArea(displ, app, pixmap, pix, 0, 0, stat_app.width, stat_app.height, 0, 0);
    XFreeGC(displ, pix);
}
/* ##################################################################################################################### */
const void pixmapdisplay(void) {

    GC pix = XCreateGC(displ, app, 0, NULL);

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

    wmatom[App_Maximized] = XInternAtom(displ, "_NET_WM_STATE_MAXIMIZED", True);
    if (wmatom[App_Maximized] == None)
        perror("No atom found.\n");

    wmatom[App_Iconified] = XInternAtom(displ, "_NET_WM_STATE_ICONIFIED", True);
    if (wmatom[App_Maximized] == None)
        perror("No atom found.\n");

    wmatom[App_NormalState] = XInternAtom(displ, "_NET_WM_STATE_NORMAL", True);
    if (wmatom[App_Maximized] == None)
        perror("No atom found.\n");

}
/* ##################################################################################################################### */
// General initialization and event handling.
const int board() {

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

    atomsinit();

    while (RUNNING) {
        printf("Event type: %2d  -->>  ", event.type);
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

