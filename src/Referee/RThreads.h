#pragma once

#include "Referee.h"

typedef struct TParam_ReceiveQnARequest TParam_ReceiveQnARequest;
typedef struct TParam_ReadFromGame TParam_ReadFromGame;
typedef struct TParam_ChampionshipFlow TParam_ChampionshipFlow;
typedef struct TParam_AwakePlayer TParam_AwakePlayer;

struct TParam_ReceiveQnARequest {
  Application* app;
};
struct TParam_ReadFromGame {
  Application* app;
  int myPlayerIndex;
};
struct TParam_ChampionshipFlow {
  Application* app;
};
struct TParam_AwakePlayer {
  Application* app;
  int myPlayerIndex;
};

void* Thread_ReceiveQnARequests(void* _param);
void* Thread_ReadFromGame(void* _param);
void* Thread_ChampionshipFlow(void* _param);
void* Thread_AwakePlayer(void* _param);