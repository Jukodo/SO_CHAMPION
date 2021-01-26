#pragma once

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../Utils/CCommunication.h"
#include "../Utils/Utils.h"

#define DEBUG 1

#define DEFAULT_GAMEDIR "~/Documents/SO/SO_CHAMPION/Execs/Games/"
#define DEFAULT_MAXPLAYER 10
#define DEFAULT_MINPLAYERS_START 2

#define MIN_CHAMP_DURATION 60000
#define MAX_CHAMP_DURATION 600000

#define MIN_WAITING_DURATION 30000
#define MAX_WAITING_DURATION 120000

#define MAX_MAXPLAYER 30

typedef enum ChampionshipState ChampionshipState;

typedef struct GameProc GameProc;
typedef struct Game Game;
typedef struct Player Player;
typedef struct Referee Referee;
typedef struct AvailableGames AvailableGames;

typedef struct ThreadHandles ThreadHandles;

enum ChampionshipState {
  CS_WAIT_PLAYERS,
  CS_LACK_PLAYERS,
  CS_LACK_PLAYERS_DURING,
  CS_STARTED,
  CS_ENDED,
  CS_WINNER,
};

typedef struct Application
    Application;  // This struct will keep all information that this application
                  // needs

struct GameProc {
  bool active;
  int procId;

  pthread_t gameHandleThread;

  int fdReadFromGame;
  int fdWriteToGame;

  char gameName[STRING_MEDIUM];
};

struct Game {
  char gameName[STRING_MEDIUM];
  char fileName[STRING_MEDIUM];
};

struct Player {
  bool active;

  char username[STRING_LARGE];
  int procId;
  int lastScore;

  GameProc gameProc;

  int fdComm_Write;
  int fdComm_Read;

  pthread_t hComm_Read;
};

struct Referee {
  char gameDir[STRING_MEDIUM];
  int maxPlayers;

  bool isChampionshipClosed;

  int championshipDuration;
  int waitingDuration;
};

struct AvailableGames {
  Game* gameList;
  int quantityGames;
};

struct ThreadHandles {
  pthread_t hEntryRequests;
  pthread_t hChampionshipFlow;
};

struct Application {
  Referee referee;
  AvailableGames availableGames;
  Player* playerList;

  ThreadHandles threadHandles;
  pthread_mutex_t mutCountdown;
  pthread_mutex_t mutStartChampionship;
};