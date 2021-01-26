#pragma once

#include "Player.h"

bool Setup_Application(Application *app);
bool Setup_NamedPipes(Application *app);
bool Setup_Threads(Application *app);

bool Service_Login(Application *app, char *username);
void Service_OpenPrivateWrite(Application *app);
bool Service_Input(Application *app, char *command);
void Service_SendTossComm(Application *app, TossComm *tossComm);

void Print_Application(Application *app);