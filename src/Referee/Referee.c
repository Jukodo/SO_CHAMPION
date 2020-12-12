#include "Referee.h"

#include "RService.h"

void handle_sigusr1(int sig) {
  printf("\n\n\tReceived signal: %d...", sig);
  printf("\n\tClosing App and deleting FIFOs!\n\n");
  unlink(FIFO_R2P);
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  struct sigaction sa = {0};  //{0} - The struct starts with 0's
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = &handle_sigusr1;
  sigaction(SIGINT, &sa, NULL);

  Application *app = (Application *)malloc(sizeof(Application));
  if (app == NULL) {
    printf("\n\nApplication malloc() failed!!\n\n");
  }

  if (!Setup_Application(app, argc, argv)) {
    printf("\n\nSetup Application failed!\n\n");
    return 1;
  }

  printf("\n\tNow waiting for thread to end");
  pthread_join(app->threadHandles.hQnARequests, NULL);
  printf("\n\tThread has ended");

  Print_Application(app);

  unlink(FIFO_R2P);
  free(app);
  return 0;
}