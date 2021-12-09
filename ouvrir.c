#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shm.h"

#define SHARED_VACCINODROME "/vaccinodrome-anthony-adam"

int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;
    // déclaration des variables
    int patientMaxNumber = 0;
    int doctorNumber = 0;
    int vaccinationTime = 0;

    printf("Hello ! \n");

    if (ainit("ouvrir") == -1) {
        perror("ainit");
        exit(EXIT_FAILURE);
    }

    if (argv[1] == NULL || atoi(argv[1]) <= 0) {
        printf("usage: error on arg 1 : <unsigned int> n, <unsigned int> m, "
                  "<unsigned utime> t\n");
        return 1;
    }
    patientMaxNumber = atoi(argv[1]);

    if (argv[2] == NULL || atoi(argv[2]) <= 0) {
        printf("usage: error on arg 2 : <unsigned int> n, <unsigned int> m, "
                  "<unsigned utime> t\n");
        return 1;
    }
    doctorNumber = atoi(argv[2]);

    if (argv[3] == NULL || atoi(argv[3]) <= 0) {
        printf("usage: error on arg 3 : <unsigned int> n, <unsigned int> m, "
                  "<uunsigned utime> t\n");
        return 1;
    }
    vaccinationTime = atoi(argv[3]);

    if (argc > 4) {
        printf("usage: error on arg 4 : <unsigned int> n, <unsigned int> m, "
                  "<unsigned utime> t\n");
        return 1;
    }

    asem_t sem_siege;
    asem_t sem_getPatient;
    asem_init(&sem_siege, "siege", 1, patientMaxNumber);
    asem_init(&sem_getPatient, "getPatient", 1, 1);

    char seats[patientMaxNumber];

    // create a shared memory
    int shmid = shm_open(SHARED_VACCINODROME, O_CREAT | O_RDWR |
                                              O_EXCL, 0666);
    if (shmid < 0) {
        perror("shm_open");
        return 1;
    }
    ftruncate(shmid, sizeof(vaccinodrome) + (patientMaxNumber * sizeof(char)));
    vaccinodrome *vacci_ptr = mmap(NULL, sizeof(vaccinodrome) + (patientMaxNumber * sizeof(char)),
                                   PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (vacci_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    vacci_ptr->maxDoctor = doctorNumber;
    vacci_ptr->timeToVaccinate = vaccinationTime;
    vacci_ptr->sem_seat = sem_siege;
    vacci_ptr->seats = seats;
    vacci_ptr->sem_getPatient = sem_getPatient;
    return 0 ;
}
