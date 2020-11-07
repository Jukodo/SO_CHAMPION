#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/utils.h"

#define DEFAULT_GAMEDIR "~/Documents/SO/SO_CHAMPION/Games/"
#define DEFAULT_MAXPLAYER 10

void printVariables(int argc, char **argv){
  printf("Argument variables:\n");
  //argv[0] is already the executable path
  for(int i = 0; i < argc; i++){
    printf("\t[%d] - %s\n", i, argv[i]);
  }

  char gamedir[STRING_MEDIUM];
  strcpy(gamedir, (Utils_StringIsEmpty(getenv("GAMEDIR"))) ?
    DEFAULT_GAMEDIR :
    getenv("GAMEDIR"));

  int maxplayer = (Utils_StringIsEmpty(getenv("MAXPLAYER"))) ? 
    DEFAULT_MAXPLAYER : 
    Utils_StringIsNumber(getenv("MAXPLAYER")) ? 
      atoi(getenv("MAXPLAYER")) : 
      DEFAULT_MAXPLAYER;

  printf("\nEnvironment variables:\n");
  printf("\t[GAMEDIR] - %s\n", gamedir);
  printf("\t[GAMEDIR] - %d\n", maxplayer);
}

int main(int argc, char **argv){
  printVariables(argc, argv);

  printf(getenv("GAMEDIR"));

  return false;
}