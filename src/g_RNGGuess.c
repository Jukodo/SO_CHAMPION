#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "utils/utils.h"

#define GAME_TITLE "RNGGuess"

void handle_sigusr1(int sig){
  printf("Received signal: %d AKA SIGUSR1", sig);
}

void Setup_SIGUSR1(){
  struct sigaction sa = {0}; //{0} - The struct starts with 0's
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = &handle_sigusr1;
  sigaction(SIGUSR1, &sa, NULL);
}

void Print_Introduction(){
  printf("\nHello! Welcome to the %s!", GAME_TITLE);
  printf("\nThe game flow is: ");
  printf("\n\t - A random number will be generated from 1 to 9");
  printf("\n\t - You, the player, will have to guess it");
  printf("\n\t - Each correct guess will increase your score by 1");
  printf("\n\t - If you choose to play again, the number will change");
  printf("\n\t - The number only changes when you guess it");
}

int main(int argc, char **argv){
  Setup_SIGUSR1();

  sleep(5);
  kill(getpid(), SIGUSR1);

  Print_Introduction();
  
  char line[STRING_MEDIUM];
  
  printf("\n\tPlease press ENTER when you are ready to start!");
  getchar();

  bool flag_leave = true;
  int score = 0;

  while(true){
    srand(time(0));
    int number = (rand()%9)+1;
    printf("\nCurrent score: %d\n", score);
    while(true){
      printf("\n\t-> ");
      scanf("%[^\n]", line);
      Utils_CleanStdin();

      if(atoi(line) == number){
        score++;

        printf("\tYou got it!\n\tPlay again? (n/N to leave):");
        printf("\n\t-> ");
        
        scanf("%[^\n]", line);
        Utils_CleanStdin();
        
        flag_leave = strcmp(line, "n") == 0 || strcmp(line, "N") == 0;
        break;
      }else{
        printf("\tWrong...\n");
        if(abs(atoi(line) - number) <= 2){
          printf("\tPretty close though!\n");
        }
      }
    }

    if(flag_leave){
      break;
    }
  }

  printf("\n\nThank you for playing %s!", GAME_TITLE);
  printf("\n\tYour final score: %d", score);

  printf("\n\nPress ENTER when you are ready to leave...");
  getchar();

  return false;
}