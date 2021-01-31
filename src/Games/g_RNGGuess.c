#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../Utils/Utils.h"

#define GAME_TITLE "RNGGuess"

int score = 0;
void handle_sigusr1(int sig) { exit(score); }

void Setup_SIGUSR1() {
  struct sigaction sa = {0};  //{0} - The struct starts with 0's
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = &handle_sigusr1;
  sigaction(SIGUSR1, &sa, NULL);
}

void Print_Introduction() {
  char bufSTDOUT[STRING_LARGE];

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\nHello! Welcome to the %s!\n", GAME_TITLE);
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\tThe game flow is: \n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - A random number will be generated from 1 to 9\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - You, the player, will have to guess it\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - Each correct guess will increase your score by 1\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - The number only changes when you guess it\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - To leave press CTRL-C or type #quit\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
}

int main(int argc, char **argv) {
  char bufSTDOUT[STRING_LARGE];
  char bufSTDIN[STRING_LARGE];

  Setup_SIGUSR1();

  Print_Introduction();

  bool flag_leave = true;

  while (true) {
    srand(time(0));
    int number = (rand() % 9) + 1;

    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\n\tCurrent score: %d\n", score);
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    while (true) {
      memset(bufSTDOUT, '\0', STRING_LARGE);
      sprintf(bufSTDOUT, "\tPick a number: \n");
      write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

      memset(bufSTDIN, '\0', STRING_LARGE);
      read(STDIN_FILENO, bufSTDIN, STRING_LARGE);

      if (atoi(bufSTDIN) == number) {
        score++;

        memset(bufSTDOUT, '\0', STRING_LARGE);
        sprintf(bufSTDOUT, "\tYou got it! +1 score! Keep going...\n");
        write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

        break;
      } else {
        memset(bufSTDOUT, '\0', STRING_LARGE);
        sprintf(bufSTDOUT, "\tWrong...\n");
        write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

        if (abs(atoi(bufSTDIN) - number) <= 2) {
          memset(bufSTDOUT, '\0', STRING_LARGE);
          sprintf(bufSTDOUT, "\tPretty close though!\n");
          write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
        }
      }
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