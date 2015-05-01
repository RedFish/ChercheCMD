#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <magic.h>
#include <pcre.h>
#define OVECCOUNT 30

void *lib;
pcre *re;

void (*magic_close_)(magic_t);
magic_t (*magic_open_)(int);
int (*magic_load_)(magic_t, const char *);
const char *(*magic_error_)(magic_t);
const char *(*magic_file_)(magic_t, const char *);

int isTextInFile(char * string, char * path);
int pcre_init(char *pattern);
int pcre_end();
int isMatch(char * pattern, char * path);
int load_libmagic();
int close_libmagic();
int isPicture(char * path);
