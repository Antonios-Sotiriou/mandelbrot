#ifndef _SHMEM_H
#define _SHMEM_H 1

const key_t gorckey(const char *path, const int key_id);
const int crshmem(const key_t key, const size_t size, const int shmflag);
void *attshmem(int shmid, const void *shmaddr, int shmflag);
const int dtshmem(const void *shmaddr);
int destshmem(int shmid, int cmd, struct shmid_ds *buf);

#endif /* _SHMEM_H */

