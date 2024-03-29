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
void Service_PlayerLogout(Application *app, int procId);
void Service_PlayerInput(Application *app, int procId, char *command);
PlayerInputResponse Service_HandlePlayerCommand(Application *app, int procId,
                                                char *command);
void Service_HandleSelfCommand(Application *app, char *command);
bool Service_KickPlayer(Application *app, char *username);
bool Service_SetPlayerAndGameDoor(Application *app, char *username,
                                  bool newStatus);
void Service_Exit(Application *app);
void Service_OpenGame(Application *app, int playerProcId);
void Service_CloseGame(Application *app, int playerProcId);
void Service_BroadcastChampionshipState(Application *app,
                                        ChampionshipState state);
void Service_SendTossComm(Application *app, int procId, TossComm *tossComm);

int getRandomGameIndex(Application *app);
int getPlayerListEmptyIndex(Application *app);
int getPlayerIndexByProcId(Application *app, int procId);
int getPlayerIndexByUsername(Application *app, char *username);
int getQuantityPlayers(Application *app);
void Clean_Player(Application *app, int procId);

void Print_Application(Application *app);
void Print_PlayerList(Application *app);
void Print_AvailableGameList(Application *app);