#include <stdio.h>

// Only define once.
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
// comment to prevent auto linting
#include "driver.h"

int main() {
  while (true) {
    char* line = NULL;
    size_t size = 0;
    if (getline(&line, &size, stdin) == -1) {
      printf("No line\n");
      break;
    } else {
      Parser* parser = InitParser(line);
      if (Parse(parser) == 0) {
        PrintContext print_ctx = MakePrintContext();
        print_parse_node(parser->tree, &print_ctx);
        free_parse_node(parser->tree);
      } else {
        printf("failed to parse\n");
      }
    }
  }
}
