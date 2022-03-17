#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "header_files/objects.h"
#include "header_files/painter.h"
#include "header_files/mutexes.h"

int iterator(Object obj) {

    pthread_t threads[obj.winattr->width * obj.winattr->height];
    pthread_mutex_init(&mutex, NULL);

    int counter = 0;
    for (int x = 0; x <= obj.winattr->width; x++) {
        for (int y = 0; y <= obj.winattr->height; y++) {
            obj.x = x;
            obj.y = y;
            if (pthread_create(&threads[counter], NULL, &painter, &obj)) {
                exit(1);
            }
            if (pthread_join(threads[counter], NULL)) {
                exit(1);
            }
            counter++;
        }
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}

