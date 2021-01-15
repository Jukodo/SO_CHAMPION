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
    // printf("\n\n\tB: %d\n", sizeof(QnARequest));
    // printf("\n\n\tRead B: %d\n", readBytes);
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
            return PLR_INVALID_UNDEFINED;
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