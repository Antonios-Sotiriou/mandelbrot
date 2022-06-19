#ifndef _BOARD_H
#define _BOARD_H 1

// general headers
#ifndef _STDIO_H
    #include <stdio.h>
#endif
#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

// multiprocessing includes
#ifndef _UNISTD_H
    #include <unistd.h>
#endif
#ifndef _SYS_WAIT_H
    #include <sys/wait.h>
#endif

// shared memory
#ifndef _SYS_IPC_H
    #include <sys/ipc.h>
#endif
#ifndef _SYS_SHM_H
    #include <sys/shm.h>
#endif
#ifndef _SEMAPHORE_H
    #include <semaphore.h>
#endif

// signals
#ifndef _SIGNAL_H
    #include <signal.h>
#endif

// HEADERS INCLUDED FOR TESTING
#include <time.h>

// Project specific headers
#ifndef _SHMEM_H
    #include "./shmem.h"
#endif
#ifndef _OBJECTS_H
    #include "./objects.h"
#endif
#ifndef _GLOBAL_VARS_H
    #include "./global_constants.h"
#endif
#ifndef _TRANSMITTER_H
    #include "./transmitter.h"
#endif

int board(const int pids[]);

#endif /* _BOARD_H */

