#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "shm.h"

int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;
    int patientMaxNumber = 0;
    int doctorNumber = 0;
    int vaccinationTime = 0;

    CHECK(ainit("ouvrir") != -1);

    if (argv[1] == NULL || atoi(argv[1]) <= 0) {
        printf("usage: error on arg 1 : <int> n, <int> m, "
               "<unsigned utime> t\n");
        exit(EXIT_FAILURE);
    }
    patientMaxNumber = atoi(argv[1]);

    if (argv[2] == NULL || atoi(argv[2]) <= 0) {
        printf("usage: error on arg 2 : <int> n, <int> m, "
               "<unsigned utime> t\n");
        exit(EXIT_FAILURE);
    }
    doctorNumber = atoi(argv[2]);

    if (argv[3] == NULL || atoi(argv[3]) < 0) {
        printf("usage: error on arg 3 : <int> n, <int> m, "
               "<uunsigned utime> t\n");
        exit(EXIT_FAILURE);
    }
    vaccinationTime = atoi(argv[3]);

    if (argc > 4) {
        printf("usage: error on arg 4 : <int> n, <int> m, "
               "<unsigned utime> t\n");
        exit(EXIT_FAILURE);
    }

    // Calc the total size of the shared memory one time.
    size_t length = sizeof(vaccinodrome) +
                    (patientMaxNumber * sizeof(waiting_seats)) +
                    (sizeof(boxDoctor) * doctorNumber);

    // Semaphore declaration
    asem_t sema_siege;
    asem_t sema_doctors;
    asem_t lock_seat;
    asem_t lock_doctors;
    asem_t wait_patient;
    asem_t wait_vaccination;
    asem_t wait_close;
    // number of seats
    CHECK(asem_init(&sema_siege, "seats", 1, patientMaxNumber) != -1);
    //+1 for each doctor when arrives
    CHECK(asem_init(&sema_doctors, "doctors", 1, 0) != -1);
    CHECK(asem_init(&lock_seat, "lockSeats", 1, 1) != -1);
    CHECK(asem_init(&lock_doctors, "lockBox", 1, 1) != -1);
    CHECK(asem_init(&wait_patient, "waitPatient", 1, 0) != -1);
    CHECK(asem_init(&wait_vaccination, "waitVaccination", 1, 0) != -1);
    CHECK(asem_init(&wait_close, "waitclose", 1, 0) != -1);

    // shared memory initialization and allocation
    int shmid = shm_open(SHARED_VACCINODROME, O_CREAT | O_RDWR |
                                              O_EXCL, 0666);
    CHECK(shmid != -1);

    CHECK(ftruncate(shmid, (__off_t)length) != -1);

    vaccinodrome *vacci_ptr = mmap(NULL, length,
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED, shmid, 0);
    if (vacci_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // this will add an array with doctors box to the vaccinodrome structure.
    boxDoctor * doctorBox = (boxDoctor *)
            &vacci_ptr->seats[patientMaxNumber - 1] + 1;

    // NULL placeholder patient
    patient undifined_patient = {
            "NULL",
            -1,
            -1
    };

    // NULL placeholder doctor
    boxDoctor undifined_doctor = {
            FALSE,
            -1,
            undifined_patient
    };

    // NULL placeholder seat
    waiting_seats seats = {
            FALSE,
            undifined_patient
    };

    // See @shm.h for definition.
    vacci_ptr->maxDoctor = doctorNumber;
    vacci_ptr->timeToVaccinate = vaccinationTime;
    vacci_ptr->nbrSeats = patientMaxNumber;
    vacci_ptr->isOpen = TRUE;
    vacci_ptr->sema_seat = sema_siege;
    vacci_ptr->sema_doctors = sema_doctors;
    vacci_ptr->lock_seat = lock_seat;
    vacci_ptr->lock_doctors = lock_doctors;
    vacci_ptr->wait_patient = wait_patient;
    vacci_ptr->wait_vaccination = wait_vaccination;
    vacci_ptr->wait_close = wait_close;
    // Initialize the seats with placeholders
    for (int i = 0; i < patientMaxNumber; ++i) {
        vacci_ptr->seats[i] = seats;
    };
    // Initialize the doctor box with placeholders
    for (int i = 0; i < doctorNumber; ++i) {
        doctorBox[i] = undifined_doctor;
    };

    return 0 ;
}
