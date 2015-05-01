#include "search.h"

int isTextInFile(char * string, char * path){
	FILE *file = NULL;
	int size = strlen(string)+1;
	char *string_tmp;
	const int buf_size=1000;

	string_tmp = malloc(sizeof(char)*buf_size);
	file = fopen(path,"r");

	if(file!=NULL){
		while (fgets(string_tmp, buf_size, file)!=NULL){
			if(strstr(string_tmp,string)!=NULL) {
				free(string_tmp);
				fclose(file);
				return 1;
			}
		}
		free(string_tmp);
		fclose(file);
	}
	else{
		fprintf(stderr, "error opening %s\n", path);
		free(string_tmp);
		return -1;
	}

	return 0;
}

int pcre_init(char *pattern){
	const char *error;
	int erroffset;
	re = pcre_compile(
		pattern,              /* the pattern */
 		0,                    /* default options */
 		&error,               /* for error message */
 		&erroffset,           /* for error offset */
 		NULL
 	);                /* use default character tables */

	/* Compilation failed: print the error message and exit */
	if (re == NULL){
		return -1;
	}

	return 0;
}

int pcre_end(){
	pcre_free(re);/* Release memory used for the compiled pattern */
}

int isMatch(char * pattern, char * path){
	FILE *file = NULL;
	char *string_tmp;
	const int buf_size=1000;
	int rc;
	int result=0;
	int ovector[OVECCOUNT];

	string_tmp = malloc(sizeof(char)*buf_size);
	file = fopen(path,"r");

	if(file!=NULL){
		while (fgets(string_tmp, buf_size, file)!=NULL){
			rc = pcre_exec(
  				re,                   /* the compiled pattern */
  				NULL,                 /* no extra data - we didn't study the pattern */
  				string_tmp,           /* the subject string */
  				strlen(string_tmp),   /* the length of the subject */
  				0,                    /* start at offset 0 in the subject */
  				0,                    /* default options */
  				ovector,              /* output vector for substring information */
  				OVECCOUNT			  /* number of elements in the output vector */
  			);           
			if(rc>0) {
				free(string_tmp);
				fclose(file);
				return 1;
			}
		}
		free(string_tmp);
		fclose(file);
	}
	else{
		fprintf(stderr, "error opening %s\n", path);
		free(string_tmp);
		return -1;
	}

	return 0;
}

int load_libmagic(){
    if (!(lib = dlopen("/usr/lib/libmagic.so", RTLD_NOW))) return -1;
	if (!(magic_close_ = (void (*)(magic_t)) dlsym(lib, "magic_close"))) return -1;
	if (!(magic_open_ = (magic_t (*)(int)) dlsym(lib, "magic_open"))) return -1;
	if (!(magic_load_ = (int (*)(magic_t,const char *)) dlsym(lib, "magic_load"))) return -1;
	if (!(magic_error_ = (const char * (*)(magic_t)) dlsym(lib, "magic_error"))) return -1;
	if (!(magic_file_ = (const char * (*)(magic_t, const char *)) dlsym(lib, "magic_file"))) return -1;
	return 0;
}

int close_libmagic(){
	dlclose(lib);
}

int isPicture(char * path){


	int result = 0;
	const char *mine_type;
	magic_t magic_cookie = magic_open_(MAGIC_MIME);
	if (magic_cookie != NULL) {
		if (magic_load_(magic_cookie, NULL) == 0) {
			mine_type = magic_file_(magic_cookie, path);
			//printf("%s\n", mine_type);
			if(strncmp(mine_type,"image",5)==0) result = 1;
			magic_close_(magic_cookie);
		}
		else{
			fprintf(stderr, "error loading magic database - %s\n", magic_error_(magic_cookie));
			magic_close_(magic_cookie);
			return -1;
		}
	}
	else{
		fprintf(stderr, "error initializing magic library\n");
		return -1;
	}

	return result;
}
