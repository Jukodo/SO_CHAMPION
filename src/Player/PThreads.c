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

  char fifoName_PlayerRead[STRING_LARGE];
  sprintf(fifoName_PlayerRead, "%s_%d", FIFO_PLAYER, getpid());
  param->app->namedPipeHandles.fdComm_Read =
      open(fifoName_PlayerRead, O_RDONLY);
  if (param->app->namedPipeHandles.fdComm_Read == -1) {
    printf("[ERROR] - Unexpected error on open()! Error: %d\n", errno);
    return (void*)EXIT_FAILURE;
  }

  int readBytes;
  TossComm receivedTossComm;

  do {
    readBytes = read(param->app->namedPipeHandles.fdComm_Read,
                     &receivedTossComm, sizeof(TossComm));
    printf("[DEBUG] - I've read %d bytes!\n", readBytes);

    if (readBytes == sizeof(TossComm)) {
      switch (receivedTossComm.tossType) {
        case TCRT_LOGIN_RESP:
          switch (
              receivedTossComm.playerLoginResponse.playerLoginResponseType) {
            case PLR_SUCCESS:
              param->app->player.loggedIn = true;
              break;
            case PLR_INVALID_EXISTS:
              param->app->player.loggedIn = false;
              printf(
                  "[WARNING] - Player with the same username already "
                  "exists!\nTry again with another username...\n");
              break;
            case PLR_INVALID_FULL:
              param->app->player.loggedIn = false;
              printf("[WARNING] - Player queue is full! Try again later...\n");
              break;
            case PLR_INVALID_CLOSED:
              param->app->player.loggedIn = false;
              printf(
                  "[WARNING] - A championship is running at the moment or not "
                  "accepting players by choice! Try again later...\n");
              break;
            default:
              param->app->player.loggedIn = false;
              printf("[ERROR] - Unhandled login response!\n");
              break;
          }
          pthread_mutex_unlock(param->app->mutexHandles.hMutex_LoggedIn);
          break;
        case TCRT_INPUT_RESP:
          switch (
              receivedTossComm.playerInputResponse.playerInputResponseType) {
            case PIR_SHUTDOWN:
              printf("I can shutdown\n");
              kill(getpid(), SIGUSR1);
              break;
            case PIR_GAMENAME:
              printf("My game name is: %s\n",
                     receivedTossComm.playerInputResponse.gameName);
              break;
            default:
              printf(
                  "[WARNING] - I received something from TCRT_INPUT_RESP that "
                  "I "
                  "can't handle\n");
              break;
          }
          break;
        case TCRT_GAME_OUTPUT:
          printf("%s\n", receivedTossComm.gameOutput.output);
          break;
        case TCRT_CHAMPIONSHIP_MSG:
          printf("%s\n", receivedTossComm.championshipMsg.msg);
          break;
        default:
          printf("[WARNING] - I got information but it is unhandled!\n");
          break;
      }
    }
  } while (readBytes > 0);

  printf("[DEBUG] - Lost connection to server!\n");

  free(param);
  return (void*)EXIT_SUCCESS;
}