/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psftag.h"

#ifdef WIN32
#define strcasecmp _stricmp
#endif

/////////////////////////////////////////////////////////////////////////////

void titlefromfilename(char *filename) {
  char *t;
  // trim some beginning characters
  for(t = filename; t; t++) {
    if(!strchr(" _%0123456789-", *t)) break;
  }
  if(t != filename) { memmove(filename, t, strlen(t) + 1); }
  // eat after and including last dot
  t = strrchr(filename, '.');
  if(t) *t = 0;
  // replace some other stuff
  for(t = filename; *t; ) {
    if(!strncmp(t, "  ", 2)) {
      memmove(t, t + 1, strlen(t));
    } else if(!strncmp(t, "%20", 3)) {
      memmove(t, t + 2, strlen(t) - 1);
      *t = ' ';
    } else if(*t == '_') {
      *t = ' ';
    } else {
      t++;
    }
  }

}

/////////////////////////////////////////////////////////////////////////////

int parseoption(
  const char *a,
  void *psftag,
  const char *filename,
  int output
) {
  char *var = NULL;
  char *val = NULL;
  if(!strcmp(a, "tf")) {
    if(output) printf("title=[from filename]\n");
    if(!filename) return 0;
    var = malloc(6);
    if(!var) abort();
    strcpy(var, "title");
    val = malloc(strlen(filename) + 1);
    if(!val) abort();
    strcpy(val, filename);
    titlefromfilename(val);
  } else {
    const char *b = strchr(a, '=');
    if((!b) || (b == a)) {
      fprintf(stderr, "unknown option '-%s'\n", a);
      return 1;
    }
    var = malloc((b - a) + 1);
    if(!var) abort();
    memcpy(var, a, b - a);
    var[b - a] = 0;
    b++;
    val = malloc(strlen(b) + 1);
    if(!val) abort();
    strcpy(val, b);
  }
  if(output) printf("%s=%s\n", var, val);
  if(psftag) psftag_setvar(psftag, var, val);
  free(var);
  free(val);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
  int i;
  int opt, optstart;

  if(argc < 3) {
    fprintf(stderr, "PSFPoint v1.04 - Simple command-line PSF tagger\n");
    fprintf(stderr, "Written by Neill Corlett\n");
    fprintf(stderr, "1.04 modifications by ugetab\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [-tf] [-variable=value ...] psf-file(s)\n", argv[0]);
    return 1;
  }

  /*
  ** Look for variable=value things
  */
  for(i = 1; i < argc; i++) {
    if(argv[i][0] != '-') break;
    if(!strcasecmp(argv[i], "--")) { i++; break; }
    if(parseoption(argv[i] + 1, NULL, NULL, 0)) return 1;
  }
  if(i == 1) {
    fprintf(stderr, "no variables or options were given - nothing to do\n");
    return 1;
  }
  optstart = i;
  if(optstart >= argc) {
    fprintf(stderr, "no filenames were given - nothing to do\n");
    return 1;
  }

  /*
  ** Dump variable replacements
  */
  fprintf(stdout, "-----replacing variables-----\n");
  for(i = 1; i < optstart; i++) {
    if(argv[i][0] != '-') break;
    if(!strcasecmp(argv[i], "--")) { i++; break; }
    if(parseoption(argv[i] + 1, NULL, NULL, 1)) return 1;
  }
  fprintf(stdout, "-----------------------------\n");


  for(opt = optstart; opt < argc; opt++) {
    int r;
    void *psftag;
    const char *name = argv[opt];
    printf("%s: ", name); fflush(stdout);

    psftag = psftag_create();
    if(!psftag) abort();

    //
    // Okay if it fails - might be untagged
    //
    psftag_readfromfile(psftag, name);

    for(i = 1; i < optstart; i++) {
      if(argv[i][0] != '-') break;
      if(!strcasecmp(argv[i], "--")) { i++; break; }
      parseoption(argv[i] + 1, psftag, name, 0);
    }

    r = psftag_writetofile(psftag, name);

    if(r < 0) {
      printf("%s\n", psftag_getlasterror(psftag));
    } else {
      printf("ok\n");
    }

    psftag_delete(psftag);
  }

  printf("done\n");

  return 0;
}
