#pragma once

#include "Referee.h"

typedef struct TParam_QnARequest TParam_QnARequest;

struct TParam_QnARequest {
  Application* app;
};

void* Thread_ReceiveQnARequests(void* _param);