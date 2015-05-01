#ifndef FILES_H
#define FILES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
typedef struct Fichier Fichier;
#include "liste.h"

// Définition de plusieurs sémaphores pour analyser les fichiers
pthread_mutex_t mutex_liste;
pthread_mutex_t mutex_nb_analyses;
pthread_mutex_t mutex_prochaine_recherche;
pthread_mutex_t mutex_affichage;
int semaphore_listing;
int semaphore_affichage;

// Représentation de la structure d'un fichier
struct Fichier
{
  char * chemin;
  char * chemin_relatif;
  struct stat * infos;
};

// Renvoie une strucutre représentant les informations d'un fichier
Fichier * creer_fichier(char * chemin_fichier, char * chemin_relatif_fichier, int listing_long);

// Permet de lister le contenu d'un répertoire et le stocke dans une pile passée en paramètre
void listing(Liste ** liste, char * chemin_origine, char * chemin_relatif_origine, int listing_long);

// Fonction qui permet de chercher si un élément correspond à des critères de recherche
void * chercher(void * data);

// Affiche les résultats d'une recherche
void * afficher_resultats(void * data);

#endif