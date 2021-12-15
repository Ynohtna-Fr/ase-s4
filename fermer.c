#include "shm.h"
#include <sys/mman.h>

int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;

    CHECK(ainit("fermer") != -1);

    if (argc > 1 || argv[1] != NULL) {
        printf("usage: this programme don't take any arguments \n");
        exit(EXIT_FAILURE);
    }

    // get the shared memory
    vaccinodrome *vacci = getSharedMemmory();

    adebug(1, "Fermeture du vaccinodrome");
    vacci->isOpen = FALSE;

    // free the doctors
    for (int i = 0; i < vacci->maxDoctor; ++i) {
        CHECK(asem_post(&vacci->wait_patient) != -1);
    }

    // Free the patients
    for (int i = 0; i < vacci->nbrSeats; ++i) {
        CHECK(asem_post(&vacci->sema_seat) != -1);
    }

    adebug(1, "Fermeture définitive je supprime tout !");
    CHECK(shm_unlink(SHARED_VACCINODROME) != -1);
    CHECK(asem_destroy(&vacci->sema_seat) != -1);
    CHECK(asem_destroy(&vacci->sema_doctors) != -1);
    CHECK(asem_destroy(&vacci->lock_seat) != -1);
    CHECK(asem_destroy(&vacci->lock_doctors) != -1);
    CHECK(asem_destroy(&vacci->wait_patient) != -1);
    CHECK(asem_destroy(&vacci->wait_vaccination) != -1);
}
