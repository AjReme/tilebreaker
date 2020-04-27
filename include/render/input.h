#pragma once

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  NO_DIRECTION,
} DIRECTION;

char getch(void);

void* InputGetter(void* args);
