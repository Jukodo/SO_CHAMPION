#include "RThreads.h"

#include "RService.h"
#include "Referee.h"

void* Thread_ReceiveQnARequests(void* _param) {
  TParam_ReceiveQnARequest* param = (TParam_ReceiveQnARequest*)_param;

  int fdQnARequest_Read = open(FIFO_REFEREE, O_RDONLY);

  if (fdQnARequest_Read == -1) {
    free(param);
    return (void*)EXIT_FAILURE;
  }

  QnARequest request;
  while (1) {
    sleep(2);
    // Receive Question
    int readBytes = read(fdQnARequest_Read, &request, sizeof(QnARequest));
    // printf("\n\n\tB: %d\n", sizeof(QnARequest));
    // printf("\n\n\tRead B: %d\n", readBytes);
    if (readBytes != sizeof(QnARequest)) {
      printf("\n\tReceived something but cannot read correctly");
      printf("\n\tError: %d", errno);
      continue;
    }

    // Send Answer
    TossComm tossComm;
    int playerIndex;
    switch (request.requestType) {
      case QnART_LOGIN:
        if (Service_PlayerLogin(param->app, request.playerLoginRequest.procId,
                                request.playerLoginRequest.username)) {
          playerIndex =
              getPlayerIndex(param->app, request.playerLoginRequest.procId);

          if (playerIndex == -1) {
            continue;
          }

          tossComm.tossType = TCRT_LOGIN_RESP;
          tossComm.playerLoginResponse.playerLoginResponseType = PLR_SUCCESS;
        } else {
          continue;
        }
        break;
      case QnART_INPUT: {
        PlayerInputResponse resp =
            Service_PlayerInput(param->app, request.playerInputRequest.procId,
                                request.playerInputRequest.command);
        if (resp.playerInputResponseType != PIR_INPUT) {
          tossComm.tossType = TCRT_INPUT_RESP;
          tossComm.playerInputResponse = resp;
        } else {
          continue;
        }
      } break;
      default:
        continue;
    }

    int writtenBytes = write(param->app->playerList[playerIndex].fdComm_Write,
                             &tossComm, sizeof(TossComm));

    if (writtenBytes != sizeof(TossComm)) {
      printf("TossComm failed\n");
    }
  }

  close(fdQnARequest_Read);
  free(param);
  return (void*)EXIT_SUCCESS;
}