#include "gamestate/field.h"

#include "networking/packet.h"
#include "render/input.h"

#define MAX_PLAYERS 6
#define FIELD_ROWS 30
#define FIELD_COLS 100

void FieldInit(Field* field) {
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    field->players[i] = (Player){-1, -1};
  }
  for (int i = 0; i < FIELD_ROWS; ++i) {
    for (int j = 0; j < FIELD_COLS; ++j) {
      field->states[i][j] = (State){-1, -1};
    }
  }
}

void FieldDestroy(Field* field) {
}

void FieldInitPlayers(Field* field) {
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        int a = field->players[i].x + dx;
        int b = field->players[i].y + dy;
        if (0 <= a && a < FIELD_ROWS && 0 <= b && b < FIELD_COLS) {
          field->states[a][b] = (State){i, -1};
        }
      }
    }
  }
}

static void FieldDeletePlayer(Field* field, int id) {
  field->players[id] = (Player){-1, -1};
  for (int i = 0; i < FIELD_ROWS; ++i) {
    for (int j = 0; j < FIELD_COLS; ++j) {
      if (field->states[i][j].owner == id) {
        field->states[i][j].owner = -1;
      } else if (field->states[i][j].captured_by == id) {
        field->states[i][j].captured_by = -1;
      }
    }
  }
}

static void FieldCaptureByPlayer(Field* field, int id) {
  for (int i = 0; i < FIELD_ROWS; ++i) {
    for (int j = 0; j < FIELD_COLS; ++j) {
      if (field->states[i][j].captured_by == id) {
        field->states[i][j].owner = id;
        field->states[i][j].captured_by = -1;
      }
    }
  }
}

static void FieldMovePlayer(Field* field, int x, int y, int id) {
  if (field->states[x][y].captured_by != -1) {
    int target = field->states[x][y].captured_by;
    FieldDeletePlayer(field, target);
    if (target == id) {
      return;
    }
  }
  field->players[id] = (Player){x, y};
  field->states[x][y].captured_by = id;
  if (field->states[x][y].owner == id) {
    FieldCaptureByPlayer(field, id);
  }
}

void FieldAssignUpdate(Field* field, DIRECTION* dirs) {
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    if (field->players[i].x == -1 || field->players[i].y == -1) {
      continue;
    }
    int x = field->players[i].x;
    int y = field->players[i].y;
    switch (dirs[i]) {
      case UP: {
        x--;
        break;
      }
      case DOWN: {
        x++;
        break;
      }
      case LEFT: {
        y--;
        break;
      }
      case RIGHT: {
        y++;
        break;
      }
      case NO_DIRECTION: {
        break;
      }
    }
    if (!(0 <= x && x < FIELD_ROWS && 0 <= y && y < FIELD_COLS)) {
      FieldDeletePlayer(field, i);
    } else {
      FieldMovePlayer(field, x, y, i);
    }
  }
}
