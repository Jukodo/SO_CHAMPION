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
    app->playerList[i].emptyStruct = true;
  }

  return true;
}

bool Setup_Variables(Application *app, int argc, char **argv) {
  printf("Referee is starting...\n");
  printf("\nParameters received:\n");

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

        printf("\tChampionship duration: %d\n", intValue);
        app->referee.championshipDuration = intValue;
        break;
      case 'W':
        if (!isValid_WaitingDuration(app, intValue)) return false;

        printf("\tWaiting time: %d\n", intValue);
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

  printf("\nEnvironment variables:\n");
  printf("\t[GAMEDIR] - %s\n", gamedir);
  printf("\t[MAXPLAYER] - %d\n", maxplayer);

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

  app->playerList[emptyIndex].emptyStruct = false;
  app->playerList[emptyIndex].procId = procId;
  strcpy(app->playerList[emptyIndex].username, username);
  strcpy(app->playerList[emptyIndex].gamename, "Not yet developed");

  Print_Application(app);

  return true;
}

PlayerInputResponse Service_PlayerInput(Application *app, int procId,
                                        char *command) {
  PlayerInputResponse resp;
  resp.playerInputResponseType = PIR_INVALID;

  int playerIndex = getPlayerIndex(app, procId);
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

    resp = Service_HandleCommand(app, procId, command);
    return resp;
  } else {
    printf("Received a game input: %s from Player with procId: %d\n", command,
           procId);

    resp.playerInputResponseType = PIR_INPUT;
    return resp;
  }
}

PlayerInputResponse Service_HandleCommand(Application *app, int procId,
                                          char *command) {
  PlayerInputResponse resp;

  if (strcmp(command, "#quit") == 0) {
    resp.playerInputResponseType = PIR_SHUTDOWN;
  } else if (strcmp(command, "#mygame") == 0) {
    int playerIndex = getPlayerIndex(app, procId);
    if (playerIndex == -1) {
      resp.playerInputResponseType = PIR_INVALID;
      return resp;
    }

    resp.playerInputResponseType = PIR_GAMENAME;
    strcpy(resp.gameName, app->playerList[playerIndex].gamename);
  }

  return resp;
}

int getPlayerListEmptyIndex(Application *app) {
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (app->playerList[i].emptyStruct) {
      return i;
    }
  }

  return -1;
}

int getPlayerIndex(Application *app, int procId) {
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (app->playerList[i].procId == procId) {
      return i;
    }
  }

  return -1;
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
  printf("\n\tAvailable Games: %d", app->availableGames.quantityGames);
  for (int i = 0; i < app->availableGames.quantityGames; i++) {
    printf("\n\t\t[%02d] - %s", i + 1,
           app->availableGames.gameList[i].fileName);
  }

  printf("\n\tPlayer List");
  for (int i = 0; i < app->referee.maxPlayers; i++) {
    if (app->playerList[i].emptyStruct) {
      printf("\n\t\t[%02d] - EMPTY SLOT", i + 1);
    } else {
      printf("\n\t\t[%02d] - %s", i + 1, app->playerList[i].username);
    }
  }

  printf("\n");
}