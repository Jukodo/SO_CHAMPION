#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../Utils/Utils.h"

#define GAME_TITLE "ArithmeticBrain"

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
  sprintf(bufSTDOUT, "\t - A random arithmetic will be generated\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT,
          "\t - Every answer will be rounded UP TO 2 decimal places\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - You, the player, will have to solve it\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT,
          "\t - Each correct answer will increase your score by 1\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(
      bufSTDOUT,
      "\t - The arithmetic changes after you answer, either correct or not\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - To leave press CTRL-C or type #quit\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
}

char getRandomOperator() {
  int op = rand() % 4;
  switch (op) {
    case 0:
      return '*';
    case 1:
      return '/';
    case 2:
      return '+';
    case 3:
      return '-';
    default:
      return '+';
  }
}

float Solve_Arithmetic(char* arithmetic) {
  if (Utils_StringIsEmpty(arithmetic)) {
    return -1;
  }

  // Copy to a local variable, avoiding strtok to change the source
  char localArithmetic[STRING_MEDIUM];
  strncpy(localArithmetic, arithmetic, STRING_MEDIUM);

  char delimit[] = " ";
  int strtokIt = 0;
  char* separatedArithmetic[STRING_SMALL];
  separatedArithmetic[strtokIt] = strtok(localArithmetic, delimit);
  while (separatedArithmetic[strtokIt] != NULL) {
    strtokIt++;
    separatedArithmetic[strtokIt] = strtok(NULL, delimit);
  };
  strtokIt--;

  int op = '*';
  bool flagLeave = false;

  // * -> / -> + -> - -> FINISHED
  while (!flagLeave) {
    // Handle *
    for (int i = 0; i < strtokIt; i++) {
      if (Utils_StringIsEmpty(separatedArithmetic[i])) {
        break;
      }

      // Non-empty string & Valid operator (1 char) & *
      if (strlen(separatedArithmetic[i]) == 1 &&
          separatedArithmetic[i][0] == op) {
        // Invalid operator position (can't have any side isolated)
        if (i == 0 || i == strtokIt) {
          return -1;
        }

        switch (op) {
          case '*':
            snprintf(separatedArithmetic[i - 1], STRING_SMALL, "%.2f",
                     atof(separatedArithmetic[i - 1]) *
                         atof(separatedArithmetic[i + 1]));
            break;
          case '/':
            snprintf(separatedArithmetic[i - 1], STRING_SMALL, "%.2f",
                     atof(separatedArithmetic[i - 1]) /
                         atof(separatedArithmetic[i + 1]));
            break;
          case '+':
            snprintf(separatedArithmetic[i - 1], STRING_SMALL, "%.2f",
                     atof(separatedArithmetic[i - 1]) +
                         atof(separatedArithmetic[i + 1]));
            break;
          case '-':
            snprintf(separatedArithmetic[i - 1], STRING_SMALL, "%.2f",
                     atof(separatedArithmetic[i - 1]) -
                         atof(separatedArithmetic[i + 1]));
            break;
        }

        // Offset 2 times to left side
        for (int o = i; o < strtokIt; o++) {
          // Cleanup 2 most right side values
          if (strtokIt - 2 <= 0) {
            separatedArithmetic[o] = "\0";
          } else {
            separatedArithmetic[o] = separatedArithmetic[o + 2];
          }
        }

        // Found one * and reset to try to find another *
        i = -1;
      }
    }

    // Set next operation (or leave)
    switch (op) {
      case '*':
        op = '/';
        break;
      case '/':
        op = '+';
        break;
      case '+':
        op = '-';
        break;
      case '-':
        flagLeave = true;
        break;
    }
  }

  return atof(separatedArithmetic[0]);
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

    char arithmetic[STRING_MEDIUM];
    snprintf(arithmetic, STRING_MEDIUM, "%d %c %d %c %d",
             Utils_GetRandomNumber(1, 9), getRandomOperator(),
             Utils_GetRandomNumber(1, 9), getRandomOperator(),
             Utils_GetRandomNumber(1, 9));
    float correctAnswer = Utils_RoundFloat(Solve_Arithmetic(arithmetic), 2);

    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\tSolve for %s: \n", arithmetic);
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\tAnswer: \n");
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    memset(bufSTDIN, '\0', STRING_LARGE);
    read(STDIN_FILENO, bufSTDIN, STRING_LARGE);

    if (atof(bufSTDIN) == correctAnswer) {
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