#pragma once
#include "Utils.h"

#define FIFO_REFEREE "../tmp/fifo_qna_referee"
#define FIFO_PLAYER "../tmp/fifo_toss_player"
#define FIFO_PLAYER_TO_REFEREE "../tmp/fifo_p2r"

#pragma region NamedPipe - Structs / Enums Prototypes
// QnARequest Requests Variants
typedef struct PlayerLoginRequest PlayerLoginRequest;
typedef struct PlayerInputRequest PlayerInputRequest;

// QnARequest Request Types Enums
typedef enum QnARequestType QnARequestType;

// TossComm Variants (QnA Responses and independant comms)
typedef enum PlayerLoginResponseType PlayerLoginResponseType;
typedef struct PlayerLoginResponse PlayerLoginResponse;

typedef enum PlayerInputResponseType PlayerInputResponseType;
typedef struct PlayerInputResponse PlayerInputResponse;

typedef struct GameOutput GameOutput;
typedef struct ChampionshipMsg ChampionshipMsg;

// TossComm Types Enums
typedef enum TossCommType TossCommType;

// Core Structs
typedef struct QnARequest QnARequest;
typedef struct TossComm TossComm;
#pragma endregion
#pragma region NamedPipe - Structs / Enums Definement
// QnARequest Requests Variants
struct PlayerLoginRequest {
  char username[STRING_LARGE];
  int procId;
};
struct PlayerInputRequest {
  char command[STRING_LARGE];
  int procId;
};

// QnARequest Request Types Enums
enum QnARequestType {  // Types of QnA requests
  QnART_LOGIN,         // Player login
  QnART_INPUT,         // Player input
};

// TossComm Variants (QnA Responses and independant comms)
enum PlayerLoginResponseType {
  PLR_SUCCESS,
  PLR_INVALID_UNDEFINED,
  PLR_INVALID_CLOSED,
  PLR_INVALID_FULL,
  PLR_INVALID_EXISTS
};
struct PlayerLoginResponse {
  PlayerLoginResponseType playerLoginResponseType;
};

enum PlayerInputResponseType {
  PIR_INVALID,
  PIR_INPUT,
  PIR_GAMENAME,
  PIR_SHUTDOWN
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
// TossComm Types Enums
enum TossCommType {
  TCRT_LOGIN_RESP,
  TCRT_INPUT_RESP,
  TCRT_GAME_OUTPUT,
  TCRT_CHAMPIONSHIP_MSG
};

// Core Structs
struct QnARequest {
  union {
    PlayerLoginRequest playerLoginRequest;
    PlayerInputRequest playerInputRequest;
  };
  QnARequestType requestType;
};
struct TossComm {
  union {
    PlayerLoginResponse playerLoginResponse;
    PlayerInputResponse playerInputResponse;
    GameOutput gameOutput;
    ChampionshipMsg championshipMsg;
  };
  TossCommType tossType;
};

#pragma endregion