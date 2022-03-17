#include <stdio.h>
#include <X11/Xlib.h>

#include "header_files/palette.h"
#include "header_files/objects.h"

void painter(Object obj, int n, int x, int y) {

    if (n == obj.max_iter) {
        obj.values.foreground = pallete[n];
        XChangeGC(obj.displ, obj.gc, GCForeground, &obj.values);
        XDrawPoint(obj.displ, obj.win, obj.gc, x, y);                                
    } else if (n < obj.max_iter && n > 18) {
        obj.values.foreground = pallete[n];
        XChangeGC(obj.displ, obj.gc, GCForeground, &obj.values);
        XDrawPoint(obj.displ, obj.win, obj.gc, x, y);                                                    
    } else if (n < obj.max_iter && n <= 18) {
        obj.values.foreground = 0x000000;
        XChangeGC(obj.displ, obj.gc, GCForeground, &obj.values);
        XDrawPoint(obj.displ, obj.win, obj.gc, x, y);
    }
}

