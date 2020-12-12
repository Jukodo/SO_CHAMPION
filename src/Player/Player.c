#include "Player.h"

#include "PService.h"

int main(int argc, char** argv) {
  Application* app = malloc(sizeof(Application));
  if (app == NULL) {
    printf("\n\nApplication malloc() failed!!\n\n");
  }

  char retryOpt[2];
  bool flagOfflineCentral = true;
  do {
    memset(retryOpt, '\0', 2);
    flagOfflineCentral = !Setup_Application(app);
    if (flagOfflineCentral) {
      printf("\n\nCentral is offline... Do you want to try again? (Y/n)");
      printf("\n\t-> ");
      scanf((" %[^\n]"), retryOpt);
      Utils_CleanStdin();

      if (_tolower(retryOpt[0]) == 'n' || retryOpt[0] == 'n') {
        return EXIT_FAILURE;
      }
    }
  } while (flagOfflineCentral);
  printf("\n\nConnection to central successful!");

  char username[STRING_MEDIUM];
  do {
    memset(username, '\0', STRING_MEDIUM);

    printf("\nPlease provide your username:");
    printf("\n\t-> ");
    scanf(" %[^\n]", username);
    Utils_CleanStdin();

    strcpy(app->player.username, username);

    Service_Login(app, username);
  } while (!app->player.loggedIn);

  getchar();

  Print_Application(app);

  return EXIT_SUCCESS;
}