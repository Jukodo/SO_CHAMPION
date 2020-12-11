#pragma once
#include "Utils.h"

#define FIFO_R2P "../tmp/fifo_qna_r2p"

#pragma region Shared Memory - Structs / Enums Prototypes
// QnARequest Requests Variants
typedef struct PlayerLoginRequest PlayerLoginRequest;

// QnARequest Responses Variants
typedef enum PlayerLoginResponseType PlayerLoginResponseType;
typedef struct PlayerLoginResponse PlayerLoginResponse;

// QnARequest Types Enums
typedef enum QnARequestType QnARequestType;

// Core Structs
typedef struct QnARequest QnARequest;
#pragma endregion

#pragma region Shared Memory - Structs / Enums Definement
// QnARequest Requests Variants
struct PlayerLoginRequest {
  char username[STRING_LARGE];
};

// QnARequest Responses Variants
enum PlayerLoginResponseType {
  PLR_SUCCESS,
  PLR_INVALID_UNDEFINED,
  PLR_INVALID_CLOSED,
  PLR_INVALID_EXISTS
};
struct PlayerLoginResponse {
  PlayerLoginResponseType playerLoginResponseType;
};

// QnARequest Types Enums
enum QnARequestType {  // Types of QnA requests
  QnART_LOGIN,         // Player login
};

// Core Structs
struct QnARequest {
  union {
    PlayerLoginRequest playerLoginRequest;
  };
  QnARequestType requestType;
  union {
    PlayerLoginResponse playerLoginResponse;
  };
};
#pragma endregion