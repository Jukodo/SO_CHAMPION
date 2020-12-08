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

  app->availableGames.gameList = NULL;
  app->availableGames.quantityGames = 0;
  
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
      DIR* dir = opendir(gamedir);
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

bool Setup_AvailableGames(Application* app){
  DIR* gamedir = opendir(app->referee.gameDir);
  struct dirent *dir;
  if (gamedir) {
    while ((dir = readdir(gamedir)) != NULL) {
      if(dir->d_type == DT_REG){ //Is a file (maybe executable)
        /**TAG_TODO
         * Check if file is executable
         */
        printf("\n\t%s is executable", dir->d_name);

        app->availableGames.quantityGames++;
        if(app->availableGames.gameList == NULL){
          app->availableGames.gameList = calloc(app->availableGames.quantityGames, sizeof(Game));
          if(app->availableGames.gameList == NULL){
            printf("Malloc failed");
            return false;
          }
        }else{
          Game* newGameList = realloc(app->availableGames.gameList, app->availableGames.quantityGames * sizeof(Game));
          if(newGameList == NULL){
            printf("Realloc failed");
            return false;
          }else{
            app->availableGames.gameList = newGameList;
          }
        }
        strcpy(app->availableGames.gameList[app->availableGames.quantityGames-1].fileName, dir->d_name);
      }
    }

    closedir(gamedir);
  }else{
    printf("\n\tUnexpected error on mkdir()! Program will exit...");
    return false;
  }

  return true;
}

void Print_Application(Application* app){
  printf("\nMyApplication");
  printf("\n\tChampionship duration: %d", app->referee.championshipDuration);
  printf("\n\tWaiting duration: %d", app->referee.waitingDuration);
  printf("\n\tGame directory: %s", app->referee.gameDir);
  printf("\n\tMax players: %d", app->referee.maxPlayers);
  printf("\n\tAvailable Games: %d", app->availableGames.quantityGames);
  for(int i = 0; i < app->availableGames.quantityGames; i++){
    printf("\n\t\t[%d] - %s", i+1, app->availableGames.gameList[i].fileName);
  }
}

int main(int argc, char **argv){
  Application* app = Setup_Application();
  if(app == NULL){
    printf("\n\nSetup Application failed!\n\n");
  }

  if(!Setup_Variables(app, argc, argv)){
    printf("\n\nSetup Variables failed!\n\n");
    return 1;
  }

  if(!Setup_AvailableGames(app)){
    printf("\n\nSetup Available Games failed!\n\n");
    return 1;
  }
  
  Print_Application(app);

  return 0;
}