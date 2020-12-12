#include "PService.h"

#include "PThreads.h"
#include "Player.h"

bool Setup_Application(Application *app) {
  app->player.loggedIn = false;
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
  app->namedPipeHandles.fdQnARequest_Write = open(FIFO_R2P, O_WRONLY);
  if (app->namedPipeHandles.fdQnARequest_Write == -1) {
    printf("\nUnexpected error on open()! Program will exit...");
    printf("\n\tError: %d", errno);
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

bool Service_Login(Application *app, char *username) {
  TParam_SendQnARequest *param =
      (TParam_SendQnARequest *)malloc(sizeof(TParam_SendQnARequest));
  if (param == NULL) {
    return false;
  }

  param->app = app;
  param->request.requestType = QnART_LOGIN;
  strcpy(param->request.playerLoginRequest.username, username);

  pthread_t currThread;

  if (pthread_create(&currThread, NULL, &Thread_SendQnARequests,
                     (void *)param) != 0) {
    free(param);
    return false;
  };

  pthread_join(currThread, NULL);
  return true;
}

void Print_Application(Application *app) {
  printf("\nMyApplication");
  printf("\n\tUsername: %s", app->player.username);
  printf("\n\tQnARequest_Read FD: %d",
         app->namedPipeHandles.fdQnARequest_Write);
}