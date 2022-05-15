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

#include "header_files/threader.h"

#include "header_files/objects.h"
#include "header_files/palette.h"
#include "header_files/iterator.h"

int LOOP_CON = 1;

void signal_handler(int sig);

int main(int argc, char *argv[]) {

    int sig_val;

    struct sigaction sig = { 0 };
    sig.sa_handler = &signal_handler;
    sig.sa_flags = SA_RESTART;
    sig_val = sigaction(SIGUSR1, &sig, NULL);

    for (int i = 0; i < argc; i++) {
        printf("Arguments list %d = %s\n", i, argv[i]);
    }

    printf("Signal received, entering main loop\n");
    printf("Signal value before loop: %d\n", sig_val);

    while (!sig_val) {
        printf("Entered main loop\n");
        sleep(1);
        printf("Process id step2: %d\n", getpid());
        if (LOOP_CON) {
            printf("Loop contition has been met, calling threader()\n");
            threader();
            printf("Threader finished, returning to loop\n");
            LOOP_CON = 0;
        }
    }
}

void signal_handler(int sig) {
    LOOP_CON = 1;
}

