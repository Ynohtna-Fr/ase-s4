#include <stdio.h>
#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// Fichier nettoyer.c à rédiger
int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;

    ainit("nettoyer");

    if (argc > 1 || argv[1] != NULL) {
        printf("usage: this programme don't take any arguments \n");
        return 1;
    }

    shm_unlink(SHARED_VACCINODROME);
}
