#include "Player.h"

#include "PService.h"

int main(int argc, char** argv) {
  Application* app = malloc(sizeof(Application));
  if (app == NULL) {
    printf("\n\nApplication malloc() failed!!\n\n");
  }

#pragma region Connect to central
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
#pragma endregion
#pragma region Provide Username
  char username[STRING_LARGE];
  do {
    memset(username, '\0', STRING_LARGE);

    printf("\nPlease provide your username:\n");
    printf("\t-> ");
    scanf(" %[^\n]", username);
    Utils_CleanStdin();

    strcpy(app->player.username, username);

    Service_Login(app, username);
    pthread_mutex_lock(app->mutexHandles.hMutex_LoggedIn);
  } while (!app->player.loggedIn);

  printf("\nWelcome to the championship %s!\n", username);
  printf("\tWrite any command below:\n");
#pragma endregion
#pragma region Handle stdin
  char command[STRING_LARGE];
  bool flagContinue = true;
  do {
    memset(command, '\0', STRING_LARGE);

    printf("\t-> ");
    scanf(" %[^\n]", command);
    Utils_CleanStdin();

    Service_Input(app, command);
  } while (flagContinue);
#pragma endregion

  getchar();

  Print_Application(app);

  pthread_mutex_destroy(app->mutexHandles.hMutex_LoggedIn);
  return EXIT_SUCCESS;
}