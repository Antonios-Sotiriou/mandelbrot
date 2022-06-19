#ifndef _EXTERNAL_VARS_H
#define _EXTERNAL_VARS_H 1

#ifndef _SHMEM_H
    #include <sys/ipc.h>
#endif

extern int shknot_id;
extern int shimage_id;
extern key_t knot_key;
extern key_t image_key;

#endif /* _EXTERNAL_VARS_H */