#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xlocale.h>

// multiprocessing includes
#include <unistd.h>
#include <signal.h>

// Time included for testing execution time
#include <time.h>

#include "header_files/objects.h"
#include "header_files/palette.h"
#include "header_files/iterator.h"

void signal_handler(int sig);

int main(int argc, char *argv[]) {

    struct sigaction sig = { 0 };
    sig.sa_handler = &signal_handler;
    sig.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sig, NULL);

    for (int i = 0; i < argc; i++) {
        printf("Arguments list %d = %s\n", i, argv[i]);
    }
    while (1) {
        sleep(1);
        printf("Process id step2: %d\n", getpid());
    }
}

void signal_handler(int sig) {
    char buffer[30] = "testing if function works\n";
    write(1, &buffer, sizeof(buffer));
}

