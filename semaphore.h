#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

int creation_semaphores(char * chaine,int nbs);
int initialisation_semaphores(int identifiant, int nbs, int valeur);
int p(int identifiant, int sem);
int v(int identifiant, int sem);
int detruire_semaphores(int identifiant);

#endif