#ifndef _TRANSMITTER_H
#define _TRANSMITTER_H 1

// general headers
#ifndef _STDIO_H
    #include <stdio.h>
#endif
#ifndef _STDLIB_H
    #include <stdlib.h>
#endif
#ifndef _ERRNO_H
    #include <errno.h>
#endif

// shared memory
#ifndef _SYS_SHM_H
    #include <sys/shm.h>
#endif

/* semaphore specific includes */
#ifndef _FCNTL_H
    #include <fcntl.h>
#endif

// signals
#ifndef _SIGNAL_H
    #include <signal.h>
#endif

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
#ifndef _THREADER_H
    #include "./threader.h"
#endif
#ifndef _PROCSYNC_H
    #include "./procsync.h"
#endif

const int transmitter(const Object obj, const int pids[]);

#endif /* _TRANSMITTER_H */

