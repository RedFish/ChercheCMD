#ifndef LISTE_H
#define LISTE_H
typedef struct Liste Liste;
#include "file.h"

// Structure représentant une liste
struct Liste
{
  Fichier * fichier;
  struct Liste * suivant;
  int checked;
};

// Permet de vider la liste
void free_liste(Liste * liste);

// Permet d'ajouter un fichier dans la liste
void ajouter(Liste ** liste, Fichier * fichier);

#endif