#include "Utils/Utils.h"

typedef struct Player Player;

typedef struct Application
    Application;  // This struct will keep all information that this application
                  // needs

struct Player {
  char username[STRING_MEDIUM];
};

struct Application {
  Player player;
};