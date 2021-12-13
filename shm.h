// Fichier shm.h à rédiger
#include "./asem.h"
#define SHARED_VACCINODROME "/vaccinodrome-anthony-adam"

#define TRUE 1
#define FALSE 0

#ifndef CHECK_H
#define CHECK_H
#include <stdio.h> // For fprintf, perror
#include <stdlib.h> // For exit

#define CHECK(x) \
  do { \
    if (!(x)) { \
      fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
      perror(#x); \
      exit(EXIT_FAILURE); \
    } \
  } while (0)

#endif /* CHECK_H */

typedef struct patient {
    char name[10]; // name of the patient
    int numSiege; // number of the patient's seat
    int boxNumber; // number of the patient's box
} patient;

typedef struct waiting_seats {
    int isTaken; // check is the seats is taken or not by a patient
    patient patient; // patient in the seat
} waiting_seats;

typedef struct boxDoctor {
    int isTaken; // Taken = a patient is in the box
    int medecinNumber; // number of the doctor who take the box. -1 = no doctor
    patient patient; // patient in the box
} boxDoctor;

typedef struct vaccinodrome {
    int maxDoctor; // maximum number of doctors
    int timeToVaccinate; // time to vaccinate
    int nbrSeats; // number of seats
    int isOpen; // check if the room is open or not
    asem_t sema_seat; // semaphore for the seats
    asem_t sema_doctors; // semaphore lock for the patient to wait for the doctor
    asem_t lock_seat; // semaphore lock for the patient to get a seat
    asem_t lock_doctors; // semaphore lock for the patient to wait for the doctor
    asem_t wait_patient; // semaphore lock for the doctor to get a patient
    asem_t wait_vaccination; // semaphore lock for the patient to wait for the vaccination
    asem_t wait_close; // semaphore lock for the patient to wait for the vaccination
    waiting_seats seats[]; // array of the seats
    // implicite : boxDoctor doctorBox[]; // array of the boxes
} vaccinodrome;

