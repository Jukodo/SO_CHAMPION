#include "RService.h"

#include "RThreads.h"
#include "Referee.h"

bool Setup_Application(Application *app, int argc, char **argv) {
  app->referee.championshipDuration = 0;
  app->referee.waitingDuration = 0;

  app->availableGames.gameList = NULL;
  app->availableGames.quantityGames = 0;

  memset(app->referee.gameDir, '\0', STRING_MEDIUM);
  app->referee.maxPlayers = 0;

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
    memset(&app->playerList[i], '\0', sizeof(Player));
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
  if (mkfifo(FIFO_REFEREE, 0777) == -1) {
    // Only returns error if file does not exist after operation
    if (errno != EEXIST) {
      printf("\n\tUnexpected error on mkfifo()! Program will exit...");
      printf("\n\t\tError: %d", errno);
      return false;
    }
  }

  /**TAG_LOOKAT
   * Is this needed?
   * Virtual Client can be useful to keep the thread not blocked, but does it
   * matter?
   */
  // if (open(FIFO_REFEREE, O_WRONLY) == -1) {
  //   printf("\n\tUnexpected error on open()! Program will exit...");
  //   return 1;
  // }

  return true;
}

bool Setup_Threads(Application *app) {
  TParam_ReceiveQnARequest *param = malloc(sizeof(TParam_ReceiveQnARequest));
  if (param == NULL) {
    return false;
  }

  param->app = app;

  if (pthread_create(&app->threadHandles.hQnARequests, NULL,
                     &Thread_ReceiveQnARequests, (void *)param) != 0) {
    free(param);
    return false;
  };

  return true;
}

bool Service_PlayerLogin(Application *app, int procId, char *username) {
  /**TAG_TODO
   * Validate is Player is valid to login
   * Conditions:
   *  - Championship is open for new players
   *  - No duplicate username
   *  Check up and maybe other conditions
   */

  int emptyIndex = getPlayerListEmptyIndex(app);
  if (emptyIndex == -1) {
    return false;
  }

  /**TAG_TODO
   * Just open the fifo from the player: {FIFO_PLAYER}_{procId}
   * Fifo is created by each player
   */

  char playerFifoName[STRING_LARGE];
  sprintf(playerFifoName, "%s_%d", FIFO_PLAYER, procId);
  app->playerList[emptyIndex].fdComm_Write = open(playerFifoName, O_RDWR);
  if (app->playerList[emptyIndex].fdComm_Write == -1) {
    printf("\tUnexpected error on open()!\n");
    printf("\t\tError: %d\n", errno);
    return false;
  }

  app->playerList[emptyIndex].active = true;
  app->playerList[emptyIndex].procId = procId;
  sem_init(&app->playerList[emptyIndex].semNamedPipe, PTHREAD_PROCESS_PRIVATE,
           1);
  strcpy(app->playerList[emptyIndex].username, username);

  printf("\nNew player has logged in!\n");
  printf("\tUsername: %s\n", username);
  printf("\tProcId: %d\n\n", procId);

  return true;
}

PlayerInputResponse Service_PlayerInput(Application *app, int procId,
                                        char *command) {
  PlayerInputResponse resp;
  resp.playerInputResponseType = PIR_INVALID;

  int playerIndex = getPlayerIndexByProcId(app, procId);
  if (playerIndex == -1) {
    printf("Could not find Player with procId:%d\n", procId);
    return resp;
  }
  if (Utils_StringIsEmpty(command)) {
    printf("Received command is empty\n");
    return resp;
  }

  // Check if received input is a command or an input directed to the game
  if (command[0] == '#') {
    printf("Received a command: %s from Player with procId: %d\n", command,
           procId);

    resp = Service_HandlePlayerCommand(app, procId, command);
    return resp;
  } else {
    printf("Received a game input: %s from Player with procId: %d\n", command,
           procId);

    resp.playerInputResponseType = PIR_INPUT;
    return resp;
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
  }
}

bool Service_KickPlayer(Application *app, char *username) {
  if (Utils_StringIsEmpty(username)) {
    printf("Username is empty!\n");
    return false;
  }

  int playerIndex = getPlayerIndexByUsername(app, username);
  if (playerIndex == -1) {
    printf("Username is invalid!\n");
    return false;
  }

  printf("\nTrying to kick a player\n");
  printf("\tUsername: %s\n", username);
  TossComm tossComm;
  tossComm.tossType = TCRT_INPUT_RESP;
  tossComm.playerInputResponse.playerInputResponseType = PIR_SHUTDOWN;

  int writtenBytes = write(app->playerList[playerIndex].fdComm_Write, &tossComm,
                           sizeof(TossComm));
  if (writtenBytes != sizeof(TossComm)) {
    printf("\nKick player has failed!\n");
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
      fprintf(stderr, "[CHILDREN ERROR] - Execl failed! Error: %d\n", errno);
    }
    fprintf(stderr, "[CHILDREN ERROR] - I'm not supposed to be here\n");
  } else {
    close(fdGame2Referee[1]);  // Will not need G => R writting pipe
    close(fdReferee2Game[0]);  // Will not need R => G reading pipe

    printf("Reached here\n");

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

    app->playerList[playerIndex].gameProc.fdReadFromGame = fdGame2Referee[0];
    app->playerList[playerIndex].gameProc.fdWriteToGame = fdReferee2Game[1];

    snprintf(app->playerList[playerIndex].gameProc.gameName, STRING_MEDIUM,
             "%s", app->availableGames.gameList[randomGameIndex].fileName);
  }
}

int getRandomGameIndex(Application *app) {
  srand(time(NULL));

  return rand() % app->availableGames.quantityGames;
}

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

int getPlayerIndexByUsername(Application *app, char *username) {
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (strcmp(app->playerList[i].username, username) == 0) {
      return i;
    }
  }

  return -1;
}

void Clean_Player(Application *app, int procId) {
  int playerIndex = getPlayerIndexByProcId(app, procId);
  if (playerIndex == -1) {
    return;
  }

  close(app->playerList[playerIndex].fdComm_Write);
  memset(&app->playerList[playerIndex], '\0', sizeof(Player));
  app->playerList[playerIndex].active = false;
}

bool isValid_ChampionshipDuration(Application *app, int value) {
  // App already registered a championship duration
  if (app->referee.championshipDuration > 0) {
    printf("\n\tDuplicate parameter! Program will exit...");
    return false;
  }

  if (value < MIN_CHAMP_DURATION) {
    printf(
        "\n\tChampionship duration cannot be lower than %d! Program will "
        "exit...",
        MIN_CHAMP_DURATION);
    return false;
  }

  if (value > MAX_CHAMP_DURATION) {
    printf(
        "\n\tChampionship duration cannot be higher than %d! Program will "
        "exit...",
        MAX_CHAMP_DURATION);
    return false;
  }

  return true;
}

bool isValid_WaitingDuration(Application *app, int value) {
  // App already registered a championship duration
  if (app->referee.waitingDuration > 0) {
    printf("\n\tDuplicate parameter! Program will exit...");
    return false;
  }

  if (value < MIN_WAITING_DURATION) {
    printf(
        "\n\tWaiting duration cannot be lower than %d! Program will "
        "exit...",
        MIN_WAITING_DURATION);
    return false;
  }

  if (value > MAX_WAITING_DURATION) {
    printf(
        "\n\tWaiting duration cannot be higher than %d! Program will "
        "exit...",
        MAX_WAITING_DURATION);
    return false;
  }

  return true;
}

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

void Print_AvailableGameList(Application *app) {
  printf("\n\tAvailable Game List\n");
  for (int i = 0; i < app->availableGames.quantityGames; i++) {
    printf("\t\t[%02d] - %s\n", i + 1,
           app->availableGames.gameList[i].fileName);
  }
}