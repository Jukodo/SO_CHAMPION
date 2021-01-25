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
#pragma region Create - Player read from Referee
  // Create
  char fifoName_PlayerRead[STRING_LARGE];
  sprintf(fifoName_PlayerRead, "%s_%d", FIFO_REFEREE_TO_PLAYER, getpid());
  if (mkfifo(fifoName_PlayerRead, 0777) == -1) {
    // Only returns error if file does not exist after operation
    if (errno != EEXIST) {
      printf("[ERROR] Unexpected error on mkfifo()! Error: %d\n", errno);
      return false;
    }
  }

  printf(
      "[DEBUG] - Created a named pipe named %s... Objetive: Read from "
      "Referee\n",
      fifoName_PlayerRead);
#pragma endregion

#pragma region Create - Player write to Referee
  // Create
  char fifoName_PlayerWrite[STRING_LARGE];
  sprintf(fifoName_PlayerWrite, "%s_%d", FIFO_PLAYER_TO_REFEREE, getpid());
  if (mkfifo(fifoName_PlayerWrite, 0777) == -1) {
    // Only returns error if file does not exist after operation
    if (errno != EEXIST) {
      printf("[ERROR] Unexpected error on mkfifo()! Error: %d\n", errno);
      return false;
    }
  }

  printf(
      "[DEBUG] - Created a named pipe named %s... Objetive: Write to "
      "Referee\n",
      fifoName_PlayerWrite);
#pragma endregion

#pragma region Open - Player Login Write
  app->namedPipeHandles.fdComm_Entry = open(FIFO_REFEREE_ENTRY, O_WRONLY);
  if (app->namedPipeHandles.fdComm_Entry == -1) {
    printf("Unexpected error on open()! Program will exit...\n");
    printf("\tError: %d\n", errno);
    return false;
  }
#pragma endregion

  return true;
}

bool Setup_Threads(Application *app) {
  TParam_ReadFromReferee *param = malloc(sizeof(TParam_ReadFromReferee));
  if (param == NULL) {
    return false;
  }

  param->app = app;

  if (pthread_create(&app->threadHandles.hReadFromReferee, NULL,
                     &Thread_ReadFromReferee, (void *)param) != 0) {
    free(param);
    return false;
  };

  return true;
}

bool Service_Login(Application *app, char *username) {
  TParam_SendEntryRequest *param =
      (TParam_SendEntryRequest *)malloc(sizeof(TParam_SendEntryRequest));
  if (param == NULL) {
    return false;
  }

  param->app = app;
  strcpy(param->entryRequest.username, username);
  param->entryRequest.procId = getpid();

  pthread_t currThread;
  if (pthread_create(&currThread, NULL, &Thread_SendEntryRequest,
                     (void *)param) != 0) {
    free(param);
    return false;
  };

  return true;
}

void Service_OpenPrivateWrite(Application *app) {
  char fifoName_PlayerWrite[STRING_LARGE];
  sprintf(fifoName_PlayerWrite, "%s_%d", FIFO_PLAYER_TO_REFEREE, getpid());

  app->namedPipeHandles.fdComm_Write = open(fifoName_PlayerWrite, O_WRONLY);
  if (app->namedPipeHandles.fdComm_Write == -1) {
    printf("[ERROR] - Unexpected error on open()! Error: %d\n", errno);
    return;
  }

  printf("[DEBUG] - I have opened my named pipe %s for writting!\n",
         fifoName_PlayerWrite);

  return;
}

bool Service_Input(Application *app, char *command) {
  TParam_WriteToReferee *param = malloc(sizeof(TParam_WriteToReferee));
  if (param == NULL) {
    return false;
  }

  param->app = app;
  param->tossComm.tossType = TCRT_PLAYER_INPUT;
  strcpy(param->tossComm.playerInput.command, command);
  param->tossComm.playerInput.procId = getpid();

  pthread_t currThread;
  if (pthread_create(&currThread, NULL, &Thread_WriteToReferee,
                     (void *)param) != 0) {
    free(param);
    return false;
  };

  return true;
}

void Print_Application(Application *app) {
  printf("\nMyApplication\n");
  printf("\tUsername: %s\n", app->player.username);
  printf("\tQnARequest_Read FD: %d\n", app->namedPipeHandles.fdComm_Entry);
}