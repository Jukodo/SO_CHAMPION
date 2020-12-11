#pragma once

#include "../Utils/CCommunication.h"
#include "Player.h"

bool Setup_Application(Application *app);
bool Setup_NamedPipes(Application *app);
bool Setup_Threads(Application *app);

void Print_Application(Application *app);