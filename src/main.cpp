#include "parser.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>

int main() {
  fprintf(stderr, "ready> ");
  get_next_token();

  main_loop();

  return 0;
}
