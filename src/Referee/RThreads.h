#pragma once

#include "Referee.h"

typedef struct TParam_ReceiveQnARequest TParam_ReceiveQnARequest;

struct TParam_ReceiveQnARequest {
  Application* app;
};

void* Thread_ReceiveQnARequests(void* _param);