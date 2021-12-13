#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;
    int myNum = -1;
    int stopWork = FALSE;

    CHECK(ainit("medecin") != -1);

    if (argc > 1 || argv[1] != NULL) {
        printf("usage: this programme don't take any arguments \n");
        exit(EXIT_FAILURE);
    }

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

    // Get the doctors box corresponding array
    boxDoctor * doctors = (boxDoctor *) &vacci->seats[vacci->nbrSeats - 1] + 1;

    // NULL placeholder patient
    patient undifined_patient = {
            "NULL",
            -1,
            -1
    };

    adebug(1, "Hello les collègues, je prend mon service, Quel numéro ajrd ?");
    CHECK(asem_wait(&vacci->lock_doctors) != -1);
    // Check wich box is free and set the doctor here
    for (int i = 0; i < vacci->maxDoctor; ++i) {
        if (doctors[i].medecinNumber == -1) {
            adebug(1, "Le box numéro : %d est libre, je prend !", i);
            doctors[i].medecinNumber = i;
            myNum = i;
            break;
        }
    }
    CHECK(asem_post(&vacci->lock_doctors) != -1);
    // All the box if full = doctor don't have any numbers.
    if (myNum == -1) {
        adebug(1, "Medecin déchu : On on à pas besoin de moi je bouge bisous");
        exit(EXIT_FAILURE);
    }

    fflush(stdout);
    do {
        adebug(1, "Medecin %d : Je suis prêt à traiter les patients", myNum);
        CHECK(asem_post(&vacci->sema_doctors) != -1);
        CHECK(asem_wait(&vacci->wait_patient) != -1);

        // if the vaccinodrome is close and there is no more patient in the box
        // or in the waiting room, the doctor will leave
        if (vacci->isOpen == FALSE && doctors[myNum].isTaken == FALSE) {
            adebug(1, "Medecin %d : Ah, c'est bientôt la fin !", myNum);
            int flag = FALSE;

            CHECK(asem_wait(&vacci->lock_seat) != -1);
            for (int i = 0; i < vacci->nbrSeats; ++i) {
                if (vacci->seats[i].isTaken == TRUE) {
                    flag = TRUE;
                    break;
                }
            }
            CHECK(asem_post(&vacci->lock_seat) != -1);

            if (flag == FALSE) {
                adebug(1, "Medecin %d : Aucun patient en attente", myNum);
                stopWork = TRUE;
                adebug(1, "Medecin %d : Je quitte ce job pour ajrd !", myNum);
                break;
            }
        }

        usleep(vacci->timeToVaccinate * 1000);
        // TODO : maybe move up the ligne before usleep
        printf("medecin %d vaccine %s\n", myNum,
               doctors[myNum].patient.name);
        CHECK(asem_post(&vacci->wait_vaccination) != -1);

        CHECK(asem_wait(&vacci->lock_doctors) != -1);
        doctors[myNum].isTaken = FALSE;
        doctors[myNum].patient = undifined_patient;
        CHECK(asem_post(&vacci->lock_doctors) != -1);
    } while (stopWork == FALSE);

}
