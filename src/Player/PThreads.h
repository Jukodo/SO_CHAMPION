#pragma once

#include "Player.h"

typedef struct TParam_SendQnARequest TParam_SendQnARequest;
typedef struct TParam_ReceiveComms TParam_ReceiveComms;

struct TParam_SendQnARequest {
  Application* app;
  QnARequest request;
};

struct TParam_ReceiveComms {
  Application* app;
};

void* Thread_SendQnARequests(void* _param);
void* Thread_ReceiveComms(void* _param);