#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/utils.h"
#include "Player.h"

Application* Setup_Application(){
  Application* app = (Application*) malloc(sizeof(Application));

  memset(app->player.username, '\0', STRING_MEDIUM);

  return app;
}

void Print_Application(Application* app){
  printf("\nMyApplication");
  printf("\n\tUsername: %s", app->player.username);
}

int main(int argc, char **argv){
  Application* app = Setup_Application();
  if(app == NULL){
    printf("\n\nSetup Application failed!\n\n");
  }

  char username[STRING_MEDIUM];
  printf("\nPlease provide your username:");
  printf("\n\t-> ");
  scanf("%[^\n]", username);
  Utils_CleanStdin();

  strcpy(app->player.username, username);

  Print_Application(app);

  return false;
}