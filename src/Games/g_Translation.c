#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../Utils/Utils.h"

#define GAME_TITLE "Translation"

#define ENGLISH 0
#define PORTUGUESE 1

int score = 0;
void handle_sigusr1(int sig) { exit(score); }

void Setup_SIGUSR1() {
  struct sigaction sa = {0};  //{0} - The struct starts with 0's
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = &handle_sigusr1;
  sigaction(SIGUSR1, &sa, NULL);
}

char* translations[20][2] = {
    {
        "grandfather",
        "avo",
    },
    {
        "goodbye",
        "adeus",
    },
    {
        "yes",
        "sim",
    },
    {
        "daughter",
        "filha",
    },
    {
        "he",
        "ele",
    },
    {
        "west",
        "oeste",
    },
    {
        "street",
        "rua",
    },
    {
        "we",
        "nos",
    },
    {
        "napkin",
        "guardanapo",
    },
    {
        "eraser",
        "borracha",
    },
    {
        "fork",
        "garfo",
    },
    {
        "bag",
        "saco",
    },
    {
        "notebook",
        "caderno",
    },
    {
        "airport",
        "aeroporto",
    },
    {
        "husband",
        "marido",
    },
    {
        "paper",
        "papel",
    },
    {
        "morning",
        "manha",
    },
    {
        "evening",
        "tarde",
    },
    {
        "aeroplane",
        "aviao",
    },
    {
        "avenue",
        "avenida",
    },
};

void Print_Introduction() {
  char bufSTDOUT[STRING_LARGE];

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\nHello! Welcome to the %s!\n", GAME_TITLE);
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\tThe game flow is: \n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - A random word will be picked\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - This word can be either in English or Portuguese\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT,
          "\t - You, the player, will have to write the translation to the "
          "opposite language\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT,
          "\t - Each correct answer will increase your score by 1\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(
      bufSTDOUT,
      "\t - The question changes after you answer, either correct or not\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - To leave press CTRL-C or type #quit\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
}

int main(int argc, char** argv) {
  Utils_GenerateNewRandSeed();

  char bufSTDOUT[STRING_LARGE];
  char bufSTDIN[STRING_LARGE];

  Setup_SIGUSR1();

  Print_Introduction();

  bool flag_leave = true;

  while (true) {
    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\n\tCurrent score: %d\n", score);
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    int translationIndex = Utils_GetRandomNumber(0, 19);
    int sourceLanguage = Utils_GetRandomNumber(0, 1);
    printf("%d\n", sourceLanguage);

    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\tTranslate \"%s\" to %s: \n",
            translations[translationIndex][sourceLanguage],
            sourceLanguage == ENGLISH ? "Portuguese" : "English");
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\tTranslated: %s\n",
            translations[translationIndex][!sourceLanguage]);
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\tTranslation: \n");
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    memset(bufSTDIN, '\0', STRING_LARGE);
    read(STDIN_FILENO, bufSTDIN, STRING_LARGE);

    printf("%s\n", bufSTDIN);
    printf("%s\n", translations[translationIndex][!sourceLanguage]);

    Utils_CleanString(bufSTDIN);
    if (strcmp(bufSTDIN, translations[translationIndex][!sourceLanguage]) ==
        0) {
      score++;

      memset(bufSTDOUT, '\0', STRING_LARGE);
      sprintf(bufSTDOUT, "\tYou got it! +1 score! Keep going...\n");
      write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
    } else {
      memset(bufSTDOUT, '\0', STRING_LARGE);
      sprintf(bufSTDOUT, "\tWrong...\n");
      write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
    }
  }

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\nThank you for playing %s!\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\tYour final score: %d\n", score);
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  return false;
}