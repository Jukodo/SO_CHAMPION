#pragma once
#include "Referee.h"
#include "Utils.h"

#pragma region Shared Memory - Structs / Enums Prototypes
// QnARequest Requests Variants
typedef struct PlayerLoginRequest PlayerLoginRequest;

// QnARequest Responses Variants
typedef struct PlayerLoginResponse PlayerLoginResponse;
typedef enum PlayerLoginResponseType PlayerLoginResponseType;

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
struct PlayerLoginResponse {
  PlayerLoginResponseType taxiLoginResponseType;
};
enum PlayerLoginResponseType {
  PLR_SUCCESS,
  PLR_INVALID_UNDEFINED,
  PLR_INVALID_CLOSED,
  PLR_INVALID_EXISTS
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