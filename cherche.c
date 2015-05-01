#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "file.h"
#include "liste.h"
#include "semaphore.h"

// Variables globales
Liste * liste;

int listing_long = 0;
pthread_t * threads;
pthread_t * thread_affichage;
char *motif = NULL; //motif éventuel à chercher dans les fichiers
char *motif_T = NULL;
int recherche_image = 0;
int nb_threads = 1;
char *current_path;
long nb_insertions = 0;
long nb_analyses = 0;
Liste ** prochaine_recherche;
Liste ** prochain_affichage;

// Retourne la position du path a explorer dans les arguements
int pathPositionInArgs(int argc, char * argv[], int pos,int *pos_path){
  if(pos<argc){
    if(strcmp(argv[pos],"-l")!=0&&strcmp(argv[pos],"-t")!=0&&strcmp(argv[pos],"-i")&&strcmp(argv[pos],"-p")){//si l'arguement est différent de -i -l
      if(*pos_path==-1) *pos_path=pos;
    }
  }
}

void getAbsolutePath(char *path){
  current_path=realpath(path,current_path);
  
  if(current_path[strlen(current_path)-1]!='/'){
    current_path = realloc(current_path,sizeof(char)*(strlen(current_path)+2));
    current_path[strlen(current_path)]='/';
  }
}

// Fonction principale
int main(int argc, char * argv[]){
  int i=0; //variable de travail

  int position_path = -1;
  char c; //caractere de retour
  extern char * optarg;//arguement
  opterr=0; //on masque les erreurs

  while ((c = getopt(argc , argv, "lt:ip:hT:")) != -1){
    switch (c) {
      case 'l': //Listing détaillé
        listing_long = 1;
        pathPositionInArgs(argc,argv,optind,&position_path);
        break;
      case 't':	//Retourne la liste de fichier contenant la chaine passé en paramètre
        motif = malloc(sizeof(char)*(strlen(optarg)+1));
        strcpy(motif,optarg);
        break;
      case 'i':	//Retourne les fichiers images
        recherche_image = 1;
        pathPositionInArgs(argc,argv,optind,&position_path);
	      if(load_libmagic()==-1){fprintf (stderr, "error dlopen libmagic.so\n");exit(1);}
        break;
      case 'p':	//Utilisation des threads
        nb_threads = atoi(optarg);
        break;
      case 'h':	//Affiche le fichier d'aide
        system("cat HELP");
        exit(0);
        break;
      case 'T':
        motif_T = malloc(sizeof(char)*(strlen(optarg)+1));
        strcpy(motif_T,optarg);
        if(pcre_init(motif_T)==-1){fprintf (stderr, "error compil pcre\n");exit(1);}
        break;  
      case '?':
        if (optopt=='p'|| optopt=='t' || optopt=='T') fprintf (stderr, "L'option -%c nécessite un arguement (voir ./cherche -h)\n", optopt);
        else fprintf (stderr, "Option [-%c] inconnue (voir ./cherche -h)\n", optopt);
	      exit(1);
        break;
    }
  }
  if(optind<=argc-1&&argc>1) position_path=optind;

  if(position_path!=-1){
    getAbsolutePath(argv[position_path]);
  }
  else{
    getAbsolutePath(".");
  }

  // Initialisations
  semaphore_listing = creation_semaphores(argv[0],1);
  semaphore_affichage = creation_semaphores(strcat(argv[0],"bim"),1);
  initialisation_semaphores(semaphore_listing, 1, 0);
  initialisation_semaphores(semaphore_affichage, 1, 0);

  pthread_mutex_init(&mutex_liste, NULL);
  pthread_mutex_init(&mutex_nb_analyses, NULL);
  pthread_mutex_init(&mutex_prochaine_recherche, NULL);
  pthread_mutex_init(&mutex_affichage, NULL);

  prochaine_recherche = &liste;
  prochain_affichage = &liste;
  threads = malloc(sizeof(pthread_t)*nb_threads);
  thread_affichage = malloc(sizeof(pthread_t));
  pthread_create(thread_affichage,NULL,afficher_resultats,NULL);
  for(i=0; i<nb_threads; i++)
  {
    pthread_create(&threads[i],NULL,chercher,NULL);
  }

  // Lancement du listing pour donner du travail aux threads
  listing(&liste, current_path, "", listing_long);
  
  // Attente de la fin de toutes les analyses faites par les threads
  int fin_des_threads = 0;
  while(!fin_des_threads)
  {
    if(nb_analyses == nb_insertions)
      fin_des_threads = 1;
  }

  // L'analyse est terminée, on tue les threads
  pthread_cancel(*thread_affichage);
  for(i=0; i<nb_threads; i++)
  {
    pthread_cancel(threads[i]);
  }

  free(threads);
  free(current_path);
  free(motif);

if (motif_T!=NULL)
{
  free(motif_T);
  pcre_end();
}
if(recherche_image==1)if(close_libmagic()==-1) fprintf (stderr, "error dlopen libmagic.so\n");

  // Destruction du sémaphore
  detruire_semaphores(semaphore_listing);
  detruire_semaphores(semaphore_affichage);

  pthread_mutex_destroy(&mutex_liste);
  pthread_mutex_destroy(&mutex_nb_analyses);
  pthread_mutex_destroy(&mutex_prochaine_recherche);
  pthread_mutex_destroy(&mutex_affichage);
  return 0;
}
