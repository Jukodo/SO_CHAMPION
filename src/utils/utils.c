#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void Utils_CleanStdin(){
  int c;
  while((c = fgetc(stdin)) != '\n' && c != EOF);
}