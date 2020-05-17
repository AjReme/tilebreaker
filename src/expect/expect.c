#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/retcode.h"

void EXPECT(RETCODE code) {
  if (code != SUCCESS) {
    fprintf(stderr, "Errored with GUDP RETCODE %d\n", code);
    if (errno != 0) {
      fprintf(stderr, "System error: %d\n", errno);
      perror("Error description");
    }
    fflush(stderr);
    exit(1);
  }
}
