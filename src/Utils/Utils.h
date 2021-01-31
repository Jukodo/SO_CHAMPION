#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// bool define
typedef int bool;
#define true 1
#define false 0

#define STRING_SMALL 25
#define STRING_MEDIUM 50
#define STRING_LARGE 100
#define STRING_XL 255

void Utils_CleanStdin();
void Utils_CleanString(char* str);
bool Utils_StringIsNumber(char* str);
bool Utils_StringIsEmpty(char* str);
void Utils_GenerateNewRandSeed();
int Utils_GetRandomNumber(int min, int max);
float Utils_RoundFloat(float n, float i);