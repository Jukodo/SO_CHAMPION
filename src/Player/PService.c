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

  if (DEBUG) {
    printf(
        "[DEBUG] - Created a named pipe named %s... Objetive: Read from "
        "Referee\n",
        fifoName_PlayerRead);
  }
#pragma endregion

#pragma region Create - Player write to Referee
  // Create
  char fifoName_PlayerWrite[STRING_LARGE];
  sprintf(fifoName_PlayerWrite, "%s_%d", FIFO_PLAYER_TO_REFEREE, getpid());
  if (mkfifo(fifoName_PlayerWrite, 0777) == -1) {
    // Only returns error if file does not exist after operation
    if (errno != EEXIST) {
      printf("[ERROR] - Unexpected error on mkfifo()! Error: %d\n", errno);
      return false;
    }
  }

  if (!Service_OpenPrivateWrite(app)) {
    printf("[ERROR] - Unexpected error waiting for named pipe open!\n");
    return false;
  }
  if (DEBUG) {
    printf(
        "[DEBUG] - Created a named pipe named %s... Objetive: Write to "
        "Referee\n",
        fifoName_PlayerWrite);
  }
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
  EntryRequest entryRequest;

  strcpy(entryRequest.username, username);
  entryRequest.procId = getpid();

  if (write(app->namedPipeHandles.fdComm_Entry,  // File Descriptor
            &entryRequest,                       // Value
            sizeof(EntryRequest)                 // Size of written value
            ) == -1) {
    printf(
        "[ERROR] - Could not communicate with Referee's entry named pipe! "
        "Error: "
        "%d\n",
        errno);

    return false;
  }

  return true;
}

/**Create a thread that waits until named pipe has been open on the Referee side
 *
 * Why: When the 2nd (Defined minimum players to start championship) player
 * joins the championship will start right away, however, this named pipe will
 * not be open at the moment, and the player will not receive the Championship
 * Status, this way, it will be instantly
 */
bool Service_OpenPrivateWrite(Application *app) {
  TParam_OpenPrivateWrite *param = malloc(sizeof(TParam_OpenPrivateWrite));
  if (param == NULL) {
    return false;
  }

  param->app = app;

  pthread_t currThread;
  if (pthread_create(&currThread, NULL, &Thread_OpenPrivateWrite,
                     (void *)param) != 0) {
    free(param);
    return false;
  };

  return true;
}

bool Service_Input(Application *app, char *command) {
  TossComm *tossComm = malloc(sizeof(TossComm));
  if (tossComm == NULL) {
    printf("[ERROR] - Malloc failed to allocate for tossComm! Error: %d\n",
           errno);
    return false;
  }

  tossComm->tossType = TCRT_PLAYER_INPUT;
  strcpy(tossComm->playerInput.command, command);
  tossComm->playerInput.procId = getpid();

  Service_SendTossComm(app, tossComm);

  return true;
}

/**Send a TossComm to referee from the player privated named pipe
 * Create a TossComm allocated in memory (malloc)
 * Fill up the toss comm and inform which procId should it send to
 */
void Service_SendTossComm(Application *app, TossComm *tossComm) {
  // Send entry request
  if (write(app->namedPipeHandles.fdComm_Write,  // File Descriptor
            tossComm,                            // Value
            sizeof(TossComm)                     // Size of written value
            ) == -1) {
    printf(
        "[ERROR] - Could not write to Referee's named pipe! Error: "
        "%d\n",
        errno);
    return;
  }

  free(tossComm);
}

void Print_Application(Application *app) {
  printf("\nMyApplication\n");
  printf("\tUsername: %s\n", app->player.username);
  printf("\tQnARequest_Read FD: %d\n", app->namedPipeHandles.fdComm_Entry);
}