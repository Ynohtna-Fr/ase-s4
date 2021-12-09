#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "shm.h"

// Fichier patient.c à rédiger
int main (int argc, char *argv []) {
    ainit("Patient");
    (void) argc ;
    (void) argv ;
    char name[10];

    if (argv[1] == NULL || strlen(argv[1]) > 10 || argc > 1) {
        printf("usage: <char[10]> patientName \n");
        return 1;
    }
    memcpy(name, argv[1], strlen(argv[1]));
    adebug(1, "Patient %s :\n", name);

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
    printf("%d\n", vacci->maxDoctor);

}
