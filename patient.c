#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "shm.h"

// Fichier patient.c à rédiger
int main (int argc, char *argv []) {
    ainit("patient");
    (void) argc ;
    (void) argv ;

    if (argv[1] == NULL || strlen(argv[1]) > 10 || argc > 2) {
        printf("usage: <char[10]> patientName \n");
        return 1;
    }

    if (strcmp(argv[1], "") == 0) {
        printf("usage: <char[10]> patientName \n");
        return 1;
    }

    patient *p = malloc(sizeof(struct patient));
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
    if (shm_fd == -1) {
        perror("shm_open");
        adebug(1, "Le vaccinodrome n'est pas ouvert !");
        return 1;
    }
    struct stat sharedLength;
    fstat(shm_fd, &sharedLength);
    vaccinodrome *vacci = mmap(NULL, sharedLength.st_size,
                               PROT_READ | PROT_WRITE, MAP_SHARED,
                               shm_fd, 0);
    if (vacci == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    boxDoctor * doctors = (boxDoctor *) &vacci->seats[vacci->nbrSeats - 1] + 1;

    adebug(1, "%s : j'attend devant le vaccinodrome\n", p->name);
    asem_wait(&vacci->sema_seat);

    adebug(1, "%s : On me fait signe, je fonce !\n", p->name);
    if (vacci->isOpen == FALSE) {
        adebug(1, "%s : fu*k le vaccinodrome est fermé\n", p->name);
        exit(1);
    }

    fflush(stdout);
    adebug(1, "%s : c'est ouvert, je cherche une place\n", p->name);
    asem_wait(&vacci->lock_seat);
    //mutex to only access the seats one by one
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
    asem_post(&vacci->lock_seat);

    adebug(1, "%s : J'attend qu'un medecin ce libère", p->name);
    asem_wait(&vacci->sema_doctors);

    adebug(1, "%s : Un medecin est libre, je vais le chopper lui !", p->name);
    asem_wait(&vacci->lock_doctors);
    for (int i = 0; i < vacci->maxDoctor; ++i) {
        // if nobody is in the box, go into it
        if (doctors[i].isTaken == FALSE) {
            printf("patient %s medecin %d\n", p->name, i);
            doctors[i].isTaken = TRUE;
            asem_wait(&vacci->lock_seat);
            adebug(1, "%s : sur le siège %d je m'appel : %s\n", p->name, p->numSiege, vacci->seats[p->numSiege].patient.name);
            doctors[i].patient = vacci->seats[p->numSiege].patient;
            vacci->seats[p->numSiege].isTaken = FALSE;
            vacci->seats[p->numSiege].patient = undifined_patient;
            asem_post(&vacci->lock_seat);
            asem_post(&vacci->sema_seat);
            doctors[i].patient.numSiege = -1;
            doctors[i].patient.boxNumber = i;
            p->boxNumber = i;
            p->numSiege = -1;
            break;
        }
    }
    asem_post(&vacci->lock_doctors);
    asem_post(&vacci->wait_patient);

    adebug(1, "%s : Il me vaccine, je sais pas combien de temps ça prend", p->name);
    asem_wait(&vacci->wait_vaccination);
    adebug(1, "%s : Super je suis vacciné ! Je part bisous", p->name);
    asem_wait(&vacci->lock_doctors);
    doctors[p->boxNumber].isTaken = FALSE;
    doctors[p->boxNumber].patient = undifined_patient;
    asem_post(&vacci->lock_doctors);
    free(p);
}
