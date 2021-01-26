#include "RService.h"

#include "RThreads.h"
#include "Referee.h"

bool Setup_Application(Application *app, int argc, char **argv) {
  app->referee.championshipDuration = 0;
  app->referee.waitingDuration = 0;

  app->availableGames.gameList = NULL;
  app->availableGames.quantityGames = 0;
  app->referee.isChampionshipClosed = false;

  memset(app->referee.gameDir, '\0', STRING_MEDIUM);
  app->referee.maxPlayers = 0;

  if (pthread_mutex_init(&app->mutCountdown, NULL) == -1) {
    printf("[ERROR] - Timer init failed! Error: %d", errno);
  }
  pthread_mutex_lock(&app->mutCountdown);

  if (pthread_mutex_init(&app->mutStartChampionship, NULL) == -1) {
    printf("[ERROR] - Championship flag init failed! Error: %d", errno);
  }
  pthread_mutex_lock(&app->mutStartChampionship);

  if (!Setup_Variables(app, argc, argv)) {
    return false;
  }
  if (!Setup_AvailableGames(app)) {
    return false;
  }
  if (!Setup_NamedPipes(app)) {
    return false;
  }
  if (!Setup_Threads(app)) {
    return false;
  }

  app->playerList = calloc(app->referee.maxPlayers, sizeof(Player));
  if (app->playerList == NULL) {
    printf("\tUnexpected error on calloc()!\n");
    return false;
  }

  for (int i = 0; i < app->referee.maxPlayers; i++) {
    memset(&app->playerList[i], 0, sizeof(Player));
    app->playerList[i].active = false;
  }

  Print_Application(app);

  return true;
}

bool Setup_Variables(Application *app, int argc, char **argv) {
  // argv[0] always is the executable path (skipped on the cycle)
  for (int i = 1; i < argc; i++) {
    if (argc != 3) {
      printf(
          "Parameters are invalid!\n\tPlease follow the rule [(char) "
          "ID]_[(int) VALUE]\n\tAnd provide D_**** and W_****\n");
      return false;
    }

    char *identifier = strtok(argv[i], "_");
    char *value = strtok(NULL, "_");

    if (strlen(identifier) != 1 || !Utils_StringIsNumber(value)) {
      printf(
          "Parameters are invalid!\n\tPlease follow the rule [(char) "
          "ID]_[(int) VALUE]\n\tAnd provide D_**** and W_****\n");
      return false;
    }

    int intValue = atoi(value);
    switch (identifier[0]) {
      case 'D':
        if (!isValid_ChampionshipDuration(app, intValue)) return false;

        app->referee.championshipDuration = intValue;
        break;
      case 'W':
        if (!isValid_WaitingDuration(app, intValue)) return false;

        app->referee.waitingDuration = intValue;
        break;
      default:
        printf("\tUnknown paremeter! Program will exit...\n");
        return false;
    }
  }

  char gamedir[STRING_MEDIUM];
  strcpy(gamedir, (Utils_StringIsEmpty(getenv("GAMEDIR"))) ? DEFAULT_GAMEDIR
                                                           : getenv("GAMEDIR"));

  int maxplayer = (Utils_StringIsEmpty(getenv("MAXPLAYER")))
                      ? DEFAULT_MAXPLAYER
                      : Utils_StringIsNumber(getenv("MAXPLAYER"))
                            ? atoi(getenv("MAXPLAYER"))
                            : DEFAULT_MAXPLAYER;

  if (maxplayer > MAX_MAXPLAYER) maxplayer = MAX_MAXPLAYER;
  if (maxplayer <= 1) maxplayer = DEFAULT_MAXPLAYER;

  DIR *dir = opendir(gamedir);
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    printf("\tDirectory does not exist... Create? (* = yes | n = no)\n");
    printf("\t-> ");

    char line[STRING_MEDIUM];
    scanf("%[^\n]", line);
    Utils_CleanStdin();

    if (strcmp(line, "n") != 0 && strcmp(line, "N") != 0) {
      mkdir(gamedir, 0777);
      DIR *dir = opendir(gamedir);
      if (dir) {
        printf("\n\tDirectory has been created successfuly!");
        closedir(dir);
      } else {
        printf("\n\tUnexpected error on mkdir()! Program will exit...");
        return false;
      }
    } else {
      printf(
          "\n\tProgram cannot run without an existing directory! Program "
          "will exit...");
      return false;
    }
  } else {
    printf("\n\tUnexpected error on opendir()! Program will exit...");
    return false;
  }

  strcpy(app->referee.gameDir, gamedir);
  app->referee.maxPlayers = maxplayer;
  /**TAG_TODO
   * Add environment variables into struct variables
   */
  return true;
}

bool Setup_AvailableGames(Application *app) {
  DIR *gamedir = opendir(app->referee.gameDir);
  struct dirent *dir;
  if (gamedir) {
    while ((dir = readdir(gamedir)) != NULL) {
      if (dir->d_type == DT_REG) {  // Is a file (maybe executable)
        app->availableGames.quantityGames++;
        if (app->availableGames.gameList == NULL) {
          app->availableGames.gameList =
              calloc(app->availableGames.quantityGames, sizeof(Game));
          if (app->availableGames.gameList == NULL) {
            printf("Malloc failed\n");
            return false;
          }
        } else {
          Game *newGameList =
              realloc(app->availableGames.gameList,
                      app->availableGames.quantityGames * sizeof(Game));
          if (newGameList == NULL) {
            printf("Realloc failed\n");
            return false;
          } else {
            app->availableGames.gameList = newGameList;
          }
        }
        strcpy(
            app->availableGames.gameList[app->availableGames.quantityGames - 1]
                .fileName,
            dir->d_name);
      }
    }

    closedir(gamedir);
  } else {
    printf("\n\tUnexpected error on mkdir()! Program will exit...");
    return false;
  }

  return true;
}

bool Setup_NamedPipes(Application *app) {
  if (mkfifo(FIFO_REFEREE_ENTRY, 0777) == -1) {
    // Only returns error if file does not exist after operation
    if (errno != EEXIST) {
      printf("\n\tUnexpected error on mkfifo()! Program will exit...");
      printf("\n\t\tError: %d", errno);
      return false;
    }
  }

  return true;
}

bool Setup_Threads(Application *app) {
#pragma region Read Entry Requests
  TParam_ReceiveEntryRequest *param_entryReq =
      malloc(sizeof(TParam_ReceiveEntryRequest));
  if (param_entryReq == NULL) {
    return false;
  }

  param_entryReq->app = app;

  if (pthread_create(&app->threadHandles.hEntryRequests, NULL,
                     &Thread_ReceiveEntryRequests,
                     (void *)param_entryReq) != 0) {
    free(param_entryReq);
    return false;
  };
#pragma endregion

#pragma region Championship flow
  TParam_ChampionshipFlow *param_csf = malloc(sizeof(TParam_ChampionshipFlow));
  if (param_csf == NULL) {
    return false;
  }

  param_csf->app = app;

  if (pthread_create(&app->threadHandles.hChampionshipFlow, NULL,
                     &Thread_ChampionshipFlow, (void *)param_csf) != 0) {
    free(param_csf);
    return false;
  };
#pragma endregion

  return true;
}

PlayerLoginResponseType Service_PlayerLogin(Application *app, int procId,
                                            char *username) {
  // Check if championship is closed
  if (app->referee.isChampionshipClosed) {
    return PLR_INVALID_CLOSED;
  }

  // Check if player with same username already exists
  int emptyIndex = getPlayerListEmptyIndex(app);
  if (getPlayerIndexByUsername(app, username) != -1) {
    return PLR_INVALID_EXISTS;
  }

  // Check if an empty slot is available and get empty slot index (-1 if no slot
  // available)
  if (emptyIndex == -1) {
    return PLR_INVALID_FULL;
  }

  Player *newPlayer = &app->playerList[emptyIndex];

  char fifoName_WriteToPlayer[STRING_LARGE];
  sprintf(fifoName_WriteToPlayer, "%s_%d", FIFO_REFEREE_TO_PLAYER, procId);

  newPlayer->fdComm_Write = open(fifoName_WriteToPlayer, O_RDWR);
  if (newPlayer->fdComm_Write == -1) {
    printf("[ERROR] - Unexpected error on open()! Error: %d\n", errno);
    return PLR_INVALID_UNDEFINED;
  }

  if (DEBUG) {
    printf(
        "[DEBUG] - Openned a named pipe belonging to %s named as %s... I can "
        "now "
        "write!\n",
        username, fifoName_WriteToPlayer);
  }

  newPlayer->active = true;
  newPlayer->procId = procId;
  strcpy(newPlayer->username, username);

  TParam_ReadFromSpecificPlayer *param =
      malloc(sizeof(TParam_ReadFromSpecificPlayer));
  if (param == NULL) {
    return PLR_INVALID_UNDEFINED;
  }
  param->app = app;
  param->myPlayerIndex = emptyIndex;
  if (pthread_create(&newPlayer->hComm_Read, NULL,
                     &Thread_ReadFromSpecificPlayer, (void *)param) != 0) {
    printf("[ERROR] - Could not create thread\n");
    free(param);
    return PLR_INVALID_UNDEFINED;
  };

  printf("\n\t[INFO] New player has logged in!\n");
  printf("\t\tUsername: %s\n", username);
  printf("\t\tProcId: %d\n\n", procId);

  if (getQuantityPlayers(app) >= DEFAULT_MINPLAYERS_START) {
    pthread_mutex_unlock(&app->mutStartChampionship);
  }

  return PLR_SUCCESS;
}

void Service_PlayerLogout(Application *app, int procId) {
  int foundPlayerIndex = getPlayerIndexByProcId(app, procId);
  if (foundPlayerIndex == -1) {
    return;
  }

  Player *foundPlayer = &app->playerList[foundPlayerIndex];
  foundPlayer->active = false;
  memset(foundPlayer->username, '\0', STRING_LARGE);

  /**TAG_TODO
   * Close all handles for player
   * Named pipes
   * Threads
   * Active game
   * etc...
   */
}

void Service_PlayerInput(Application *app, int procId, char *command) {
  int playerIndex = getPlayerIndexByProcId(app, procId);
  if (playerIndex == -1) {
    // Since it could not find player, it can't communicate back
    printf("[WARNING] - Could not find Player with procId: %d\n", procId);
    return;
  }
  if (Utils_StringIsEmpty(command)) {
    // Since the command is empty, it is unnecessary to communicate back
    printf("[WARNING] - Received command is empty!\n");
  }

  // Check if received input is a command or an input directed to the game
  if (command[0] == '#') {
    if (DEBUG) {
      printf("[DEBUG] - Received a command: %s from %s with procId: %d\n",
             command, app->playerList[playerIndex].username, procId);
    }

    // Create a response comm
    TossComm *tossComm = malloc(sizeof(TossComm));
    if (tossComm == NULL) {
      return;
    }

    tossComm->tossType = TCRT_INPUT_RESP;
    tossComm->playerInputResponse =
        Service_HandlePlayerCommand(app, procId, command);

    Service_SendTossComm(app, procId, tossComm);
    return;
  } else {
    // An input directed to the game does not need an answer
    if (DEBUG) {
      printf("[DEBUG] - Received a game input: %s from %s with procId: %d\n",
             command, app->playerList[playerIndex].username, procId);
    }

    Player player = app->playerList[playerIndex];
    if (player.gameProc.active) {
      if (DEBUG) {
        printf("[DEBUG] - Trying to write %s to game of player [%s]\n", command,
               player.username);
      }
      if (write(player.gameProc.fdWriteToGame, command, STRING_LARGE) == -1) {
        printf("[ERROR] - Could not write to game... Error: %d", errno);
      }
    }
    return;
  }
}

PlayerInputResponse Service_HandlePlayerCommand(Application *app, int procId,
                                                char *command) {
  PlayerInputResponse resp;

  if (strcmp(command, "#quit") == 0) {
    resp.playerInputResponseType = PIR_SHUTDOWN;
  } else if (strcmp(command, "#mygame") == 0) {
    int playerIndex = getPlayerIndexByProcId(app, procId);
    if (playerIndex == -1) {
      resp.playerInputResponseType = PIR_INVALID;
      return resp;
    }

    resp.playerInputResponseType = PIR_GAMENAME;
    if (app->playerList[playerIndex].gameProc.active) {
      snprintf(resp.gameName, STRING_LARGE, "%s",
               app->playerList[playerIndex].gameProc.gameName);
    } else {
      snprintf(resp.gameName, STRING_LARGE, "Currently not playing!");
    }
  }

  return resp;
}

void Service_HandleSelfCommand(Application *app, char *command) {
  char *commandName = strtok(command, " ");

  if (strcmp(commandName, "players") == 0) {
    Print_PlayerList(app);
  } else if (strcmp(commandName, "games") == 0) {
    Print_AvailableGameList(app);
  } else if (strcmp(commandName, "k") == 0) {
    char *commandValue = strtok(NULL, " ");
    Service_KickPlayer(app, commandValue);
  } else if (strcmp(commandName, "exit") == 0) {
    Service_Exit(app);
  } else if (strcmp(commandName, "t_opengame") == 0) {
    if (app->playerList[0].active) {
      Service_OpenGame(app, app->playerList[0].procId);
    }
  } else if (strcmp(commandName, "t_unlockcsf") == 0) {
    pthread_mutex_unlock(&app->mutStartChampionship);
  } else if (strcmp(commandName, "t_unlocklb") == 0) {
    pthread_mutex_unlock(&app->mutCountdown);
  } else if (strcmp(commandName, "t_unlockcs") == 0) {
    pthread_mutex_unlock(&app->mutCountdown);
  }
}

bool Service_KickPlayer(Application *app, char *username) {
  if (Utils_StringIsEmpty(username)) {
    printf("[WARNING] - Cannot kick player! Username parameter is empty...\n");
    return false;
  }

  int playerIndex = getPlayerIndexByUsername(app, username);
  if (playerIndex == -1) {
    printf("[WARNING] - Cannot kick [%s]! Player could not be found...\n",
           username);
    return false;
  }

  printf("\n\t[INFO] - Kicking a player\n");
  printf("\t\tUsername: %s\n", username);
  TossComm tossComm;
  tossComm.tossType = TCRT_INPUT_RESP;
  tossComm.playerInputResponse.playerInputResponseType = PIR_SHUTDOWN;

  int writtenBytes = write(app->playerList[playerIndex].fdComm_Write, &tossComm,
                           sizeof(TossComm));
  if (writtenBytes != sizeof(TossComm)) {
    printf("[ERROR] - Could not kick player %s! Error unknown...\n", username);
  }

  Clean_Player(app, app->playerList[playerIndex].procId);
  return true;
}

void Service_Exit(Application *app) {
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (app->playerList[i].active) {
      Service_KickPlayer(app, app->playerList[i].username);
    }
  }
  kill(getpid(), SIGUSR1);
}

void Service_OpenGame(Application *app, int playerProcId) {
  int playerIndex = getPlayerIndexByProcId(app, playerProcId);
  if (playerIndex == -1) {
    printf("[ERROR] - Tried to open a game for a non existing player!\n");
    return;
  }

  int fdGame2Referee[2];  // Child => Parent
  int fdReferee2Game[2];  // Parent => Child

  if (pipe(fdGame2Referee) == -1) {
    return;
  }
  if (pipe(fdReferee2Game) == -1) {
    return;
  }

  int forkStatus = fork();
  if (forkStatus < 0) {
    printf("[ERROR] - Fork failed!\n");
  }

  int randomGameIndex = getRandomGameIndex(app);

  if (forkStatus == 0) {
    close(fdGame2Referee[0]);  // Will not need G => R reading pipe
    close(fdReferee2Game[1]);  // Will not need R => G writting pipe

    close(1);
    dup(fdGame2Referee[1]);

    close(0);
    dup(fdReferee2Game[0]);

    int gameIndex = getRandomGameIndex(app);

    char gamePath[STRING_LARGE];
    snprintf(gamePath, STRING_LARGE, "%s%s", app->referee.gameDir,
             app->availableGames.gameList[randomGameIndex].fileName);

    if (execl(gamePath, gamePath, NULL) == -1) {
      fprintf(stderr, "[ERROR] - Execl failed! Error: %d\n", errno);
    }
    fprintf(stderr, "[ERROR] - I'm not supposed to be here\n");
  } else {
    close(fdGame2Referee[1]);  // Will not need G => R writting pipe
    close(fdReferee2Game[0]);  // Will not need R => G reading pipe

    TParam_ReadFromGame *param = malloc(sizeof(TParam_ReadFromGame));
    if (param == NULL) {
      return;
    }

    param->app = app;
    param->myPlayerIndex = playerIndex;
    if (pthread_create(&app->playerList[playerIndex].gameProc.gameHandleThread,
                       NULL, &Thread_ReadFromGame, (void *)param) != 0) {
      free(param);
      return;
    };

    app->playerList[playerIndex].gameProc.active = true;
    app->playerList[playerIndex].gameProc.procId = forkStatus;

    app->playerList[playerIndex].gameProc.fdReadFromGame = fdGame2Referee[0];
    app->playerList[playerIndex].gameProc.fdWriteToGame = fdReferee2Game[1];

    snprintf(app->playerList[playerIndex].gameProc.gameName, STRING_MEDIUM,
             "%s", app->availableGames.gameList[randomGameIndex].fileName);
  }
}

void Service_CloseGame(Application *app, int playerProcId) {
  int playerIndex = getPlayerIndexByProcId(app, playerProcId);
  if (playerIndex == -1) {
    printf("[ERROR] - Tried to open a game for a non existing player!\n");
    return;
  }

  Player *player = &app->playerList[playerIndex];

  int status;

  kill(player->gameProc.procId, SIGUSR1);
  if (waitpid(player->gameProc.procId, &status, WUNTRACED) == -1) {
    printf("[ERROR] - Failed waiting for %s game! Error: %d\n",
           player->username, errno);
    exit(EXIT_FAILURE);
  }

  if (DEBUG) {
    printf("[DEBUG] - Retrieved score of %d from %s game!\n",
           WEXITSTATUS(status), player->username);
  }

  close(player->gameProc.fdReadFromGame);
  close(player->gameProc.fdWriteToGame);

  player->gameProc.active = false;
  player->lastScore = WEXITSTATUS(status);

  memset(player->gameProc.gameName, '\0', STRING_MEDIUM);
}

void Service_BroadcastChampionshipState(Application *app, int state) {
  TossComm tossComm;
  tossComm.tossType = TCRT_CHAMPIONSHIP_MSG;

  switch (state) {
    case 1:
      snprintf(tossComm.championshipMsg.msg, STRING_LARGE,
               "THE WINNER IS.........\n");
      break;
    case 2:
      snprintf(tossComm.championshipMsg.msg, STRING_LARGE,
               "Championship has started\n");
      break;
    case 3:
      snprintf(tossComm.championshipMsg.msg, STRING_LARGE,
               "Championship has been canceled due to lack of players\n");
      break;
    case 4:
      snprintf(tossComm.championshipMsg.msg, STRING_LARGE,
               "Waiting %d seconds for more players\n",
               app->referee.waitingDuration);
      break;
    case 5:
      snprintf(tossComm.championshipMsg.msg, STRING_LARGE,
               "Championship is now full! Starting...\n");
      break;
    default:
      snprintf(tossComm.championshipMsg.msg, STRING_LARGE,
               "I forgot what i was about to say...\n");
      break;
  }
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (app->playerList[i].active) {
      int writtenBytes =
          write(app->playerList[i].fdComm_Write, &tossComm, sizeof(TossComm));
      if (writtenBytes <= 0) {
        printf("[ERROR] Could not broadcast to %s... Error: %d\n",
               app->playerList[i].username, errno);
      }
    }
  }
}

/**Send a TossComm to player with specific process id
 * Create a TossComm allocated in memory (malloc)
 * Fill up the toss comm and inform which procId should it send to
 */
void Service_SendTossComm(Application *app, int procId, TossComm *tossComm) {
  int sendToIndex = getPlayerIndexByProcId(app, procId);
  if (sendToIndex == -1) {
    printf("[WARNING] - Could not find Player with procId: %d\n", procId);
    return;
  }

  // Create a thread param for said thread
  TParam_WriteToSpecificPlayer *param =
      malloc(sizeof(TParam_WriteToSpecificPlayer));
  if (param == NULL) {
    free(tossComm);
    return;
  }

  param->app = app;
  param->myPlayerIndex = sendToIndex;
  param->tossComm = tossComm;

  // Create thread so that this operation won't block other operations
  pthread_t currThread;
  if (pthread_create(&currThread, NULL, &Thread_WriteToSpecificPlayer,
                     (void *)param) != 0) {
    free(tossComm);
    free(param);
    return;
  };
}

/**
 * Get a random game index from avaliable games
 */
int getRandomGameIndex(Application *app) {
  srand(time(NULL));

  return rand() % app->availableGames.quantityGames;
}

/**
 * Returns empty player index
 * If max players is reached, returns -1
 */
int getPlayerListEmptyIndex(Application *app) {
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (!app->playerList[i].active) {
      return i;
    }
  }

  return -1;
}

/**
 * Returns found player index
 * If not found, returns -1
 */
int getPlayerIndexByProcId(Application *app, int procId) {
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (app->playerList[i].procId == procId) {
      return i;
    }
  }

  return -1;
}

/**
 * Returns found player index
 * If not found, returns -1
 */
int getPlayerIndexByUsername(Application *app, char *username) {
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (strcmp(app->playerList[i].username, username) == 0) {
      return i;
    }
  }

  return -1;
}

/**
 * Returns quantity of registered players
 */
int getQuantityPlayers(Application *app) {
  int quantity = 0;

  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (app->playerList[i].active) {
      quantity++;
    }
  }

  return quantity;
}

/**
 * Cleans memory which currently holds player with procId received
 */
void Clean_Player(Application *app, int procId) {
  int playerIndex = getPlayerIndexByProcId(app, procId);
  if (playerIndex == -1) {
    return;
  }

  close(app->playerList[playerIndex].fdComm_Write);
  memset(&app->playerList[playerIndex], '\0', sizeof(Player));
  app->playerList[playerIndex].active = false;
}

/**
 * Validates received championship duration
 */
bool isValid_ChampionshipDuration(Application *app, int value) {
  // App already registered a championship duration
  if (app->referee.championshipDuration > 0) {
    printf("\n[ERROR] - Duplicate parameter! Program will exit...");
    return false;
  }

  if (value < MIN_CHAMP_DURATION) {
    printf(
        "\n[ERROR] - Championship duration cannot be lower than %d! Program "
        "will exit...",
        MIN_CHAMP_DURATION);
    return false;
  }

  if (value > MAX_CHAMP_DURATION) {
    printf(
        "\n[ERROR] - Championship duration cannot be higher than %d! Program "
        "will exit...",
        MAX_CHAMP_DURATION);
    return false;
  }

  return true;
}

/**
 * Validates received waiting duration
 */
bool isValid_WaitingDuration(Application *app, int value) {
  // App already registered a championship duration
  if (app->referee.waitingDuration > 0) {
    printf("\n[ERROR] - Duplicate parameter! Program will exit...");
    return false;
  }

  if (value < MIN_WAITING_DURATION) {
    printf(
        "\n[ERROR] - Waiting duration cannot be lower than %d! Program will "
        "exit...",
        MIN_WAITING_DURATION);
    return false;
  }

  if (value > MAX_WAITING_DURATION) {
    printf(
        "\n[ERROR] - Waiting duration cannot be higher than %d! Program will "
        "exit...",
        MAX_WAITING_DURATION);
    return false;
  }

  return true;
}

/**
 * Print application data
 */
void Print_Application(Application *app) {
  printf("\n\nMyApplication");
  printf("\n\tChampionship duration: %d", app->referee.championshipDuration);
  printf("\n\tWaiting duration: %d", app->referee.waitingDuration);
  printf("\n\tGame directory: %s", app->referee.gameDir);
  printf("\n\tMax players: %d", app->referee.maxPlayers);
  Print_AvailableGameList(app);
  Print_PlayerList(app);

  printf("\n");
}

/**
 * Print player list
 */
void Print_PlayerList(Application *app) {
  printf("\n\tPlayer List\n");
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (!app->playerList[i].active) {
      printf("\t\t[%02d] - EMPTY SLOT\n", i + 1);
    } else {
      printf("\t\t[%02d] - %s\n", i + 1, app->playerList[i].username);
    }
  }
}

/**
 * Print available game list
 */
void Print_AvailableGameList(Application *app) {
  printf("\n\tAvailable Game List\n");
  for (int i = 0; i < app->availableGames.quantityGames; i++) {
    printf("\t\t[%02d] - %s\n", i + 1,
           app->availableGames.gameList[i].fileName);
  }
}