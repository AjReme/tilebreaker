#pragma once

#include "render/input.h"

#define MAX_PLAYERS 6
#define FIELD_ROWS 30
#define FIELD_COLS 100

typedef struct {
  int owner;
  int captured_by;
} State;

typedef struct {
  int x;
  int y;
} Player;

typedef struct {
  int my_id;
  Player players[MAX_PLAYERS];
  State states[FIELD_ROWS][FIELD_COLS];
} Field;

void FieldInit(Field* field);

void FieldDestroy(Field* field);

void FieldInitPlayers(Field* field);

void FieldAssignUpdate(Field* field, DIRECTION* dirs);
