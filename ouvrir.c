#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "shm.h"

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
    size_t length = sizeof(vaccinodrome) +
            (patientMaxNumber * sizeof(waiting_seats)) +
            (sizeof(boxDoctor) * doctorNumber);

    // Semaphore declaration
    asem_t sem_siege;
    asem_t sem_doctors;
    asem_t lock_seat;
    asem_t lock_doctors;
    asem_t wait_patient;
    asem_t wait_vaccination;
    asem_init(&sem_siege, "sieges", 1, patientMaxNumber); // number of seats
    asem_init(&sem_doctors, "doctors", 1, 0); // +1 for each doctor when arrives
    asem_init(&lock_seat, "getPlace", 1, 1);
    asem_init(&lock_doctors, "lockBox", 1, 1);
    asem_init(&wait_patient, "getPatient", 1, 0);
    asem_init(&wait_vaccination, "waitVaccination", 1, 0);

    // create a shared memory
    int shmid = shm_open(SHARED_VACCINODROME, O_CREAT | O_RDWR |
                                              O_EXCL, 0666);
    if (shmid < 0) {
        perror("shm_open");
        return 1;
    }

    ftruncate(shmid, length);
    vaccinodrome *vacci_ptr = mmap(NULL, length,
                                   PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (vacci_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // this will add an array with box to the vaccinodrome structure.
    boxDoctor * doctorBox = (boxDoctor *) (vacci_ptr->seats + (patientMaxNumber * sizeof(waiting_seats)) + 1);

    patient undifined_patient = {
            "NULL",
            -1,
            -1
    };

    boxDoctor undifined_doctor = {
            FALSE,
            -1,
            undifined_patient
    };

    waiting_seats seats = {
            FALSE,
            undifined_patient
    };

    vacci_ptr->maxDoctor = doctorNumber;
    vacci_ptr->timeToVaccinate = vaccinationTime;
    vacci_ptr->nbrSeats = patientMaxNumber;
    vacci_ptr->isOpen = TRUE;

    vacci_ptr->sem_seat = sem_siege;
    vacci_ptr->sem_doctors = sem_doctors;
    vacci_ptr->lock_seat = lock_seat;
    vacci_ptr->lock_doctors = lock_doctors;
    vacci_ptr->wait_patient = wait_patient;
    vacci_ptr->wait_vaccination = wait_vaccination;

    for (int i = 0; i < patientMaxNumber; ++i) {
        vacci_ptr->seats[i] = seats;
    };
    for (int i = 0; i < doctorNumber; ++i) {
        doctorBox[i] = undifined_doctor;
    };

    return 0 ;
}
