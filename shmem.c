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
#ifndef _SYS_IPC_H
    #include <sys/ipc.h>
#endif
#ifndef _SYS_SHM_H
    #include <sys/shm.h>
#endif

// object specific headers
#ifndef _SHMEM_H
    #include "header_files/shmem.h"
#endif

#define EXIT_FAILURE_NEGATIVE -1

// Create a key for accesing the shared memory.Returns -1 on error.
const key_t gorckey(const char *path, const int key_id) {

    key_t knot_key = ftok(path, key_id);
    if (knot_key == -1) {
        perror("gorckey()");
        return EXIT_FAILURE_NEGATIVE;
    }
    return knot_key;
}
// Create or get a shared memory according to the flag provided.Returns -1 on error.
const int crshmem(const key_t key, const size_t size, const int shmflag) {

    int shknot_id = shmget(key, size, shmflag);
    if (shknot_id == -1) {
        perror("crshmem()");
        return EXIT_FAILURE_NEGATIVE;
    }
    return shknot_id;
}
// Attach a shared memory and return a void pointer pointing to the location.Returns NULL on error.
void *attshmem(int shmid, const void *shmaddr, int shmflag) {

    void *ptr = shmat(shmid, shmaddr, shmflag);
    if (ptr == NULL) {
        perror("attshmem()");
        return NULL;
    }
    return ptr;
}
// Dettach a shared memory.Returns -1 on error.
const int dtshmem(const void *shmaddr) {

    if (shmdt(shmaddr) == -1) {
        perror("dtshmem()");
        return EXIT_FAILURE_NEGATIVE;
    }
    return EXIT_SUCCESS;
}
// Destroy a shared memory.Returns -1 on error.
int destshmem(const int shmid, const int cmd, struct shmid_ds *buf) {
    
    if (shmctl(shmid, cmd, buf) == -1) {
        perror("destshmem()");
        return EXIT_FAILURE_NEGATIVE;
    }
    return EXIT_SUCCESS;
}

