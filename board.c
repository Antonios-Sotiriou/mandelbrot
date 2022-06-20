// Project includes
#ifndef _BOARD_H
    #include "header_files/board.h"
#endif

// #define EMVADON (obj.winattr->width * obj.winattr->height)
#define YPOLOIPON (event->xconfigure.width * event->xconfigure.height % PROC_NUM)

enum { Win_Close, Win_Name, Atom_Type, Atom_Last};

#define POINTERMASKS              ( ButtonPressMask )
#define KEYBOARDMASKS             ( KeyPressMask )
#define EXPOSEMASKS               ( StructureNotifyMask )

// Some Global Variables
Display *displ;
Window win;
XWindowAttributes rootattr, winattr;
XSetWindowAttributes setattr;
Object obj;
Pixmap pixmap;
Atom wmatom[Atom_Last];
KNOT *sh_knot;

extern int shknot_id;
extern int shimage_id;
extern key_t image_key;

static int MAPCOUNT = 0;
static int FULLSCREEN = 0;
static int OLDWIDTH = WIDTH;
static int OLDHEIGHT = HEIGHT;
static int RUNNING = 1;

// initialize the knot object to be transfered because we can't transfer pointers to pointers through shared memory.
static void init_knot(KNOT *knot);
static const void clientmessage(XEvent *event, const int pids[]);
static const void reparentnotify(XEvent *event, const int pids[]);
static const void mapnotify(XEvent *event, const int pids[]);
static const void resizerequest(XEvent *event, const int pids[]);
static const void configurenotify(XEvent *event, const int pids[]);
static const void buttonpress(XEvent *event, const int pids[]);
static const void keypress(XEvent *event, const int pids[]);
static const void pixmapupdate(void);
static const void pixmapdisplay(void);
static const void atomsinit(void);
static void (*handler[LASTEvent]) (XEvent *event, const int pids[]) = {
    [ClientMessage] = clientmessage,
    [ReparentNotify] = reparentnotify,
    [MapNotify] = mapnotify,
    [ResizeRequest] = resizerequest,
    [ConfigureNotify] = configurenotify,
    [ButtonPress] = buttonpress,
    [KeyPress] = keypress,
};

static const void clientmessage(XEvent *event, const int pids[]) {

    printf("clientmessage event received\n");
    if (event->xclient.data.l[0] == wmatom[Win_Close]) {
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

        XFreePixmap(displ, pixmap);
        XDestroyWindow(displ, win);
        XCloseDisplay(displ);
        
        RUNNING = 0;
    }
}
static const void reparentnotify(XEvent *event, const int pids[]) {

    printf("reparentnotify event received\n");
    if (event->xreparent.parent != win) {

        XGetWindowAttributes(displ, win, &winattr);
        obj.winattr = &winattr;
    }
}
static const void mapnotify(XEvent *event, const int pids[]) {

    printf("mapnotify event received\n");

    if (MAPCOUNT) {
        pixmapdisplay();
    } else {
        if (!MAPCOUNT)
            MAPCOUNT = 1;
    }
}
static const void resizerequest(XEvent *event, const int pids[]) {

    printf("resizerequest event received\n");
    if (FULLSCREEN) {
        obj.winattr->width = winattr.width = event->xconfigure.width;
        obj.winattr->height = winattr.height = event->xconfigure.height;
    } else {
        obj.winattr->width = winattr.width = OLDWIDTH;
        obj.winattr->height = winattr.height = OLDHEIGHT;
    }

    if (!XPending(displ)) {

        init_knot(sh_knot);

        if (transmitter(obj, pids))
            fprintf(stderr, "Warning: Board - resizerequest Event - transmitter()\n");

        pixmapupdate();
    } else {
        return;
    }
}
static const void configurenotify(XEvent *event, const int pids[]) {

    printf("configurenotify event received\n");
    if (MAPCOUNT) {
        if (destshmem(shimage_id, IPC_RMID, 0))
            fprintf(stderr, "Warning: Board - configuenotify Event - shimage_id - destshmem()\n");
    }

    if ((event->xconfigure.width == rootattr.width && event->xconfigure.height == (rootattr.height - (event->xconfigure.y * 2)))) {
        FULLSCREEN = 1;
    } else if ((event->xconfigure.width == OLDWIDTH && event->xconfigure.height == OLDHEIGHT) && FULLSCREEN) {
        FULLSCREEN = 0;
    } else {
        OLDWIDTH = event->xconfigure.width;
        OLDHEIGHT = event->xconfigure.height;
    }

    // Create a shared image memory.We do it here because we need to recreate it if user resizes the window.
    shimage_id = crshmem(image_key, event->xconfigure.width * event->xconfigure.height * 4, 0666 | IPC_CREAT);
    if (shimage_id == -1)
        fprintf(stderr, "Warning: Board - configurenotify Event - crshmem()\n");

    event->type = ResizeRequest;
    XSendEvent(displ, win, False, StructureNotifyMask, event);
}
static const void buttonpress(XEvent *event, const int pids[]) {

    printf("buttonpress event received\n");
    if (obj.init_x == 0.00 && obj.init_y == 0.00) {
        obj.init_x = (((double)event->xbutton.x - (obj.winattr->width / obj.horiz)) / (obj.winattr->width / obj.zoom));
        obj.init_y = (((double)event->xbutton.y - (obj.winattr->height / obj.vert)) / (obj.winattr->height / obj.zoom));
    } else {
        obj.init_x = obj.init_x + (((double)event->xbutton.x - (obj.winattr->width / obj.horiz)) / (obj.winattr->width / obj.zoom));
        obj.init_y = obj.init_y + (((double)event->xbutton.y - (obj.winattr->height / obj.vert)) / (obj.winattr->height / obj.zoom));
    }

    if (event->xkey.keycode == 1) {
        obj.zoom *= 0.50;
    } else if (event->xkey.keycode == 3) {
        obj.zoom /= 0.50;
    }

    init_knot(sh_knot);
    transmitter(obj, pids);

}
static const void keypress(XEvent *event, const int pids[]) {

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
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, win, NULL);
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

    init_knot(sh_knot);
    transmitter(obj, pids);
}
static const void pixmapupdate(void) {

    XGCValues gc_vals;
    gc_vals.graphics_exposures = False;
    GC pix = XCreateGC(displ, win, GCGraphicsExposures, &gc_vals);

    pixmap = XCreatePixmap(displ, win, winattr.width, winattr.height, winattr.depth);
    XCopyArea(displ, win, pixmap, pix, 0, 0, winattr.width, winattr.height, 0, 0);
    XFreeGC(displ, pix);
}
static const void pixmapdisplay(void) {

    XGCValues gc_vals;
    gc_vals.graphics_exposures = False;
    GC pix = XCreateGC(displ, win, GCGraphicsExposures, &gc_vals);

    XCopyArea(displ, pixmap, win, pix, 0, 0, winattr.width, winattr.height, 0, 0);
    XFreeGC(displ, pix);
}
static const void atomsinit(void) {

    /* Delete window initializer area */
    wmatom[Win_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, win, &wmatom[Win_Close], 1);

    /* Change main window Title */
    wmatom[Win_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[Atom_Type] =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, win, wmatom[Win_Name], wmatom[Atom_Type], 8, PropModeReplace, (unsigned char*)"Mandelbrot Set", 14);
}
// General initialization and event handling.
const int board(const int pids[]) {

    // shared memory
    // object to transfer between processes the integer variables for each process calculations.
    sh_knot = attshmem(shknot_id, NULL, SHM_RND);
    if (sh_knot == NULL)
        fprintf(stderr, "Warning: Board - sh_knot - attshmem()\n");

    // The shared image data key.
    image_key = gorckey("./keys/image_key.txt", 8899);
    if (image_key == -1)
        fprintf(stderr, "Warning: Board - image_key - gorckey()\n");
    // The shared image data memory id.We define it here so it is available in the event loop further down.

    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        fprintf(stderr, "Warning: Board - XOpenDisplay()\n");
        return EXIT_FAILURE;
    } else {
        obj.displ = displ;
    }

    int screen = XDefaultScreen(displ);
    XGetWindowAttributes(displ, XDefaultRootWindow(displ), &rootattr);

    /*  Root main Window */
    setattr.event_mask = EXPOSEMASKS | KEYBOARDMASKS | POINTERMASKS;
    setattr.background_pixel = 0x000000;
    win = XCreateWindow(displ, XRootWindow(displ, screen), 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &setattr);
    XMapWindow(displ, win);

    atomsinit();

    obj.displ = displ;
    obj.win = win;
    obj.max_iter = ITERATIONS;
    obj.horiz = HORIZONTAL;
    obj.vert = VERTICAL;
    obj.zoom = ZOOM;
    obj.init_x = 0;
    obj.init_y = 0;

    XEvent event;
    while (RUNNING) {

        XNextEvent(displ, &event);
        
        if (event.type == ConfigureNotify && event.xconfigure.send_event)
            continue;
        else 
            if (handler[event.type])
                handler[event.type](&event, pids);
    }
    return EXIT_SUCCESS;
}

static void init_knot(KNOT *knot) {

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

