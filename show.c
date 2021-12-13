#include <stdio.h>
#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>

int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;

    ainit("fermer");

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

    struct stat sharedLength;
    fstat(shm_fd, &sharedLength);

    vaccinodrome *vacci = mmap(NULL, sharedLength.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (vacci == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    boxDoctor * doctors = (boxDoctor *) &vacci->seats[vacci->nbrSeats - 1] + 1;
    for (int i = 0; i < vacci->nbrSeats; ++i) {
        printf("Siège %d | Libre ? %d | par : %s \n", i, vacci->seats[i].isTaken, vacci->seats[i].patient.name);
    }
    printf("----------\n");
    for (int i = 0; i < vacci->maxDoctor; ++i) {
        printf("box %d | Libre ? %d | par : %s \n", i, doctors[i].isTaken, doctors[i].patient.name);
    }

}