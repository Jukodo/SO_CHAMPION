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
    printf("\n\tCould not communicate with Referee's named pipe!");
    printf("\n\tError: %d", errno);
    return (void*)EXIT_FAILURE;
  }

  // Receive Answer
  switch (param->request.requestType) {
    case QnART_LOGIN:
      break;
    default:
      break;
  }

  free(param);
  return (void*)EXIT_SUCCESS;
}