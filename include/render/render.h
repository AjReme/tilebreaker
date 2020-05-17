#pragma once

#include <stdint.h>

#include "gamestate/gamestate.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define CONSOLE_ROWS 23
#define CONSOLE_COLS 78

#define PLAYER_UNKNOWN "░"
#define PLAYER_CAPTURING "▒"
#define PLAYER_CAPTURED "▓"
#define PLAYER_SYMBOL "▉"

typedef struct {
  char color[6];
  char symbol[6];
} Symbol;

typedef struct {
  Symbol table[CONSOLE_ROWS][CONSOLE_COLS];
} ConsoleOutput;

void ConsoleOutputInit(ConsoleOutput* out);

void ConsoleOutputDestroy(ConsoleOutput* out);

void ConsoleOutputClear(ConsoleOutput* out);

void ConsoleOutputSetColor(ConsoleOutput* out, int col, int row, int color_id);

void ConsoleOutputSetSymbol(ConsoleOutput* out, int col, int row, char* symbol);

void ConsoleOutputShow(ConsoleOutput* out);

void ConsoleOutputFieldDraw(ConsoleOutput* out, Field* field);
