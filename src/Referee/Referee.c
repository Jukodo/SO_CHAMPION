#include "Referee.h"

#include "RService.h"

void handle_sigusr1(int sig) {
  printf("\n\tReceived signal: %d...\n", sig);
  printf("\tClosing App and deleting FIFOs!\n");
  unlink(FIFO_REFEREE);
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  struct sigaction sa = {0};  //{0} - The struct starts with 0's
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = &handle_sigusr1;
  sigaction(SIGINT, &sa, NULL);

  Application *app = (Application *)malloc(sizeof(Application));
  if (app == NULL) {
    printf("\nApplication malloc() failed!\n");
  }

  if (!Setup_Application(app, argc, argv)) {
    printf("\nSetup Application failed!\n");
    return 1;
  }

  pthread_join(app->threadHandles.hQnARequests, NULL);

  Print_Application(app);

  unlink(FIFO_PLAYER);
  free(app);
  return 0;
}