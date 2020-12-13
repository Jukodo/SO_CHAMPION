#include "PThreads.h"

#include "PService.h"
#include "Player.h"

void* Thread_SendQnARequests(void* _param) {
  TParam_SendQnARequest* param = (TParam_SendQnARequest*)_param;

  // Send Question
  if (write(param->app->namedPipeHandles.fdQnARequest_Write,  // File Descriptor
            &param->request,                                  // Value
            sizeof(QnARequest)  // Size of written value
            ) == -1) {
    printf("\tCould not communicate with Referee's named pipe!\n");
    printf("\tError: %d\n", errno);
    return (void*)EXIT_FAILURE;
  }

  free(param);
  return (void*)EXIT_SUCCESS;
}

void* Thread_ReceiveComms(void* _param) {
  TParam_ReceiveComms* param = (TParam_ReceiveComms*)_param;

  int readBytes;
  TossComm receivedTossComm;

  while (1) {
    readBytes = read(param->app->namedPipeHandles.fdComm_Read,
                     &receivedTossComm, sizeof(TossComm));
    if (readBytes != sizeof(TossComm)) {
      continue;
    }

    switch (receivedTossComm.tossType) {
      case TCRT_LOGIN_RESP:
        param->app->player.loggedIn = true;
        pthread_mutex_unlock(param->app->mutexHandles.hMutex_LoggedIn);
        break;
      case TCRT_INPUT_RESP:
        switch (receivedTossComm.playerInputResponse.playerInputResponseType) {
          case PIR_SHUTDOWN:
            printf("I can shutdown\n");
            kill(getpid(), SIGUSR1);
            break;
          case PIR_GAMENAME:
            printf("My game name is: %s\n",
                   receivedTossComm.playerInputResponse.gameName);
            break;
          default:
            printf("I received something that I can't handle");
            break;
        }
        break;
      default:
        break;
    }
  }

  free(param);
  return (void*)EXIT_SUCCESS;
}