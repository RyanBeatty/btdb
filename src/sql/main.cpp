#include <iostream>

#include "node.hpp"
#include "sql/context.hpp"

int main() {
  for (std::string line; std::getline(std::cin, line);) {
    ParserContext ctx(line);
    auto result = ctx.Parse();
    if (result == 0) {
      PrintContext print_ctx = MakePrintContext();
      print_parse_node(ctx.tree, &print_ctx);
      free_parse_node(ctx.tree);
    }
  }
}
