#include "Player.h"

#include "PService.h"

int main(int argc, char** argv) {
  Application* app = malloc(sizeof(Application));
  if (app == NULL) {
    printf("\n\nApplication malloc() failed!!\n\n");
  }

  char username[STRING_MEDIUM];
  printf("\nPlease provide your username:");
  printf("\n\t-> ");
  scanf("%[^\n]", username);
  Utils_CleanStdin();

  strcpy(app->player.username, username);

  if (!Setup_Application(app)) {
    printf("\n\nSetup Application failed!\n\n");
    return EXIT_FAILURE;
  }

  Print_Application(app);

  return EXIT_SUCCESS;
}