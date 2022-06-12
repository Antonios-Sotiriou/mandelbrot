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

/* semaphore specific includes */
#ifndef _SEMAPHORE_H
    #include <semaphore.h>
#endif

// Unlinks a semaphore is it was previously created and didn't closed properly.Returns -1 on error otherwise 0.
const int unlinksem(const char *name) {

    if (sem_unlink(name) == -1) {
        if (errno != ENOENT) {
            perror("sem_unlink()");
            return EXIT_FAILURE_NEGATIVE;
        }
    }
    return EXIT_SUCCESS;
}
// Opens a semaphore with given name.On error in returns SEM_FAILED which is NULL , 0.
sem_t *opensem(const char *name, int flag, mode_t mode, unsigned int value) {

    sem_t *sem;
    sem = sem_open(name, flag, mode, value);
    if (sem == SEM_FAILED) {
        perror("sem_open()");
        return SEM_FAILED;
    }
    return sem;
}
// Closes an open semaphore.Returns -1 on error otherwise 0.
const int closesem(sem_t *sem) {

    if (sem_close(sem) == -1) {
        perror("sem_close()");
        return EXIT_FAILURE_NEGATIVE;
    }
    return EXIT_SUCCESS;
}
// Waits for a semaphore.Returns -1 on error otherwise 0.
const int waitsem(sem_t *sem) {

    if (sem_wait(sem) == -1) {
        if (errno != EINTR) {
            perror("sem_wait()");
            return EXIT_FAILURE_NEGATIVE;
        }
    }
    return EXIT_SUCCESS;
}
// Posts to a waited semaphore to continue execution.Returns -1 on error otherwise 0.
const int postsem(sem_t *sem) {

    if (sem_post(sem) == -1) {
        perror("sem_post()");
        return EXIT_FAILURE_NEGATIVE;
    }
    return EXIT_SUCCESS;
}

