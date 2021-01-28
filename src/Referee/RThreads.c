#include "RThreads.h"

#include "RService.h"
#include "Referee.h"

#pragma region Referee Entry
void* Thread_ReceiveEntryRequests(void* _param) {
  TParam_ReceiveEntryRequest* param = (TParam_ReceiveEntryRequest*)_param;

  EntryRequest entryRequest;

  int fdQnARequest_Read;
  bool closed = true;
  while (1) {
    if (closed) {
      fdQnARequest_Read = open(FIFO_REFEREE_ENTRY, O_RDONLY);

      if (fdQnARequest_Read == -1) {
        free(param);
        return (void*)EXIT_FAILURE;
      }

      closed = false;
    }
    // Receive Question
    int readBytes =
        read(fdQnARequest_Read, &entryRequest, sizeof(EntryRequest));
    if (readBytes != sizeof(EntryRequest)) {
      if (DEBUG) {
        printf(
            "[DEBUG] - A player disconnected from entry channel! Getting ready "
            "for next...\n");
      }

      close(fdQnARequest_Read);
      closed = true;
      continue;
    }

    // Send Answer
    TossComm tossComm;
    int newPlayerIndex;
    int failedPlayer_fdComm_Write = -1;
    tossComm.playerLoginResponse.playerLoginResponseType = Service_PlayerLogin(
        param->app, entryRequest.procId, entryRequest.username);
    tossComm.tossType = TCRT_LOGIN_RESP;

    if (tossComm.playerLoginResponse.playerLoginResponseType == PLR_SUCCESS) {
      newPlayerIndex = getPlayerIndexByProcId(param->app, entryRequest.procId);

      if (newPlayerIndex == -1) {
        printf(
            "[ERROR] - Tried to get a non existing player! Tried to find "
            "procId: "
            "%d\n",
            entryRequest.procId);
      }

      Player* newPlayer = &param->app->playerList[newPlayerIndex];

      /**TAG_INFO
       * No need to block named pipe write operations since pipe write
       * operations are atomic (One operation at a time)
       *
       * A situation where a block would be needed is when the writtings would
       * exceed the buffer size, which the minimum is 512 bytes and default
       * "On Linux, it's 4096 bytes"
       *
       * Sources:
       * https://unix.stackexchange.com/questions/68146/what-are-guarantees-for-concurrent-writes-into-a-named-pipe
       */
      int writtenBytes =
          write(newPlayer->fdComm_Write, &tossComm, sizeof(TossComm));

      if (writtenBytes != sizeof(TossComm)) {
        printf("[ERROR] - Failed to communicate with \n", errno);
        printf("TossComm failed\n");
      }
    } else {
      // If request fails, create a temporary connection to named pipe named
      // by received process id and inform player that the login could not be
      // done

      // Concat fifo name to received process id
      char fifoName_WriteToPlayer[STRING_LARGE];
      sprintf(fifoName_WriteToPlayer, "%s_%d", FIFO_REFEREE_TO_PLAYER,
              entryRequest.procId);

      // Open the temporary connection to named pipe
      failedPlayer_fdComm_Write = open(fifoName_WriteToPlayer, O_RDWR);
      if (failedPlayer_fdComm_Write == -1) {
        printf("[ERROR] - Unexpected error on open()! Error: %d\n", errno);
        continue;
      }

      // Tries to write into the named pipe (does not matter if it fails)
      write(failedPlayer_fdComm_Write, &tossComm, sizeof(TossComm));

      // Reset temp named pipe
      close(failedPlayer_fdComm_Write);
    }
  }

  close(fdQnARequest_Read);
  free(param);
  return (void*)EXIT_SUCCESS;
}
#pragma endregion

#pragma region Each Player Private Named Pipe Handles
void* Thread_ReadFromSpecificPlayer(void* _param) {
  TParam_ReadFromSpecificPlayer* param = (TParam_ReadFromSpecificPlayer*)_param;
  Player myPlayer = param->app->playerList[param->myPlayerIndex];

  // Open player private named pipe
  char fifoName_PlayerRead[STRING_LARGE];
  sprintf(fifoName_PlayerRead, "%s_%d", FIFO_PLAYER_TO_REFEREE,
          myPlayer.procId);

  if (DEBUG) {
    printf("[DEBUG] - Trying to open a named pipe named %s\n",
           fifoName_PlayerRead);
  }

  myPlayer.fdComm_Read = open(fifoName_PlayerRead, O_RDONLY);
  if (myPlayer.fdComm_Read == -1) {
    printf("[ERROR] Unexpected error on open()! Error: %d\n", errno);
    return (void*)EXIT_FAILURE;
  }
  if (DEBUG) {
    printf(
        "[DEBUG] - Named pipe belonging to %s has his write side open now... "
        "I "
        "can now read!\n",
        myPlayer.username);
  }

  // Start cycle and only stops when broken
  TossComm receivedComm;

  int readBytes;
  while (1) {
    readBytes = read(myPlayer.fdComm_Read, &receivedComm, sizeof(TossComm));
    if (readBytes == 0) {
      break;
    }
    switch (receivedComm.tossType) {
      case TCRT_PLAYER_INPUT:
        Service_PlayerInput(param->app, receivedComm.playerInput.procId,
                            receivedComm.playerInput.command);
        break;
      default:
        printf(
            "[ERROR] - Tried to handle an non set toss comm, marked as "
            "%d!\n",
            receivedComm.tossType);
        break;
    }

    if (DEBUG) {
      printf("[DEBUG] - Read %d bytes!\n", readBytes);
    }
  };

  if (DEBUG) {
    printf("[DEBUG] - Lost connection to player %s! Logging him out...\n",
           myPlayer.username);
  }

  Service_PlayerLogout(param->app, myPlayer.procId);

  free(param);
  return (void*)EXIT_SUCCESS;
}
#pragma endregion

#pragma region Each Player Game Handle
void* Thread_ReadFromGame(void* _param) {
  TParam_ReadFromGame* param = (TParam_ReadFromGame*)_param;
  Player* myPlayer = &param->app->playerList[param->myPlayerIndex];

  char buffer[STRING_LARGE];
  int readBytes;

  TossComm tossComm;
  tossComm.tossType = TCRT_GAME_OUTPUT;

  while (1) {
    // Reset buffer
    memset(buffer, '\0', STRING_LARGE);

    // Read from game
    readBytes = read(myPlayer->gameProc.fdReadFromGame,
                     tossComm.gameOutput.output, sizeof(tossComm.gameOutput));
    if (readBytes == 0) {
      break;
    }

    // Consume and ignore if Player <-> Game door is blocked
    if (myPlayer->gameProc.blockedComms) {
      if (DEBUG) {
        printf("[DEBUG] - Ignored game output! Game belong to Player [%s]!\n",
               myPlayer->username);
      }
      continue;
    }

    // Redirect to player
    int writtenBytes =
        write(myPlayer->fdComm_Write, &tossComm, sizeof(TossComm));
  };

  if (DEBUG) {
    printf("[DEBUG] - Got disconnected from %s game!\n", myPlayer->username);
  }
  free(param);
  return (void*)EXIT_SUCCESS;
}
#pragma endregion

#pragma region Championship Flow
void* Thread_ChampionshipFlow(void* _param) {
  TParam_ChampionshipFlow* param = (TParam_ChampionshipFlow*)_param;
  Application* app = param->app;

  bool canStart;
  struct timespec ts;

  // Enters endless cycle, being always ready to start a championship, if not
  // already ongoing
  while (true) {
    canStart = false;
    do {
#pragma region Wait until minimum players are logged in
      app->referee.isChampionshipClosed = false;

      if (DEBUG) {
        printf("[DEBUG] - Championship is ready to start!\n");
      }

      pthread_mutex_trylock(&app->mutStartChampionship);
      if (getQuantityPlayers(app) < DEFAULT_MINPLAYERS_START) {
        // Only unlocked when at least 2 players have joined
        pthread_mutex_lock(&app->mutStartChampionship);
      }

      // // Give a small time for the last player to join and connect properly
      // sleep(2);
#pragma endregion

#pragma region Wait for more players
      // Start a timer to wait for more players
      if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        printf("[ERROR] - Clock getTime failed! Error: %d\n", perror);
        return (void*)EXIT_FAILURE;
      }
      ts.tv_sec += app->referee.waitingDuration;

      Service_BroadcastChampionshipState(app, CS_WAIT_PLAYERS);
      printf("[INFO] - Waiting for players for %d seconds\n",
             app->referee.waitingDuration);
      pthread_mutex_timedlock(&app->mutCountdown, &ts);
      app->referee.isChampionshipClosed = true;
      printf("[INFO] - Lobby has closed! Currently active players: %d\n",
             getQuantityPlayers(app));
#pragma endregion

#pragma region Check if can really start
      // After the timer ends, check if at least 2 players are logged in, if
      // not restart championship to initial state
      canStart = getQuantityPlayers(app) >= DEFAULT_MINPLAYERS_START;

      if (!canStart) {
        Service_BroadcastChampionshipState(app, CS_LACK_PLAYERS);
      }
#pragma endregion
    } while (!canStart);

#pragma region Open game for each player
    for (int i = 0; i < app->referee.maxPlayers; i++) {
      if (app->playerList[i].active) {
        Service_OpenGame(app, app->playerList[i].procId);
      }
    }
#pragma endregion

#pragma region Wait for championship duration to end
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
      printf("[ERROR] - Clock getTime failed! Error: %d\n", perror);
      return (void*)EXIT_FAILURE;
    }
    ts.tv_sec += app->referee.championshipDuration;

    Service_BroadcastChampionshipState(app, CS_STARTED);
    printf("[INFO] - Championship has started! Ending in %d seconds...\n",
           app->referee.championshipDuration);

    pthread_mutex_timedlock(&app->mutCountdown, &ts);
#pragma endregion

#pragma region Check if finished or canceled by lack of players

    // TRUE: still has minimum players logged in
    // FALSE: no longer has the minimum logged in
    if (getQuantityPlayers(app) >= DEFAULT_MINPLAYERS_START) {
      Service_BroadcastChampionshipState(app, CS_ENDED);
    } else {
      for (int i = 0; i < app->referee.maxPlayers; i++) {
        if (app->playerList[i].active) {
          Service_CloseGame(app, app->playerList[i].procId);
        }
      }
      Service_BroadcastChampionshipState(app, CS_LACK_PLAYERS_DURING);
      continue;
    }

    // // Give a bit of pressure while waiting for results
    // sleep(2);

    printf("[INFO] - Championship ended!\n");
#pragma endregion

#pragma region Close game for each player
    for (int i = 0; i < app->referee.maxPlayers; i++) {
      if (app->playerList[i].active) {
        Service_CloseGame(app, app->playerList[i].procId);
      }
    }
#pragma endregion

#pragma region Communicate who won
    Service_BroadcastChampionshipState(app, CS_WINNER);
#pragma endregion

#pragma region Communicate championship restart
    app->referee.championshipCount++;
    Service_BroadcastChampionshipState(app, CS_RESTARTING);
#pragma endregion
  }

  free(param);
  return (void*)EXIT_SUCCESS;
}
#pragma endregion