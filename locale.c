// general headers
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xlocale.h>

// Locale optimisation.Returns 0 if no error 1 otherwise.
int locale_init() {

    if (setlocale(LC_ALL, "") == NULL) {
        perror("setlocale()");
        return 1;
    }
    if (!XSupportsLocale()) {
        perror("XSupportsLocale()");
        return 1;
    }
    if (XSetLocaleModifiers("") == NULL) {
        perror("XSetLocaleModifiers()");
        return 1;
    }

    return 0;
}

