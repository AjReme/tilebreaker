#include "render/render.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "gamestate/gamestate.h"

static char* GetColor(int id) {
  switch (id) {
    case 0: {
      return ANSI_COLOR_RED;
    }
    case 1: {
      return ANSI_COLOR_GREEN;
    }
    case 2: {
      return ANSI_COLOR_YELLOW;
    }
    case 3: {
      return ANSI_COLOR_BLUE;
    }
    case 4: {
      return ANSI_COLOR_MAGENTA;
    }
    case 5: {
      return ANSI_COLOR_CYAN;
    }
    case -1:
    default: {
      return ANSI_COLOR_RESET;
    }
  }
}

void ConsoleOutputInit(ConsoleOutput* out) {
  for (int i = 0; i < CONSOLE_ROWS; ++i) {
    for (int j = 0; j < CONSOLE_COLS; ++j) {
      strcpy(out->table[i][j].color, ANSI_COLOR_RESET);
      strcpy(out->table[i][j].symbol, PLAYER_UNKNOWN);
    }
  }
}

void ConsoleOutputDestroy(ConsoleOutput* out) {
}

void ConsoleOutputClear(ConsoleOutput* out) {
  ConsoleOutputInit(out);
}

void ConsoleOutputSetColor(ConsoleOutput* out, int row, int col, int color_id) {
  strcpy(out->table[row][col].color, GetColor(color_id));
}

void ConsoleOutputSetSymbol(ConsoleOutput* out, int row, int col,
                            char* symbol) {
  strcpy(out->table[row][col].symbol, symbol);
}

void ConsoleOutputShow(ConsoleOutput* out) {
  printf("\e[1;1H\e[2J");
  for (int i = 0; i < CONSOLE_ROWS; ++i) {
    for (int j = 0; j < CONSOLE_COLS; ++j) {
      printf("%s", out->table[i][j].color);
      printf("%s", out->table[i][j].symbol);
      printf("%s", ANSI_COLOR_RESET);
    }
    puts("");
  }
}

void ConsoleOutputFieldDraw(ConsoleOutput* out, Field* field) {
  int xlim_min = field->players[field->my_id].x - CONSOLE_ROWS / 2;
  int ylim_min = field->players[field->my_id].y - CONSOLE_COLS / 2;
  int xlim_max = xlim_min + CONSOLE_ROWS;
  int ylim_max = ylim_min + CONSOLE_COLS;
  if (xlim_min < 0) {
    xlim_min = 0;
    xlim_max = CONSOLE_ROWS;
  }
  if (xlim_max > FIELD_ROWS) {
    xlim_max = FIELD_ROWS;
    xlim_min = xlim_max - CONSOLE_ROWS;
  }
  if (ylim_min < 0) {
    ylim_min = 0;
    ylim_max = CONSOLE_COLS;
  }
  if (ylim_max > FIELD_COLS) {
    ylim_max = FIELD_COLS;
    ylim_min = ylim_max - CONSOLE_COLS;
  }
  for (int i = xlim_min; i < xlim_max; ++i) {
    for (int j = ylim_min; j < ylim_max; ++j) {
      int a = i - xlim_min;
      int b = j - ylim_min;
      if (field->states[i][j].captured_by != -1) {
        ConsoleOutputSetSymbol(out, a, b, PLAYER_CAPTURING);
        ConsoleOutputSetColor(out, a, b, field->states[i][j].captured_by);
      } else if (field->states[i][j].owner != -1) {
        ConsoleOutputSetSymbol(out, a, b, PLAYER_CAPTURED);
        ConsoleOutputSetColor(out, a, b, field->states[i][j].owner);
      } else {
        ConsoleOutputSetSymbol(out, a, b, PLAYER_UNKNOWN);
        ConsoleOutputSetColor(out, a, b, -1);
      }
    }
  }
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    if (xlim_min <= field->players[i].x && field->players[i].x < xlim_max &&
        ylim_min <= field->players[i].y && field->players[i].y < ylim_max) {
      ConsoleOutputSetSymbol(out, field->players[i].x - xlim_min,
                             field->players[i].y - ylim_min, PLAYER_SYMBOL);
    }
  }
}
