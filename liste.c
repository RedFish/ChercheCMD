#include "liste.h"

void free_liste(Liste * liste)
{
  free(liste->fichier->chemin);
  free(liste->fichier->chemin_relatif);
  if(!liste->fichier->infos)
    free(liste->fichier->infos);
  free(liste->fichier);
}

void ajouter(Liste ** liste, Fichier * fichier)
{
  Liste * element = malloc(sizeof(Liste));  
  element->fichier = fichier;
  element->suivant = NULL;
  element->checked = -1;

  if(*liste == NULL)
    *liste = element;
  else
  {
    Liste * tmp = *liste;
    while(tmp->suivant)
      tmp = tmp->suivant;
    tmp->suivant = element;
  }
}