#pragma once

#include "Referee.h"

bool Setup_Application(Application *app, int argc, char **argv);
bool Setup_Variables(Application *app, int argc, char **argv);
bool Setup_AvailableGames(Application *app);
bool Setup_NamedPipes(Application *app);
bool Setup_Threads(Application *app);

bool isValid_ChampionshipDuration(Application *app, int value);
bool isValid_WaitingDuration(Application *app, int value);

bool Service_PlayerLogin(Application *app, int procId, char *username);
PlayerInputResponse Service_PlayerInput(Application *app, int procId,
                                        char *command);
PlayerInputResponse Service_HandleCommand(Application *app, int procId,
                                          char *command);
int getPlayerListEmptyIndex(Application *app);
int getPlayerIndex(Application *app, int procId);

void Print_Application(Application *app);