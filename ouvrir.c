#include <stdio.h>
#include <stdlib.h>

// Fichier ouvrir.c à rédiger
int main (int argc, char *argv []) {
    (void) argc ;
    (void) argv ;
    // déclaration des variables
    int patientNumber = 0;
    int doctorNumber = 0;
    int vaccinationTime = 0;

    if (argv[1] == NULL || argv[1] >= 0) {
        printf("Usage: %s <unsigned int> n, <unsigned int> m, <unsigned utime> t\n", argv[0]);
        return 1;
    }
    patientNumber = atoi(argv[1]);

    if (argv[2] == NULL || argv[2] >= 0) {
        printf("Usage: %s <unsigned int> n, <unsigned int> m, <unsigned utime> t\n", argv[0]);
        return 1;
    }
    doctorNumber = atoi(argv[2]);

    if (argv[3] == NULL || argv[3] >= 0) {
        printf("Usage: %s <unsigned int> n, <unsigned int> m, <uunsigned utime> t\n", argv[0]);
        return 1;
    }
    vaccinationTime = atoi(argv[3]);

    return 0 ;
}
