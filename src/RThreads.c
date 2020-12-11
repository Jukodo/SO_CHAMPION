#include "RThreads.h"

#include "RService.h"

void* Thread_ReceiveQnARequests(void* _param) {
  TParam_QnARequest* param = (TParam_QnARequest*)_param;

  while (1) {
    Print_Application(param->app);
    sleep(2);
  }

  free(param);
}