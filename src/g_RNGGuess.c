#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "utils/utils.h"

#define STR_MEDIUM 50

void introduction(){
  printf("\nHello! Welcome to the Number Guessing game...");
  printf("\nThe game flow is: ");
  printf("\n\t - A random number will be generated from 1 to 9");
  printf("\n\t - You, the player, will have to guess it");
  printf("\n\t - Each correct guess will increase your score by 1");
  printf("\n\t - If you choose to play again, the number will change");
  printf("\n\t - The number only changes when you guess it");
}

int main(int argc, char **argv){
  introduction();
  
  char line[STR_MEDIUM];
  
  printf("\n\tPlease press ENTER when you are ready to start!");
  getchar();

  bool flag_leave = true;
  int score = 0;

  while(true){
    srand(time(0));
    int number = (rand()%9)+1;
    printf("\nCurrent score: %d", score);
    while(true){
      printf("\nTry to guess:");
      printf("\t-> ");
      scanf("%[^\n]", line);
      Utils_CleanStdin();

      if(atoi(line) == number){
        score++;

        printf("\nYou got it! Play again? (n/N to leave):");
        printf("\t-> ");
        
        scanf("%[^\n]", line);
        Utils_CleanStdin();
        
        flag_leave = strcmp(line, "n") == 0 || strcmp(line, "N") == 0;
        break;
      }
    }

    if(flag_leave){
      break;
    }
  }

  printf("\n\nThank you for playing!");
  printf("\n\tYour score: %d", score);

  printf("\tPress ENTER when you are ready to leave...");
  getchar();

  return false;
}