#pragma once

#include "Player.h"

typedef struct TParam_SendEntryRequest TParam_SendEntryRequest;
typedef struct TParam_WriteToReferee TParam_WriteToReferee;
typedef struct TParam_ReadFromReferee TParam_ReadFromReferee;

struct TParam_SendEntryRequest {
  Application* app;
  EntryRequest entryRequest;
};

struct TParam_WriteToReferee {
  Application* app;
  TossComm* tossComm;
};

struct TParam_ReadFromReferee {
  Application* app;
};

void* Thread_SendEntryRequest(void* _param);
void* Thread_WriteToReferee(void* _param);
void* Thread_ReadFromReferee(void* _param);