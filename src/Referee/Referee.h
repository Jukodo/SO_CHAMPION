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

#define DEFAULT_GAMEDIR "~/Documents/SO/SO_CHAMPION/Games/"
#define DEFAULT_MAXPLAYER 10

#define MIN_CHAMP_DURATION 60000
#define MAX_CHAMP_DURATION 600000

#define MIN_WAITING_DURATION 30000
#define MAX_WAITING_DURATION 120000

#define MAX_MAXPLAYER 30

typedef struct Game Game;

typedef struct Referee Referee;
typedef struct AvailableGames AvailableGames;

typedef struct ThreadHandles ThreadHandles;

typedef struct Application
    Application;  // This struct will keep all information that this application
                  // needs

struct Game {
  char gameName[STRING_MEDIUM];
  char fileName[STRING_MEDIUM];
};

struct Referee {
  char gameDir[STRING_MEDIUM];
  int maxPlayers;

  int championshipDuration;
  int waitingDuration;
};

struct AvailableGames {
  Game* gameList;
  int quantityGames;
};

struct ThreadHandles {
  pthread_t hQnARequests;
};

struct Application {
  Referee referee;
  AvailableGames availableGames;

  ThreadHandles threadHandles;
};