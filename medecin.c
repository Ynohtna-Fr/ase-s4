#include <stdio.h>
#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;

    ainit("medecin");

    if (argc > 1 || argv[1] != NULL) {
        printf("usage: this programme don't take any arguments \n");
        return 1;
    }
    // get the shared memory
    int shm_fd = shm_open(SHARED_VACCINODROME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        adebug(1, "Le vaccinodrome n'est pas ouvert !");
        return 1;
    }
}
