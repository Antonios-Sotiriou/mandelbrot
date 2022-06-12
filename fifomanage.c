// general headers
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// fifo headers
#include <sys/stat.h>
#include <sys/fcntl.h>

#define EXIT_FAILURE_NEGATIVE -1

        // FIFO IMPLEMENTATION
const int createfifo(const char *name, int flag) {
    if (mkfifo(name, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo()");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

const int openwrite(const char *name, int flag, const void *buff) {

    int fd = open(name, flag);
    if (fd == -1) {
        perror("openwrite() - open()");
        return EXIT_FAILURE_NEGATIVE;
    }
    if (write(fd, &buff, sizeof(buff)) == -1) {
        perror("write() fifo...");
        return EXIT_FAILURE_NEGATIVE;
    }
    close(fd);
    return EXIT_SUCCESS;
}

const int openread(const char *name, int flag, const void *buff) {
    int fd = open(name, flag);
    if (fd == -1) {
        perror("openread() - open()");
        return EXIT_FAILURE_NEGATIVE;
    }
    if (read(fd, &buff, sizeof(buff)) == -1) {
        perror("read() fifo...");
        return EXIT_FAILURE_NEGATIVE;
    }
    close(fd);
    return fd;
}

