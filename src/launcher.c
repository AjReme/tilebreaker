#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gameclient/gameclient.h"
#include "gameserver/gameserver.h"
#include "gamestate/gamestate.h"

static const char kCreateCommand[] = "create";
static const char kConnectCommand[] = "connect";
static const char kHelpParagraph[] =
    "A console competitive game.\n"
    "\n"
    "Commands:\n"
    "  help                      Show this help and exit.\n"
    "  create  [port] [players]  Creates game on port with specified players "
    "count.\n"
    "  connect [ip]   [port]     Connects to the existing game via ip and "
    "port.\n";

int IsIPValid(char* ip) {
  if (strcmp(ip, "localhost") == 0) {
    return 1;
  }
  struct sockaddr_in sa;
  return inet_pton(AF_INET, ip, &sa.sin_addr) != 0;
}

void Help(void) {
  fprintf(stderr, kHelpParagraph);
}

int main(int argc, char** argv) {
  if (argc == 4) {
    if (strcmp(argv[1], kCreateCommand) == 0) {
      int port = atoi(argv[2]);
      int max_players = atoi(argv[3]);
      if (!(1 <= port && port <= 65535)) {
        fprintf(stderr, "Expected port in range 1-65535, found %s!\n", argv[2]);
        fflush(stderr);
        exit(1);
      }
      if (!(1 <= max_players && max_players <= MAX_PLAYERS)) {
        fprintf(stderr, "Expected players count in range 1-6, found %s!\n",
                argv[3]);
        fflush(stderr);
        exit(1);
      }
      Create(port, max_players);
    } else if (strcmp(argv[1], kConnectCommand) == 0) {
      if (!IsIPValid(argv[2])) {
        fprintf(stderr, "IP \"%s\" is not valid IP!\n", argv[2]);
        fflush(stderr);
        exit(1);
      }
      int port = atoi(argv[3]);
      if (!(1 <= port && port <= 65535)) {
        fprintf(stderr, "Expected port in range 1-65535, found %s!\n", argv[2]);
        fflush(stderr);
        exit(1);
      }
      Connect(argv[2], port);
    } else {
      Help();
    }
  } else {
    Help();
  }
}
