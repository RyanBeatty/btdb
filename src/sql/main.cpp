#include <iostream>

#include <context.hpp>

int main() {
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
    }
  }
}
