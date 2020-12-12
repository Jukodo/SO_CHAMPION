#pragma once

#include "Player.h"

typedef struct TParam_SendQnARequest TParam_SendQnARequest;

struct TParam_SendQnARequest {
  Application* app;
  QnARequest request;
};

void* Thread_SendQnARequests(void* _param);