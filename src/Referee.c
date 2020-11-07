#include <stdio.h>
#include <stdlib.h>

#include "utils/utils.h"

void printVariables(int argc, char **argv){
  printf("Argument variables:\n");
  //argv[0] is already the executable path
  for(int i = 0; i < argc; i++){
    printf("\t[%d] - %s\n", i, argv[i]);
  }

  printf("\nEnvironment variables:\n");
  printf("\t[GAMEDIR] - %s\n", getenv("GAMEDIR"));
}

int main(int argc, char **argv){
  printVariables(argc, argv);

  printf(getenv("GAMEDIR"));

  return false;
}