#include "utils/utils.h"

#define DEFAULT_GAMEDIR "~/Documents/SO/SO_CHAMPION/Games/"
#define DEFAULT_MAXPLAYER 10

#define MIN_CHAMP_DURATION 60000
#define MAX_CHAMP_DURATION 600000

#define MIN_WAITING_DURATION 30000
#define MAX_WAITING_DURATION 120000

#define MAX_MAXPLAYER 30

typedef struct Referee Referee;

typedef struct Application Application; //This struct will keep all information that this application needs

struct Referee{
  char gameDir[STRING_MEDIUM];
  int maxPlayers;

  int championshipDuration;
  int waitingDuration;
};

struct Application{
  Referee referee;
};