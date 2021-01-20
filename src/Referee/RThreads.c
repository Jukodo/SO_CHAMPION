#include "RThreads.h"

#include "RService.h"
#include "Referee.h"

void* Thread_ReceiveQnARequests(void* _param) {
  TParam_ReceiveQnARequest* param = (TParam_ReceiveQnARequest*)_param;

  QnARequest request;

  int fdQnARequest_Read;
  bool closed = true;
  while (1) {
    if (closed) {
      fdQnARequest_Read = open(FIFO_REFEREE, O_RDONLY);

      if (fdQnARequest_Read == -1) {
        free(param);
        return (void*)EXIT_FAILURE;
      }

      closed = false;
    }
    // Receive Question
    int readBytes = read(fdQnARequest_Read, &request, sizeof(QnARequest));
    if (readBytes != sizeof(QnARequest)) {
      printf("[WARNING] - No communication open, restarting comms\n");

      close(fdQnARequest_Read);
      closed = true;
      continue;
    }

    // Send Answer
    TossComm tossComm;
    int playerIndex;
    int failedPlayer_fdComm_Write = -1;
    switch (request.requestType) {
      case QnART_LOGIN:
        tossComm.playerLoginResponse.playerLoginResponseType =
            Service_PlayerLogin(param->app, request.playerLoginRequest.procId,
                                request.playerLoginRequest.username);
        tossComm.tossType = TCRT_LOGIN_RESP;

        if (tossComm.playerLoginResponse.playerLoginResponseType !=
            PLR_SUCCESS) {
          char playerFifoName[STRING_LARGE];
          sprintf(playerFifoName, "%s_%d", FIFO_PLAYER,
                  request.playerLoginRequest.procId);
          failedPlayer_fdComm_Write = open(playerFifoName, O_RDWR);
          if (failedPlayer_fdComm_Write == -1) {
            printf("\t[ERROR] Unexpected error on open()!\n\t\tError: %d\n",
                   errno);
            continue;
          }
        }
        break;
      case QnART_INPUT: {
        PlayerInputResponse resp =
            Service_PlayerInput(param->app, request.playerInputRequest.procId,
                                request.playerInputRequest.command);
        if (resp.playerInputResponseType != PIR_INPUT &&
            resp.playerInputResponseType != PIR_INVALID) {
          tossComm.tossType = TCRT_INPUT_RESP;
          tossComm.playerInputResponse = resp;
        } else {
          continue;
        }
      } break;
      default:
        continue;
    }

    playerIndex =
        getPlayerIndexByProcId(param->app, request.playerLoginRequest.procId);
    sem_wait(&param->app->playerList[playerIndex].semNamedPipe);
    int writtenBytes =
        write(failedPlayer_fdComm_Write == -1
                  ? param->app->playerList[playerIndex].fdComm_Write
                  : failedPlayer_fdComm_Write,
              &tossComm, sizeof(TossComm));
    sem_post(&param->app->playerList[playerIndex].semNamedPipe);

    // Reset temp named pipe
    if (failedPlayer_fdComm_Write != -1) {
      close(failedPlayer_fdComm_Write);
      failedPlayer_fdComm_Write = -1;
    }

    if (writtenBytes != sizeof(TossComm)) {
      printf("TossComm failed\n");
    }
  }

  close(fdQnARequest_Read);
  free(param);
  return (void*)EXIT_SUCCESS;
}

void* Thread_ReadFromGame(void* _param) {
  TParam_ReadFromGame* param = (TParam_ReadFromGame*)_param;

  char buffer[STRING_LARGE];
  int readBytes;
  Player myPlayer = param->app->playerList[param->myPlayerIndex];
  TossComm tossComm;
  tossComm.tossType = TCRT_GAME_OUTPUT;
  do {
    readBytes = read(myPlayer.gameProc.fdReadFromGame,
                     tossComm.gameOutput.output, sizeof(tossComm.gameOutput));

    sem_wait(&param->app->playerList[param->myPlayerIndex].semNamedPipe);

    int writtenBytes =
        write(param->app->playerList[param->myPlayerIndex].fdComm_Write,
              &tossComm, sizeof(TossComm));

    sem_post(&param->app->playerList[param->myPlayerIndex].semNamedPipe);

    memset(buffer, '\0', STRING_LARGE);
  } while (readBytes >= 0);

  close(myPlayer.gameProc.fdReadFromGame);
  free(param);
  return (void*)EXIT_SUCCESS;
}

void* Thread_ChampionshipFlow(void* _param) {
  printf("[INFO] - Championship flow thread has started!\n");
  TParam_ChampionshipFlow* param = (TParam_ChampionshipFlow*)_param;

  bool canStart;
  struct timespec ts;
  while (true) {
    canStart = false;
    do {
      param->app->referee.isChampionshipClosed = false;
      printf("[INFO] - Championship is ready to start!\n");
      // Only unlocked when at least 2 players have joined
      pthread_mutex_lock(&param->app->mutStartChampionship);

      // Start a timer to wait for more players
      if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        printf("[ERROR] - Clock getTime failed! Error: %d\n", perror);
        return (void*)EXIT_FAILURE;
      }
      ts.tv_sec += param->app->referee.waitingDuration;

      printf("[INFO] - Waiting for players for %d seconds\n",
             param->app->referee.waitingDuration);
      pthread_mutex_timedlock(&param->app->mutCountdown, &ts);
      param->app->referee.isChampionshipClosed = true;
      printf("[INFO] - Lobby has closed! Currently active players: %d\n",
             getQuantityPlayers(param->app));

      // After the timer ends, check if at least 2 players are logged in, if not
      // restart championship to initial state
      canStart = getQuantityPlayers(param->app) >= DEFAULT_MINPLAYERS_START;

      if (!canStart) {
        Service_BroadcastChampionshipState(param->app, 3);
      }
    } while (!canStart);

    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
      printf("[ERROR] - Clock getTime failed! Error: %d\n", perror);
      return (void*)EXIT_FAILURE;
    }
    ts.tv_sec += param->app->referee.championshipDuration;

    printf("[INFO] - Championship has started! Ending in %d seconds...\n",
           param->app->referee.championshipDuration);
    pthread_mutex_timedlock(&param->app->mutCountdown, &ts);
    printf("[INFO] - Championship ended!\n");
  }

  free(param);
  return (void*)EXIT_SUCCESS;
}