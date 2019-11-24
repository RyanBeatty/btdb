#include <iostream>

#include "node.hpp"
#include "sql/context.hpp"

int main() {
  for (std::string line; std::getline(std::cin, line);) {
    btdb::sql::ParserContext ctx(line);
    auto result = ctx.Parse();
    if (result == 0) {
      btdb::PrintContext print_ctx = btdb::MakePrintContext();
      btdb::print_parse_node(ctx.tree, &print_ctx);
      btdb::free_parse_node(ctx.tree);
    }
  }
}
