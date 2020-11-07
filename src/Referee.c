#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/utils.h"

#define DEFAULT_GAMEDIR "~/Documents/SO/SO_CHAMPION/Games/"
#define DEFAULT_MAXPLAYER 10

bool Setup(int argc, char **argv){
  printf("\nReferee is starting...");
  printf("\n\nParameters received:");
  
  //argv[0] always is the executable path (skipped on the cycle)
  for(int i = 1; i < argc; i++){
    if(argc != 3){
      printf("Parameters are invalid!\n\tPlease follow the rule [(char) ID]_[(int) VALUE]\n\tAnd provide D_**** and W_****");
      return false;
    }

    char* identifier = strtok(argv[i], "_");
    char* value = strtok(NULL, "_");

    if(strlen(identifier) != 1 || !Utils_StringIsNumber(value)){
      printf("Parameters are invalid!\n\tPlease follow the rule [(char) ID]_[(int) VALUE]\n\tAnd provide D_**** and W_****");
      return false;
    }

    switch(identifier[0]){
      case 'D':
        printf("\n\tChampionship duration: %d", atoi(value));
        break;
      case 'W':
        printf("\n\tWaiting time: %d", atoi(value));
        break;
      default:
        printf("\n\tUnknown paremeter! Program will exit...");
        return false;
    }

    /**TAG_TODO
     * Add parameters into the struct variables
     * Check if variable already exists (denies same parameter)
     */
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

  printf("\n\nEnvironment variables:\n");
  printf("\t[GAMEDIR] - %s\n", gamedir);
  printf("\t[MAXPLAYER] - %d\n", maxplayer);
  /**TAG_TODO
   * Add environment variables into struct variables
   */
   return true;
}

int main(int argc, char **argv){
  if(!Setup(argc, argv)){
    printf("\n\nSetup failed\n\n");
    return false;
  }
  

  return false;
}