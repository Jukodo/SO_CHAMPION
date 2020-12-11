#include "RThreads.h"

#include "RService.h"
#include "Referee.h"

void* Thread_ReceiveQnARequests(void* _param) {
  TParam_QnARequest* param = (TParam_QnARequest*)_param;

  int fdQnARequest_Read = open(FIFO_R2P, O_RDONLY);
  if (fdQnARequest_Read == -1) {
    free(param);
    return (void*)EXIT_FAILURE;
  }

  while (1) {
    Print_Application(param->app);
    sleep(2);
  }

  close(fdQnARequest_Read);
  free(param);
  return (void*)EXIT_SUCCESS;
}