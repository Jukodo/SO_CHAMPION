#include "PService.h"

#include "Player.h"

bool Setup_Application(Application *app) {
  memset(app->player.username, '\0', STRING_MEDIUM);

  if (!Setup_NamedPipes(app)) {
    printf("\n\nSetup_NamedPipes failed!");
    return false;
  }
  if (!Setup_Threads(app)) {
    printf("\n\nSetup_Threads failed!");
    return false;
  }

  return true;
}

bool Setup_NamedPipes(Application *app) {
  /**TAG_LOOKAT
   * Is this needed?
   * Virtual Client can be useful to keep the thread not blocked, but does it
   * matter?
   */
  app->namedPipeHandles.fdQnARequest_Read = open(FIFO_R2P, O_RDONLY);
  if (app->namedPipeHandles.fdQnARequest_Read == -1) {
    printf("\n\tUnexpected error on open()! Program will exit...");
    printf("\n\t\tError: %d", errno);
    return false;
  }

  return true;
}

bool Setup_Threads(Application *app) {
  /**TAG_TODO
   * Create and launch thread
   */

  return true;
}

void Print_Application(Application *app) {
  printf("\nMyApplication");
  printf("\n\tUsername: %s", app->player.username);
  printf("\n\tQnARequest_Read FD: %d", app->namedPipeHandles.fdQnARequest_Read);
}