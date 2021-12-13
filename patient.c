#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "shm.h"

int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;
    CHECK(ainit("patient") != -1);

    if (argv[1] == NULL || strlen(argv[1]) > 10 || argc > 2) {
        printf("usage: <char[10]> patientName \n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "") == 0) {
        printf("usage: <char[10]> patientName \n");
        exit(EXIT_FAILURE);
    }

    // current patient
    patient *p = malloc(sizeof(struct patient));

    // NULL patient for placeholder
    patient undifined_patient = {
            "NULL",
            -1,
            -1
    };

    strcpy(p->name, argv[1]);
    p->numSiege = -1;
    p->boxNumber = -1;

    adebug(1, "Patient %s :\n", p->name);

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
        return 1;
    }

    // Get the corresponding doctors array in the shared memory
    boxDoctor * doctors = (boxDoctor *) &vacci->seats[vacci->nbrSeats - 1] + 1;

    adebug(1, "%s : j'attend devant le vaccinodrome\n", p->name);
    CHECK(asem_wait(&vacci->sema_seat) != -1);

    adebug(1, "%s : On me fait signe, je fonce !\n", p->name);
    if (vacci->isOpen == FALSE) {
        adebug(1, "%s : fu*k le vaccinodrome est fermé, JE RALE\n", p->name);
        exit(EXIT_FAILURE);
    }

    fflush(stdout);
    adebug(1, "%s : c'est ouvert, je cherche une place\n", p->name);

    //mutex to only access the seats one by one
    CHECK(asem_wait(&vacci->lock_seat) != -1);
    for (int i = 0; i < vacci->nbrSeats; ++i) {
        if (vacci->seats[i].isTaken == FALSE) {
            p->numSiege = i;
            vacci->seats[i].patient = *p;
            vacci->seats[i].isTaken = TRUE;
            printf("patient %s siege %d\n", p->name, p->numSiege);
            break;
        }
        else {
            adebug(1, "%s : le siege %d est pris !", p->name, i);
        }
    }
    CHECK(asem_post(&vacci->lock_seat) != -1);

    adebug(1, "%s : J'attend qu'un medecin ce libère", p->name);
    CHECK(asem_wait(&vacci->sema_doctors) != -1);

    adebug(1, "%s : Un medecin est libre, je vais le chopper lui !", p->name);
    CHECK(asem_wait(&vacci->lock_doctors) != -1);
    for (int i = 0; i < vacci->maxDoctor; ++i) {
        // if nobody is in the box, go into it
        if (doctors[i].isTaken == FALSE) {
            printf("patient %s medecin %d\n", p->name, i);
            doctors[i].isTaken = TRUE;

            CHECK(asem_wait(&vacci->lock_seat) != -1);
            doctors[i].patient = vacci->seats[p->numSiege].patient;
            vacci->seats[p->numSiege].isTaken = FALSE;
            vacci->seats[p->numSiege].patient = undifined_patient;
            CHECK(asem_post(&vacci->lock_seat) != -1);

            CHECK(asem_post(&vacci->sema_seat) != -1);

            doctors[i].patient.numSiege = -1;
            doctors[i].patient.boxNumber = i;
            p->boxNumber = i;
            p->numSiege = -1;
            break;
        }
    }
    CHECK(asem_post(&vacci->lock_doctors) != -1);
    CHECK(asem_post(&vacci->wait_patient) != -1);

    adebug(1, "%s : Il me vaccine", p->name);
    CHECK(asem_wait(&vacci->wait_vaccination) != -1);
    adebug(1, "%s : Super je suis vacciné ! Je part bisous", p->name);
    free(p);
}
