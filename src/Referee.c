#include "Referee.h"

#include "RService.h"
#include "utils/utils.h"

int main(int argc, char **argv) {
  Application *app = (Application *)malloc(sizeof(Application));
  if (app == NULL) {
    printf("\n\nSetup Application failed!\n\n");
  }

  if (!Setup_Application(app, argc, argv)) {
    printf("\n\nSetup Application failed!\n\n");
    return 1;
  }

  printf("\n\tNow waiting for thread to end");
  sleep(100);
  printf("\n\tThread has ended");

  Print_Application(app);

  free(app);
  return 0;
}