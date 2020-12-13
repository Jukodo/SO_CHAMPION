#pragma once

#include "Player.h"

bool Setup_Application(Application *app);
bool Setup_NamedPipes(Application *app);
bool Setup_Threads(Application *app);

bool Service_Login(Application *app, char *username);
bool Service_Input(Application *app, char *command);

void Print_Application(Application *app);