#pragma once

#include "Player.h"

typedef struct TParam_OpenPrivateWrite TParam_OpenPrivateWrite;
typedef struct TParam_ReadFromReferee TParam_ReadFromReferee;

struct TParam_OpenPrivateWrite {
  Application* app;
};

struct TParam_ReadFromReferee {
  Application* app;
};

void* Thread_OpenPrivateWrite(void* _param);
void* Thread_ReadFromReferee(void* _param);