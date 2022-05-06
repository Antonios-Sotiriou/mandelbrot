#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "header_files/threader.h"

#include "header_files/mutexes.h"
#include "header_files/objects.h"
#include "header_files/palette.h"

int iterator(Object obj) {

    pthread_t threads[10];
    pthread_mutex_init(&iterMutex, NULL);

    obj.image_data = calloc(1, sizeof(char) * obj.winattr->width * obj.winattr->height * 4);
    if (obj.image_data == NULL) {
        printf("Not Enough memory for the image_data to be allocated...Exit status 1\n");
        free(obj.image_data);
        exit(1);
    }

    obj.step_counter = 0;
    obj.step_x = 0;
    obj.step_y = 0;
    obj.start_point = 0;

    for (int i = 0; i < 10; i++) {
        obj.step_point = (obj.winattr->width * obj.winattr->height / 10) * (i + 1);
        if (pthread_create(&threads[i], NULL, &threader, &obj) != 0) {
            printf("Thread number %d failed to start...Exit status 1\n", i);
            exit(1);
        }
        obj.start_point = obj.step_point;
        printf("Step x from main function: %d\n", obj.step_x);
    }
    for (int i = 0; i < 10; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Thread number %d failed to join...Exit status 1\n", i);
            exit(1);
        }
    }

    XImage *image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, obj.image_data, obj.winattr->width, obj.winattr->height, 32, 0);

    Pixmap pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);

    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);

    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    free(obj.image_data);
    XFree(image);

    pthread_mutex_destroy(&iterMutex);

    return 0;
}
