#include <iostream>

#include <context.hpp>

int main() {
  for (std::string line; std::getline(std::cin, line);) {
    btdb::sql::ParserContext ctx(line);
    ctx.Parse();
    for (const auto& it : ctx.result.select_list) {
      std::cout << it << " ";
    }
    std::cout << std::endl;
    std::cout << ctx.result.table_name << std::endl;
  }
}
