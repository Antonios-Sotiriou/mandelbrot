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
    pthread_mutex_init(&mutex, NULL);

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
        if (pthread_create(&threads[i], NULL, &threader, &obj)) {
            printf("Thread number %d failed to start...Exit status 1\n", i);
            exit(1);
        }
        printf("Starting point: %d\n", obj.start_point);
        printf("Step point: %d\n", obj.step_point);
        obj.start_point = obj.step_point;
    }
    for (int i = 0; i < 10; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("Thread number %d failed to join...Exit status 1\n", i);
            exit(1);
        }
        printf("Thread Joined: %d\n", i);
    }

    for (int z = 0; z <= 100; z++) {
        printf("Image data : %c\n", obj.image_data[z]);
    }

    XImage *image = XCreateImage(obj.displ, obj.winattr->visual, obj.winattr->depth, ZPixmap, 0, obj.image_data, obj.winattr->width, obj.winattr->height, 32, 0);

    Pixmap pixmap = XCreatePixmap(obj.displ, obj.win, obj.winattr->width, obj.winattr->height, obj.winattr->depth);

    XPutImage(obj.displ, pixmap, obj.gc, image, 0, 0, 0, 0, obj.winattr->width, obj.winattr->height);

    XCopyArea(obj.displ, pixmap, obj.win, obj.gc, 0, 0, obj.winattr->width, obj.winattr->height, 0, 0);
    free(obj.image_data);
    XFree(image);

    pthread_mutex_destroy(&mutex);

    return 0;
}


