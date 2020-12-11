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

#include "../Utils/Utils.h"

typedef struct Player Player;

typedef struct NamedPipeHandles NamedPipeHandles;

typedef struct Application
    Application;  // This struct will keep all information that this
                  // application needs

struct Player {
  char username[STRING_MEDIUM];
};

struct NamedPipeHandles {
  int fdQnARequest_Read;
};

struct Application {
  Player player;
  NamedPipeHandles namedPipeHandles;
};