#include "shm.h"
#include <sys/mman.h>

int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;

    CHECK(ainit("nettoyer") != -1);

    if (argc > 1 || argv[1] != NULL) {
        printf("usage: this programme don't take any arguments \n");
        exit(EXIT_FAILURE);
    }

    // Don't check that function because if there is nothing to clean,
    // the test specified that it should not generate any error.
    shm_unlink(SHARED_VACCINODROME);
}
