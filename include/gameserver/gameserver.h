#pragma once

#include "gamestate/field.h"
#include "server/server.h"

typedef struct {
  Field field;
  Server resolver;
} GameServer;

void GameServerInit(GameServer* server, char* ip, char* port);

void GameServerDestroy(GameServer* server);

void GameServerServe(GameServer* server);
