#include <iostream>

#include "context.hpp"
#include "node.hpp"

int main() {
  for (std::string line; std::getline(std::cin, line);) {
    btdb::sql::ParserContext ctx(line);
    auto result = ctx.Parse();
    if (result == 0) {
      btdb::sql::PrintContext print_ctx;
      btdb::sql::print_parse_node(ctx.tree->tree, print_ctx);
      std::cout << print_ctx.Print() << std::endl;
    }
  }
}
