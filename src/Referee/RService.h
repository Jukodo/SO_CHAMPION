#pragma once

#include "Referee.h"

bool Setup_Application(Application *app, int argc, char **argv);
bool Setup_Variables(Application *app, int argc, char **argv);
bool Setup_AvailableGames(Application *app);
bool Setup_NamedPipes(Application *app);
bool Setup_Threads(Application *app);

bool isValid_ChampionshipDuration(Application *app, int value);
bool isValid_WaitingDuration(Application *app, int value);

void Print_Application(Application *app);