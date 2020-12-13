#pragma once

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../Utils/CCommunication.h"
#include "../Utils/Utils.h"

typedef struct Player Player;

typedef struct NamedPipeHandles NamedPipeHandles;
typedef struct ThreadHandles ThreadHandles;
typedef struct MutexHandles MutexHandles;

typedef struct Application
    Application;  // This struct will keep all information that this
                  // application needs

struct Player {
  bool loggedIn;
  char username[STRING_MEDIUM];
};

struct NamedPipeHandles {
  int fdQnARequest_Write;  // Writes QnA Requests to Referee
  int fdComm_Read;  // Reads Communication Messages from Referee directed to
                    // self
};

struct ThreadHandles {
  pthread_t hReceiveComms;
};

struct MutexHandles {
  pthread_mutex_t *hMutex_LoggedIn;
};

struct Application {
  Player player;
  NamedPipeHandles namedPipeHandles;
  ThreadHandles threadHandles;
  MutexHandles mutexHandles;
};