#include <stdio.h>

#include "node.h"
#include "sql/driver.h"

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