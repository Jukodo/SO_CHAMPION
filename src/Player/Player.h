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
  int fdComm_Entry;  // Writes entry request to Referee

  int fdComm_Read;   // Reads communication messages from referee directed to
                     // self
  int fdComm_Write;  // Writes communication messages to referee from private
                     // named pipe channel
};

struct ThreadHandles {
  pthread_t hReadFromReferee;
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