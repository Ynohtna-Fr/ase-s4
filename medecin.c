#include <stdio.h>
#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;
    int myNum = 0;
    int stopWork = FALSE;

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

    struct stat sharedLength;
    fstat(shm_fd, &sharedLength);

    vaccinodrome *vacci = mmap(NULL, sharedLength.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (vacci == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    boxDoctor * doctors = (boxDoctor *) &vacci->seats[vacci->nbrSeats - 1] + 1;

    adebug(1, "Hello les collègues, je prend mon service, Quel numéro ajrd ?");
    asem_wait(&vacci->lock_doctors);
    for (int i = 0; i < vacci->maxDoctor; ++i) {
        if (doctors[i].medecinNumber == -1) {
            adebug(1, "Le box numéro : %d est libre, je prend !", i);
            doctors[i].medecinNumber = i;
            myNum = i;
            break;
        }
    }
    asem_post(&vacci->lock_doctors);
    fflush(stdout);
    do {
//        if (vacci->isOpen == FALSE) {
//            adebug(1, "Medecin %d : Je suis le dernier, je finalise la fermeture", myNum);
//            asem_post(&vacci->wait_close);
//            break;
//        }

        adebug(1, "Medecin %d : Je suis prêt à traiter les patients, je vais attendre les patients !", myNum);
        asem_post(&vacci->sema_doctors);
        asem_wait(&vacci->wait_patient);

        // if the vaccinodrome is close and there is no patient in the box, the doctor will leave
        if (vacci->isOpen == FALSE && doctors[myNum].isTaken == FALSE) {
            adebug(1, "Medecin %d : on m'a appeler pour rien ! Je quitte ce job claqué !", myNum);
            stopWork = TRUE;
            break;
        }

        usleep(vacci->timeToVaccinate * 1000);
        printf("medecin %d vaccine %s\n", myNum, doctors[myNum].patient.name);
        adebug(1, "Medecin %d : J'ai fini de traiter le patient '%s'", myNum, doctors[myNum].patient.name);
        asem_post(&vacci->wait_vaccination);
    } while (stopWork == FALSE);

}
