#pragma once

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT,
} DIRECTION;

char getch(void);

void* InputGetter(void* args);
