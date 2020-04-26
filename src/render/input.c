#include "render/input.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

char getch(void) {
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
    switch (getch()) {
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
