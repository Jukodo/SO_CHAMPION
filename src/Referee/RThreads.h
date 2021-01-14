#pragma once

#include "Referee.h"

typedef struct TParam_ReceiveQnARequest TParam_ReceiveQnARequest;
typedef struct TParam_ReadFromGame TParam_ReadFromGame;

struct TParam_ReceiveQnARequest {
  Application* app;
};
struct TParam_ReadFromGame {
  Application* app;
  int myPlayerIndex;
};

void* Thread_ReceiveQnARequests(void* _param);
void* Thread_ReadFromGame(void* _param);