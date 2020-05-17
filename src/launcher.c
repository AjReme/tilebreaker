#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client/client.h"
#include "common/macro.h"
#include "render/console.h"
#include "render/input.h"
#include "server/server.h"

void INTERNAL(RETCODE code, int line) {
  if (code != SUCCESS) {
    fprintf(stderr, "Errored in line %d: %d\n", line, code);
    if (errno) {
      perror("System error");
    }
    fflush(stderr);
    exit(1);
  }
}

#define EXPECT(func) INTERNAL(func, __LINE__)

static const char create_command[] = "create";
static const char connect_command[] = "connect";
static const char help_paragraph[] =
    "Usage: %s COMMAND [ip] [port]\n"
    "\n"
    "A console competitive game.\n"
    "\n"
    "Commands:\n"
    "  help     Show this help and exit.\n"
    "  create   Creates game on specified ip and port.\n"
    "  connect  Connects to the existing game via ip and port.\n";

void Help(char* name) {
  fprintf(stderr, help_paragraph, name);
}

struct timeval t1, t2;
double elapsed_time;

void SetTimer() {
  gettimeofday(&t1, NULL);
}

double GetTimer() {
  gettimeofday(&t2, NULL);
  elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;
  elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;
  return elapsed_time;
}

typedef enum {
  LOBBY_CONNECT,
  LOBBY_ACCEPT,
  LOBBY_REFUSE,
  SERVER_START,
  SERVER_PACKET,
  CLIENT_PACKET,
} RESPONSE_TYPES;

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

RESPONSE_TYPES UnpackType(Response* response) {
  return (RESPONSE_TYPES)response->data.ptr[0];
}

void Create(char* ip, char* port) {
  RAII(AddressDestroy) Address addr;
  RAII(ServerDestroy) Server server;
  RAII(ResponseDestroy) Response response;
  AddressInit(&addr, ip, atoi(port));
  ServerInit(&server, &addr);
  ServerSetTimeout(&server, 100);
  ResponseInit(&response);

  puts("Lobby created.");
  fflush(stdout);

  SetTimer();
  int players_count = 0;
  while (GetTimer() < 10000) {
    if (ServerReceive(&server, &response) == SUCCESS) {
      players_count++;
      PackLobbyAccept(&response, response.client_id);
      ServerSendTo(&server, &response);
      printf("Player %d connected.\n", response.client_id);
      fflush(stdout);
    }
  }

  srand(time(NULL));
  Player players[MAX_PLAYERS];
  memset(players, -1, MAX_PLAYERS * sizeof(Player));
  for (int i = 0; i < players_count; ++i) {
    while (1) {
      players[i] = (Player){rand() % FIELD_ROWS, rand() % FIELD_COLS};
      int good = 1;
      for (int j = 0; j < i; ++j) {
        if (abs(players[i].x - players[j].x) < 5 &&
            abs(players[i].y - players[j].y) < 5) {
          good = 0;
          break;
        }
      }
      if (good) {
        break;
      }
    }
    printf("Player %d coordinates: %d %d.\n", i, players[i].x, players[i].y);
  }

  PackServerStart(&response, players);
  ServerSend(&server, &response);
  DIRECTION dirs[MAX_PLAYERS];
  memset(dirs, NO_DIRECTION, MAX_PLAYERS * sizeof(DIRECTION));

  puts("Game started.");
  fflush(stdout);

  while (1) {
    SetTimer();
    while (GetTimer() <= 150) {
      if (ServerReceive(&server, &response) == SUCCESS) {
        if (UnpackType(&response) != CLIENT_PACKET) {
          continue;
        }
        UnpackClientPacket(&response, &dirs[response.client_id]);
      }
    }
    PackServerPacket(&response, dirs);
    ServerSend(&server, &response);
  }
}

void Play(Client* client, int id) {
  RAII(FieldDestroy) Field field;
  RAII(ConsoleOutputDestroy) ConsoleOutput out;
  RAII(ResponseDestroy) Response response;
  FieldInit(&field);
  ConsoleOutputInit(&out);
  EXPECT(ResponseInit(&response));
  field.my_id = id;

  volatile DIRECTION dir = NO_DIRECTION;
  pthread_t thread;
  pthread_create(&thread, NULL, InputGetter, &dir);

  DIRECTION dirs[MAX_PLAYERS];

  EXPECT(ClientReceive(client, &response));
  UnpackServerStart(&response, &field.players);
  FieldInitPlayers(&field);

  while (1) {
    PackClientPacket(&response, dir);
    EXPECT(ClientSend(client, &response));
    EXPECT(ClientReceive(client, &response));
    UnpackServerPacket(&response, dirs);
    FieldAssignUpdate(&field, dirs);
    if (field.players[id].x == -1 || field.players[id].y == -1) {
      return;
    }
    ConsoleOutputFieldDraw(&out, &field);
    ConsoleOutputShow(&out);
  }
}

void Connect(char* ip, char* port) {
  RAII(AddressDestroy) Address addr;
  RAII(ClientDestroy) Client client;
  RAII(ResponseDestroy) Response response;
  EXPECT(AddressInit(&addr, ip, atoi(port)));
  EXPECT(ClientInit(&client, &addr));
  EXPECT(ResponseInit(&response));
  PackLobbyConnect(&response);
  EXPECT(ClientSend(&client, &response));
  EXPECT(ClientReceive(&client, &response));
  switch (UnpackType(&response)) {
    case LOBBY_ACCEPT: {
      int id;
      UnpackLobbyAccept(&response, &id);
      Play(&client, id);
      break;
    }
    case LOBBY_REFUSE: {
      fputs("Connection refused.", stderr);
      exit(1);
    }
    default: {
      fputs("Unrecognized packet.", stderr);
      exit(1);
    }
  }
}

int main(int argc, char** argv) {
  if (argc == 4) {
    if (strcmp(argv[1], create_command) == 0) {
      Create(argv[2], argv[3]);
    } else if (strcmp(argv[1], connect_command) == 0) {
      Connect(argv[2], argv[3]);
    } else {
      Help(argv[0]);
    }
  } else {
    Help(argv[0]);
  }
}
