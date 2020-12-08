#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "utils/utils.h"
#include "Referee.h"

Application* Setup_Application(){
  Application* app = (Application*) malloc(sizeof(Application));

  app->referee.championshipDuration = 0;
  app->referee.waitingDuration = 0;
  
  memset(app->referee.gameDir, '\0', STRING_MEDIUM);
  app->referee.maxPlayers = 0;

  return app;
}

bool isValid_ChampionshipDuration(Application* app, int value){
  //App already registered a championship duration
  if(app->referee.championshipDuration > 0){
    printf("\n\tDuplicate parameter! Program will exit...");
    return false;
  }
  
  if(value < MIN_CHAMP_DURATION){
    printf("\n\tChampionship duration cannot be lower than %d! Program will exit...", MIN_CHAMP_DURATION);
    return false;
  }
  
  if(value > MAX_CHAMP_DURATION){
    printf("\n\tChampionship duration cannot be higher than %d! Program will exit...", MAX_CHAMP_DURATION);
    return false;
  }

  return true;
}

bool isValid_WaitingDuration(Application* app, int value){
  //App already registered a championship duration
  if(app->referee.waitingDuration > 0){
    printf("\n\tDuplicate parameter! Program will exit...");
    return false;
  }
  
  if(value < MIN_WAITING_DURATION){
    printf("\n\tWaiting duration cannot be lower than %d! Program will exit...", MIN_WAITING_DURATION);
    return false;
  }
  
  if(value > MAX_WAITING_DURATION){
    printf("\n\tWaiting duration cannot be higher than %d! Program will exit...", MAX_WAITING_DURATION);
    return false;
  }

  return true;
}

bool Setup_Variables(Application* app, int argc, char **argv){
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

    int intValue = atoi(value);
    switch(identifier[0]){
      case 'D':
        if(!isValid_ChampionshipDuration(app, intValue))
          return false;

        printf("\n\tChampionship duration: %d", intValue);
        app->referee.championshipDuration = intValue;
        break;
      case 'W':
        if(!isValid_WaitingDuration(app, intValue))
          return false;
          
        printf("\n\tWaiting time: %d", intValue);
        app->referee.waitingDuration = intValue;
        break;
      default:
        printf("\n\tUnknown paremeter! Program will exit...");
        return false;
    }
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

  if(maxplayer > MAX_MAXPLAYER)
    maxplayer = MAX_MAXPLAYER;
  if(maxplayer <= 1)
    maxplayer = DEFAULT_MAXPLAYER;

  printf("\n\nEnvironment variables:\n");
  printf("\t[GAMEDIR] - %s\n", gamedir);
  printf("\t[MAXPLAYER] - %d\n", maxplayer);

  DIR* dir = opendir(gamedir);
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    printf("\n\tDirectory does not exist... Create? (* = yes | n = no)");
    printf("\n\t-> ");
    
    char line[STRING_MEDIUM];
    scanf("%[^\n]", line);
    Utils_CleanStdin();
    
    if(strcmp(line, "n") != 0 && strcmp(line, "N") != 0){
      mkdir(gamedir, 0777);
      if (dir) {
        printf("\n\tDirectory has been created successfuly!");
        closedir(dir);
      }else{
        printf("\n\tUnexpected error on mkdir()! Program will exit...");
        return false;
      }
    }else{
      printf("\n\tProgram cannot run without an existing directory! Program will exit...");
      return false;
    }
  } else {
    printf("\n\tUnexpected error on opendir()! Program will exit...");
    return false;
  }

  strcpy(app->referee.gameDir, gamedir);
  app->referee.maxPlayers = maxplayer;
  /**TAG_TODO
   * Add environment variables into struct variables
   */
   return true;
}

void Print_Application(Application* app){
  printf("\nMyApplication");
  printf("\n\tChampionship duration: %d", app->referee.championshipDuration);
  printf("\n\tWaiting duration: %d", app->referee.waitingDuration);
  printf("\n\tGame directory: %s", app->referee.gameDir);
  printf("\n\tMax players: %d", app->referee.maxPlayers);
}

int main(int argc, char **argv){
  Application* app = Setup_Application();
  if(app == NULL){
    printf("\n\nSetup Application failed!\n\n");
  }

  if(!Setup_Variables(app, argc, argv)){
    printf("\n\nSetup Variables failed!\n\n");
    return false;
  }
  
  Print_Application(app);

  return false;
}