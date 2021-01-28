#pragma once

#include "Referee.h"

typedef struct TParam_ReceiveEntryRequest TParam_ReceiveEntryRequest;
typedef struct TParam_ReadFromGame TParam_ReadFromGame;
typedef struct TParam_ChampionshipFlow TParam_ChampionshipFlow;
typedef struct TParam_ReadFromSpecificPlayer TParam_ReadFromSpecificPlayer;
typedef struct TParam_WriteToSpecificPlayer TParam_WriteToSpecificPlayer;

struct TParam_ReceiveEntryRequest {
  Application* app;
};
struct TParam_ReadFromGame {
  Application* app;
  int myPlayerIndex;
};
struct TParam_ChampionshipFlow {
  Application* app;
};
struct TParam_ReadFromSpecificPlayer {
  Application* app;
  int myPlayerIndex;
};
struct TParam_WriteToSpecificPlayer {
  Application* app;
  int myPlayerIndex;
  TossComm* tossComm;
};

// Referee Entry
void* Thread_ReceiveEntryRequests(void* _param);

// Each Player Private Named Pipe Handles
void* Thread_ReadFromSpecificPlayer(void* _param);

// Each Player Game Handle
void* Thread_ReadFromGame(void* _param);

// Championship Flow
void* Thread_ChampionshipFlow(void* _param);
