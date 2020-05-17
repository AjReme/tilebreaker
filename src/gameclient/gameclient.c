#include "gameclient/gameclient.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "common/macro.h"
#include "expect/expect.h"
#include "gameresponse/gameresponse.h"
#include "render/render.h"

char Getch(void) {
  static struct termios old, new;
  tcgetattr(0, &old);
  new = old;
  new.c_lflag &= ~ICANON;
  new.c_lflag &= ~ECHO;
  tcsetattr(0, TCSANOW, &new);
  char ch = getchar();
  tcsetattr(0, TCSANOW, &old);
  return ch;
}

void* InputGetter(void* args) {
  DIRECTION volatile* dir = (DIRECTION volatile*)args;
  fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
  while (1) {
    switch (Getch()) {
      case 'w':
      case 'W': {
        if (*dir != DOWN) {
          *dir = UP;
        }
        break;
      }
      case 's':
      case 'S': {
        if (*dir != UP) {
          *dir = DOWN;
        }
        break;
      }
      case 'd':
      case 'D': {
        if (*dir != LEFT) {
          *dir = RIGHT;
        }
        break;
      }
      case 'a':
      case 'A': {
        if (*dir != RIGHT) {
          *dir = LEFT;
        }
        break;
      }
      default: {
        break;
      }
    }
    usleep(20000);
  }
  return NULL;
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

void Connect(char* ip, int port) {
  RAII(AddressDestroy) Address addr;
  RAII(ClientDestroy) Client client;
  RAII(ResponseDestroy) Response response;
  EXPECT(AddressInit(&addr, ip, port));
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
