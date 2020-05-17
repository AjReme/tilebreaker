#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "common/macro.h"
#include "expect/expect.h"
#include "gameresponse/gameresponse.h"
#include "server/server.h"

static struct timeval t1, t2;
static double elapsed_time;

static void SetTimer() {
  gettimeofday(&t1, NULL);
}

static double GetTimer() {
  gettimeofday(&t2, NULL);
  elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;
  elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;
  return elapsed_time;
}

void Create(int port, int max_players) {
  RAII(AddressDestroy) Address addr;
  RAII(ServerDestroy) Server server;
  RAII(ResponseDestroy) Response response;
  EXPECT(AddressInit(&addr, "localhost", port));
  EXPECT(ServerInit(&server, &addr));
  EXPECT(ServerSetTimeout(&server, 100));
  EXPECT(ResponseInit(&response));

  puts("Lobby created.");
  fflush(stdout);

  SetTimer();
  int players_count = 0;
  while (1) {
    if (players_count == max_players) {
      break;
    }
    if (ServerReceive(&server, &response) == SUCCESS) {
      players_count++;
      PackLobbyAccept(&response, response.client_id);
      EXPECT(ServerSendTo(&server, &response));
      printf("Player %d connected.\n", response.client_id);
      fflush(stdout);
    }
  }

  RAII(FieldDestroy) Field field;
  FieldInit(&field);
  srand(time(NULL));
  for (int i = 0; i < players_count; ++i) {
    while (1) {
      field.players[i] = (Player){rand() % FIELD_ROWS, rand() % FIELD_COLS};
      int good = 1;
      for (int j = 0; j < i; ++j) {
        if (abs(field.players[i].x - field.players[j].x) < 5 &&
            abs(field.players[i].y - field.players[j].y) < 5) {
          good = 0;
          break;
        }
      }
      if (good) {
        break;
      }
    }
    printf("Player %d coordinates: %d %d.\n", i, field.players[i].x,
           field.players[i].y);
  }
  FieldInitPlayers(&field);

  PackServerStart(&response, field.players);
  EXPECT(ServerSend(&server, &response));
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
    EXPECT(ServerSend(&server, &response));
    FieldAssignUpdate(&field, dirs);
    if (FieldAllDead(&field) == 1) {
      break;
    }
  }

  puts("Game finished.");
  fflush(stdout);
}
