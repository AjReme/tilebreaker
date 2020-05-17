#include "gameresponse/gameresponse.h"

#include "networking/packet.h"

void PackLobbyConnect(Response* response) {
  response->data.ptr[0] = (char)LOBBY_CONNECT;
  response->data.len = 1;
}

void PackLobbyAccept(Response* response, int id) {
  response->data.ptr[0] = (char)LOBBY_ACCEPT;
  response->data.ptr[1] = (char)id;
  response->data.len = 3;
}

void UnpackLobbyAccept(Response* response, int* id) {
  *id = (int)response->data.ptr[1];
}

void PackLobbyRefuse(Response* response) {
  response->data.ptr[0] = (char)LOBBY_REFUSE;
  response->data.len = 1;
}

void PackServerStart(Response* response, Player* players) {
  response->data.ptr[0] = (char)SERVER_START;
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    memcpy(response->data.ptr + 1 + i * sizeof(Player), &players[i],
           sizeof(Player));
  }
  response->data.len = 1 + MAX_PLAYERS * sizeof(Player);
}

void UnpackServerStart(Response* response, Player* players) {
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    memcpy(&players[i], response->data.ptr + 1 + i * sizeof(Player),
           sizeof(Player));
  }
}

void PackServerPacket(Response* response, DIRECTION* dirs) {
  response->data.ptr[0] = (char)SERVER_PACKET;
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    response->data.ptr[i + 1] = (char)dirs[i];
  }
  response->data.len = 1 + MAX_PLAYERS;
}

void UnpackServerPacket(Response* response, DIRECTION* dirs) {
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    dirs[i] = (DIRECTION)response->data.ptr[1 + i];
  }
}

void PackClientPacket(Response* response, DIRECTION dir) {
  response->data.ptr[0] = (char)CLIENT_PACKET;
  response->data.ptr[1] = (char)dir;
  response->data.len = 2;
}

void UnpackClientPacket(Response* response, DIRECTION* dir) {
  *dir = (DIRECTION)response->data.ptr[1];
}

RESPONSE_TYPES
UnpackType(Response* response) {
  return (RESPONSE_TYPES)response->data.ptr[0];
}
