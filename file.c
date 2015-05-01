#include "file.h"
#include <stdio.h>

void * afficher_resultats(void * data)
{
  extern Liste ** prochain_affichage;
  extern int listing_long;
  extern long nb_analyses;
  Fichier * fichier;
  
  while(1)
  {
    if(*prochain_affichage)
    {
      if((*prochain_affichage)->checked != -1)
      {
	if((*prochain_affichage)->checked == 1)
	{
	  fichier = (*prochain_affichage)->fichier;
	  if(listing_long)
	  {
	    char * link;
	    char infos_fichier[11];
	    int n;
	    infos_fichier[10] = '\0';
	    if(S_ISDIR(fichier->infos->st_mode))
	      infos_fichier[0] = 'd';
	    else
	      if(S_ISLNK(fichier->infos->st_mode))
	      {
		infos_fichier[0] = 'l';
		link = malloc(fichier->infos->st_size + 1);
		n = readlink(fichier->chemin, link, fichier->infos->st_size + 1);
		link[n] = '\0';
	      }
	      else
		infos_fichier[0] = '-';
	    
	    infos_fichier[1] = fichier->infos->st_mode & S_IRUSR ? 'r' : '-';
	    infos_fichier[2] = fichier->infos->st_mode & S_IWUSR ? 'w' : '-';
	    infos_fichier[3] = fichier->infos->st_mode & S_IXUSR ? 'x' : '-';
	    infos_fichier[4] = fichier->infos->st_mode & S_IRGRP ? 'r' : '-';
	    infos_fichier[5] = fichier->infos->st_mode & S_IWGRP ? 'w' : '-';
	    infos_fichier[6] = fichier->infos->st_mode & S_IXGRP ? 'x' : '-';
	    infos_fichier[7] = fichier->infos->st_mode & S_IROTH ? 'r' : '-';
	    infos_fichier[8] = fichier->infos->st_mode & S_IWOTH ? 'w' : '-';
	    infos_fichier[9] = fichier->infos->st_mode & S_IXOTH ? 'x' : '-';
	    printf("%s %8ld %s",infos_fichier, (long)fichier->infos->st_size, fichier->chemin_relatif);
	    
	    if(S_ISLNK(fichier->infos->st_mode))
	    {
	      printf(" -> %s\n",link);
	      free(link);
	    }
	    else printf("\n");
	  }
	  else
	  {
	      printf("%s\n",fichier->chemin_relatif);
	  }
	}
	free_liste(*prochain_affichage);
	prochain_affichage = &((*prochain_affichage)->suivant);
	// Section critique (nb_analyses)
	pthread_mutex_lock(&mutex_nb_analyses);
	  nb_analyses++;
	pthread_mutex_unlock(&mutex_nb_analyses);
      }
    }
    else
      p(semaphore_affichage,0);
  }
}

Fichier * creer_fichier(char * chemin_fichier, char * chemin_relatif_fichier, int listing_long)
{
  Fichier * fichier = malloc(sizeof(Fichier));
  
  if(listing_long)
  {
    fichier->infos = malloc(sizeof(struct stat));
    lstat(chemin_fichier,fichier->infos);
  }
  fichier->chemin = malloc(sizeof(char)*(strlen(chemin_fichier)+1));
  strcpy(fichier->chemin, chemin_fichier);
  fichier->chemin_relatif = malloc(sizeof(char)*(strlen(chemin_relatif_fichier)+1));
  strcpy(fichier->chemin_relatif, chemin_relatif_fichier);
  
  return fichier;
}

void listing(Liste ** liste,char * chemin_origine, char * chemin_relatif_origine, int listing_long)
{
  DIR * repertoire_origine;
  char * chemin_fichier;
  char * chemin_relatif_fichier;
  Fichier * fichier;
  extern long nb_insertions;
  
  struct dirent ** liste_fichiers;
  struct dirent * fichier_courant;
  int n = scandir(chemin_origine, &liste_fichiers, 0, alphasort);
  int i;
  if(n != 0)
  {
    for(i=0; i<n; i++)
    {
      fichier_courant = liste_fichiers[i];
      if(strcmp(fichier_courant->d_name,".") && strcmp(fichier_courant->d_name,".."))
      {
      	// Si le fichier actuel est un répertoire
      	if(fichier_courant->d_type == 4)
      	{
      	  chemin_fichier = malloc(sizeof(char)*(strlen(chemin_origine)+strlen(fichier_courant->d_name)+2));
      	  strcpy(chemin_fichier, chemin_origine);
      	  strcat(chemin_fichier, fichier_courant->d_name);
      	  strcat(chemin_fichier, "/");
      	  
      	  chemin_relatif_fichier = malloc(sizeof(char)*(strlen(fichier_courant->d_name)+strlen(chemin_relatif_origine)+2));
      	  strcpy(chemin_relatif_fichier, chemin_relatif_origine);
      	  strcat(chemin_relatif_fichier, fichier_courant->d_name);
      	  strcat(chemin_relatif_fichier, "/");
      	  
      	  fichier = creer_fichier(chemin_fichier, chemin_relatif_fichier, listing_long);
      	  pthread_mutex_lock(&mutex_liste);
      	  ajouter(liste, fichier);
      	  pthread_mutex_unlock(&mutex_liste);
      	  
      	  nb_insertions++;
      	  v(semaphore_listing,0);
      	  listing(liste, chemin_fichier, chemin_relatif_fichier, listing_long);
      	}
      	else
      	{
      	  chemin_fichier = malloc(sizeof(char)*(strlen(chemin_origine)+strlen(fichier_courant->d_name)+1));
      	  strcpy(chemin_fichier, chemin_origine);
      	  strcat(chemin_fichier, fichier_courant->d_name);
      	  
      	  chemin_relatif_fichier = malloc(sizeof(char)*(strlen(fichier_courant->d_name)+strlen(chemin_relatif_origine)+1));
      	  strcpy(chemin_relatif_fichier, chemin_relatif_origine);
      	  strcat(chemin_relatif_fichier, fichier_courant->d_name);
      	  
      	  fichier = creer_fichier(chemin_fichier, chemin_relatif_fichier, listing_long);
      	  pthread_mutex_lock(&mutex_liste);
      	  ajouter(liste, fichier);
      	  pthread_mutex_unlock(&mutex_liste);
      	  nb_insertions++;
      	  v(semaphore_listing,0);
      	}	
      	free(chemin_fichier);
      	free(chemin_relatif_fichier);
      }
    }
  }
}

void * chercher(void * data)
{
  Fichier * fichier;
  extern long nb_insertions;
  extern long nb_analyses;
  extern char * motif;
  extern char * motif_T;
  extern int recherche_image;
  extern Liste ** prochaine_recherche;
  extern int listing_long;
  Liste * element;


  while(1)
  {
    p(semaphore_listing,0);
    pthread_mutex_lock(&mutex_prochaine_recherche);
    element = *prochaine_recherche;
    if(nb_analyses < nb_insertions)
      prochaine_recherche = &((*prochaine_recherche)->suivant);
    else
    {
      pthread_mutex_unlock(&mutex_prochaine_recherche);
      continue;
    }
    pthread_mutex_unlock(&mutex_prochaine_recherche);
    
    fichier = element->fichier;
  
    // En fonction des critères de recherche (image ou/et texte, on ajoute l'élément sur lequel on travaille
    if(  !motif && !recherche_image && !motif_T
      || motif && !recherche_image && !motif_T && isTextInFile(motif, fichier->chemin) == 1
      || !motif && recherche_image && !motif_T  && isPicture(fichier->chemin) == 1
      || !motif && !recherche_image && motif_T && isMatch(motif_T, fichier->chemin) == 1
      || motif && recherche_image && !motif_T  && isTextInFile(motif, fichier->chemin) == 1 && isPicture(fichier->chemin) == 1
      || motif && !recherche_image && motif_T && isMatch(motif_T, fichier->chemin) == 1 && isTextInFile(motif, fichier->chemin) == 1
      || !motif && recherche_image && motif_T && isMatch(motif_T, fichier->chemin) == 1 && isPicture(fichier->chemin) == 1
      || motif && recherche_image && motif_T && isMatch(motif_T, fichier->chemin) == 1 && isPicture(fichier->chemin) == 1 && isTextInFile(motif, fichier->chemin) == 1
      )
    {
      element->checked = 1;
    }
    else
    {
      element->checked = 0;
    }
    v(semaphore_affichage,0);
  }
}