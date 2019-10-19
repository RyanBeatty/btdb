#include <iostream>

#include "context.hpp"
#include "node.hpp"

int main() {
  // btdb::sql::NIdentifier id("foo");
  // btdb::sql::NExpr expr = id;
  // btdb::sql::PrintParseTreeVisitor pp(expr);
  // std::cout << pp.PrettyPrint() << std::endl;
  for (std::string line; std::getline(std::cin, line);) {
    btdb::sql::ParserContext ctx(line);
    ctx.Parse();
    btdb::sql::PrintContext print_ctx;
    btdb::sql::print_parse_node(ctx.tree->tree, print_ctx);
    std::cout << print_ctx.Print() << std::endl;
  }
}
