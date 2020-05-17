#pragma once

#include "gamestate/gamestate.h"
#include "networking/packet.h"

typedef enum {
  LOBBY_CONNECT,
  LOBBY_ACCEPT,
  LOBBY_REFUSE,
  SERVER_START,
  SERVER_PACKET,
  CLIENT_PACKET,
} RESPONSE_TYPES;

void PackLobbyConnect(Response* response);

void PackLobbyAccept(Response* response, int id);

void UnpackLobbyAccept(Response* response, int* id);

void PackLobbyRefuse(Response* response);

void PackServerStart(Response* response, Player* players);

void UnpackServerStart(Response* response, Player* players);

void PackServerPacket(Response* response, DIRECTION* dirs);

void UnpackServerPacket(Response* response, DIRECTION* dirs);

void PackClientPacket(Response* response, DIRECTION dir);

void UnpackClientPacket(Response* response, DIRECTION* dir);

RESPONSE_TYPES
UnpackType(Response* response);
