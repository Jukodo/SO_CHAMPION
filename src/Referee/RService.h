#pragma once

#include "Referee.h"

bool Setup_Application(Application *app, int argc, char **argv);
bool Setup_Variables(Application *app, int argc, char **argv);
bool Setup_AvailableGames(Application *app);
bool Setup_NamedPipes(Application *app);
bool Setup_Threads(Application *app);

bool isValid_ChampionshipDuration(Application *app, int value);
bool isValid_WaitingDuration(Application *app, int value);

PlayerLoginResponseType Service_PlayerLogin(Application *app, int procId,
                                            char *username);
PlayerInputResponse Service_PlayerInput(Application *app, int procId,
                                        char *command);
PlayerInputResponse Service_HandlePlayerCommand(Application *app, int procId,
                                                char *command);
void Service_HandleSelfCommand(Application *app, char *command);
bool Service_KickPlayer(Application *app, char *username);
void Service_Exit(Application *app);
void Service_OpenGame(Application *app, int playerProcId);

int getRandomGameIndex(Application *app);
int getPlayerListEmptyIndex(Application *app);
int getPlayerIndexByProcId(Application *app, int procId);
int getPlayerIndexByUsername(Application *app, char *username);
void Clean_Player(Application *app, int procId);

void Print_Application(Application *app);
void Print_PlayerList(Application *app);
void Print_AvailableGameList(Application *app);