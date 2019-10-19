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
    if (std::holds_alternative<btdb::sql::SelectStmt>(ctx.stmt)) {
      const auto& select_stmt = std::get<btdb::sql::SelectStmt>(ctx.stmt);
      for (const auto& it : select_stmt.select_list) {
        std::cout << it << " ";
      }
      std::cout << std::endl;
      std::cout << select_stmt.table_name << std::endl;
      if (select_stmt.where_clause) {
        btdb::sql::PrintParseTreeVisitor pp(select_stmt.where_clause->expr);
        std::cout << pp.PrettyPrint() << std::endl;
      }
    }
  }
}
