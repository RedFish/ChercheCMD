#include "semaphore.h"

int creation_semaphores(char * chaine, int nbs)
{
  key_t clef;
  clef = ftok(chaine, 'a');
  
  return semget(clef,nbs,IPC_CREAT|0666);
}

int initialisation_semaphores(int identifiant, int nbs, int valeur)
{
  int i;
  for(i=0; i<nbs; i++)
  {
    semctl(identifiant, i, SETVAL, valeur);
  }
  
  return 0;
}

int p(int identifiant, int sem)
{
  struct sembuf op;
  op.sem_num = sem;
  op.sem_op = -1;
  op.sem_flg = 0;
  semop(identifiant, &op, 1);
  
  return 0;
}

int v(int identifiant, int sem)
{
  struct sembuf op;
  op.sem_num = sem;
  op.sem_op = 1;
  op.sem_flg = 0;
  semop(identifiant, &op, 1);
  
  return 0;
}

int detruire_semaphores(int identifiant)
{
  semctl(identifiant, 1, IPC_RMID, 0);
  
  return 0;
}