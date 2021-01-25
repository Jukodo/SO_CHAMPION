#pragma once
#include "Utils.h"

#define FIFO_REFEREE_ENTRY "../tmp/fifo_referee_entry"
#define FIFO_PLAYER_TO_REFEREE "../tmp/fifo_p2r"
#define FIFO_REFEREE_TO_PLAYER "../tmp/fifo_r2p"

#pragma region NamedPipe - Structs / Enums Prototypes
// Responses Enums
typedef enum PlayerLoginResponseType PlayerLoginResponseType;
typedef enum PlayerInputResponseType PlayerInputResponseType;

// TossComm Variants
typedef struct PlayerInput PlayerInput;
typedef struct PlayerLoginResponse PlayerLoginResponse;
typedef struct PlayerInputResponse PlayerInputResponse;
typedef struct GameOutput GameOutput;
typedef struct ChampionshipMsg ChampionshipMsg;

// TossComm Core Types Enums
typedef enum TossCommType TossCommType;

// Core Structs (Most top level struct, not owned by anyone)
typedef struct EntryRequest EntryRequest;
typedef struct TossComm TossComm;
#pragma endregion
#pragma region NamedPipe - Structs / Enums Definement
// Responses Enums
enum PlayerLoginResponseType {
  PLR_SUCCESS,
  PLR_INVALID_UNDEFINED,
  PLR_INVALID_CLOSED,
  PLR_INVALID_FULL,
  PLR_INVALID_EXISTS
};
enum PlayerInputResponseType {
  PIR_INVALID,
  PIR_INPUT,
  PIR_GAMENAME,
  PIR_SHUTDOWN
};

// TossComm Variants
struct PlayerInput {
  char command[STRING_LARGE];
  int procId;
};
struct PlayerLoginResponse {
  PlayerLoginResponseType playerLoginResponseType;
};
struct PlayerInputResponse {
  PlayerInputResponseType playerInputResponseType;
  char gameName[STRING_LARGE];
};
struct GameOutput {
  char output[STRING_LARGE];
};
struct ChampionshipMsg {
  char msg[STRING_LARGE];
};

// TossComm Core Types Enums
enum TossCommType {
  TCRT_PLAYER_INPUT,
  TCRT_LOGIN_RESP,
  TCRT_INPUT_RESP,
  TCRT_GAME_OUTPUT,
  TCRT_CHAMPIONSHIP_MSG
};

// Core Structs
struct EntryRequest {
  char username[STRING_LARGE];
  int procId;
};
struct TossComm {
  union {
    PlayerInput playerInput;
    PlayerLoginResponse playerLoginResponse;
    PlayerInputResponse playerInputResponse;
    GameOutput gameOutput;
    ChampionshipMsg championshipMsg;
  };
  TossCommType tossType;
};

#pragma endregion