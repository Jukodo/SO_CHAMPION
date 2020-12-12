#include "RThreads.h"

#include "RService.h"
#include "Referee.h"

void* Thread_ReceiveQnARequests(void* _param) {
  TParam_ReceiveQnARequest* param = (TParam_ReceiveQnARequest*)_param;

  int fdQnARequest_Read = open(FIFO_R2P, O_RDONLY);
  printf("fdQnARequest_Read: %d\n", fdQnARequest_Read);

  if (fdQnARequest_Read == -1) {
    free(param);
    return (void*)EXIT_FAILURE;
  }

  QnARequest request;
  while (1) {
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
    switch (request.requestType) {
      case QnART_LOGIN:
        printf("\tReceived Login Request from Player with username: %s\n",
               request.playerLoginRequest.username);
        break;
      default:
        break;
    }
  }

  close(fdQnARequest_Read);
  free(param);
  return (void*)EXIT_SUCCESS;
}