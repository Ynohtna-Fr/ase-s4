// Fichier shm.c à rédiger
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "shm.h"

vaccinodrome * getSharedMemmory() {
    // get the shared memory
    int shm_fd = shm_open(SHARED_VACCINODROME, O_RDWR, 0666);
    CHECK(shm_fd != -1);

    struct stat sharedLength;
    CHECK(fstat(shm_fd, &sharedLength) != -1);

    vaccinodrome *vacci = mmap(NULL, sharedLength.st_size,
                               PROT_READ | PROT_WRITE, MAP_SHARED,
                               shm_fd, 0);
    if (vacci == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    return vacci;
}