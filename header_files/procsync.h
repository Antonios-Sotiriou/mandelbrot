#ifndef _PROCSYNC_H
#define _PROCSYNC_H 1

#ifndef _SYS_STAT_H
    #include <sys/stat.h>
#endif
#ifndef _SEMAPHORE_H
    #include <semaphore.h>
#endif

const int unlinksem(const char *name);
sem_t *opensem(const char *name, int flag, mode_t mode, unsigned int value);
const int closesem(sem_t *sem);
const int waitsem(sem_t *sem);
const int postsem(sem_t *sem);

#endif /* _PROCSYNC_H */