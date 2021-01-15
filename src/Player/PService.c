#include "PService.h"

#include "PThreads.h"
#include "Player.h"

bool Setup_Application(Application *app) {
  app->player.loggedIn = false;
  memset(app->player.username, '\0', STRING_MEDIUM);

  pthread_mutex_t *loggedInMutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(loggedInMutex, NULL);
  app->mutexHandles.hMutex_LoggedIn = loggedInMutex;
  pthread_mutex_lock(app->mutexHandles.hMutex_LoggedIn);

  if (!Setup_NamedPipes(app)) {
    printf("\nSetup_NamedPipes failed!\n");
    return false;
  }
  if (!Setup_Threads(app)) {
    printf("\nSetup_Threads failed!\n");
    return false;
  }

  return true;
}

bool Setup_NamedPipes(Application *app) {
  // Create
  char playerFifoName[STRING_LARGE];
  sprintf(playerFifoName, "%s_%d", FIFO_PLAYER, getpid());
  if (mkfifo(playerFifoName, 0777) == -1) {
    // Only returns error if file does not exist after operation
    if (errno != EEXIST) {
      printf("\tUnexpected error on mkfifo()! Program will exit...\n");
      printf("\t\tError: %d\n", errno);
      return false;
    }
  }

  app->namedPipeHandles.fdComm_Read = open(playerFifoName, O_RDWR);
  if (app->namedPipeHandles.fdComm_Read == -1) {
    printf("\tUnexpected error on open()!\n");
    printf("\t\tError: %d\n", errno);
    return false;
  }

  app->namedPipeHandles.fdQnARequest_Write = open(FIFO_REFEREE, O_WRONLY);
  if (app->namedPipeHandles.fdQnARequest_Write == -1) {
    printf("Unexpected error on open()! Program will exit...\n");
    printf("\tError: %d\n", errno);
    return false;
  }

  return true;
}

bool Setup_Threads(Application *app) {
  TParam_ReceiveComms *param = malloc(sizeof(TParam_ReceiveComms));
  if (param == NULL) {
    return false;
  }

  param->app = app;

  if (pthread_create(&app->threadHandles.hReceiveComms, NULL,
                     &Thread_ReceiveComms, (void *)param) != 0) {
    free(param);
    return false;
  };

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
  param->request.playerLoginRequest.procId = getpid();

  pthread_t currThread;

  if (pthread_create(&currThread, NULL, &Thread_SendQnARequests,
                     (void *)param) != 0) {
    free(param);
    return false;
  };

  return true;
}

bool Service_Input(Application *app, char *command) {
  TParam_SendQnARequest *param =
      (TParam_SendQnARequest *)malloc(sizeof(TParam_SendQnARequest));
  if (param == NULL) {
    return false;
  }

  param->app = app;
  param->request.requestType = QnART_INPUT;
  strcpy(param->request.playerInputRequest.command, command);
  param->request.playerInputRequest.procId = getpid();

  pthread_t currThread;
  if (pthread_create(&currThread, NULL, &Thread_SendQnARequests,
                     (void *)param) != 0) {
    free(param);
    return false;
  };

  return true;
}

void Print_Application(Application *app) {
  printf("\nMyApplication\n");
  printf("\tUsername: %s\n", app->player.username);
  printf("\tQnARequest_Read FD: %d\n",
         app->namedPipeHandles.fdQnARequest_Write);
}