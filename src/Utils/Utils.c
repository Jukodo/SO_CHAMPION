#include "Utils.h"

void Utils_CleanStdin() {
  int c;
  while ((c = fgetc(stdin)) != '\n' && c != EOF)
    ;
}

void Utils_CleanString(char* str) {
  if (str[strlen(str) - 1] == '\n') {
    str[strlen(str) - 1] = '\0';
  }
}

bool Utils_StringIsNumber(char* str) {
  if (Utils_StringIsEmpty(str)) return false;

  for (unsigned int i = 0; i < strlen(str); i++) {
    if (!isdigit(str[i])) return false;
  }

  return true;
}

bool Utils_StringIsEmpty(char* str) {
  if (str == null || str[0] == '\0' || str[0] == '\n' || strlen(str) == 0)
    return true;

  return false;
}

void Utils_GenerateNewRandSeed() { srand((unsigned int)time(null)); }

int Utils_GetRandomNumber(int min, int max) {
  return (rand() % max - min) + min;
}