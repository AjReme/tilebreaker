/*#include "gameserver/gameserver.h"

#include "server/server.h"

void GameServerInit(GameServer* server, char* ip, char* port) {
  Address addr;
  AddressInit(&addr, ip, port);
  ServerInit(&server->resolver, &addr);
  SocketMakeNonBlocking(&server->resolver);
  AddressDestroy(&addr);
}

void GameServerDestroy(GameServer* server) {
  ServerDestroy(&server->resolver);
}

static void GameServerHandle(GameServer* server, Response* response) {
  switch (ResponseGetType(&response)) {
    case CONNECT: {
      ServerSend();
    }
    case DISCONNECT: {
      fputs(stderr, "Unhandled disconnect!");
      exit(1);
    }
    default: {
      fputs(stderr, "Unhandled response type!");
      exit(1);
    }
  }
}

void GameServerServe(GameServer* server) {
  Response response;
  ResponseInit(&response);
  while (1) {
    if (ServerReceive(&server->resolver, &response) == SOCKET_TIMEOUT) {
      GameServerHandle(server, &response);
    }
    //ServerSend(&server->resolver, );
  }
  ResponseDestroy(&response);
}*/
