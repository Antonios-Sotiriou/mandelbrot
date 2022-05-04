#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "header_files/threader.h"

#include "header_files/mutexes.h"
#include "header_files/objects.h"
#include "header_files/palette.h"

int iterator(Object obj) {

    pthread_t thread;
    pthread_mutex_init(&mutex, NULL);

    obj.image_data = calloc(1, sizeof(char) * obj.winattr->width * obj.winattr->height * 4);
    if (obj.image_data == NULL) {
        printf("Not Enough memory for the image_data to be allocated...Exit status 1");
        free(obj.image_data);
        exit(1);
    }
    for (int i = 0; i < 10; i++) {
        printf("I = %d\n", i);
    }

    pthread_create(&thread, NULL, &threader, &obj);
    pthread_join(thread, NULL);

    XImage *image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, obj.image_data, obj.winattr->width, obj.winattr->height, 32, 0);

    Pixmap pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);

    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);

    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    free(obj.image_data);
    XFree(image);

    pthread_mutex_destroy(&mutex);

    return 0;
}


