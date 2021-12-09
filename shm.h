// Fichier shm.h à rédiger
#include "./asem.h"
#define SHARED_VACCINODROME "/vaccinodrome-anthony-adam"

typedef struct {
    asem_t sem_seat;
    asem_t sem_getPatient;
    int maxDoctor;
    int timeToVaccinate;
    char *seats;
} vaccinodrome;