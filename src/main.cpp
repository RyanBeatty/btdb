#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>

#include "sql/context.hpp"

using btdb::sql::RawStmt;

void Panic(const std::string& msg) {
  std::cerr << "Panic: " << msg << std::endl;
  exit(1);
}

struct SystemCatalog {};

struct SelectQuery {
  std::vector<std::string> target_list;
  std::vector<std::string> range_table;
};

typedef std::variant<SelectQuery> Query;

Query AnalyzeAndRewriteStmt(RawStmt& stmt) {
  Query query;
  switch (stmt.index()) {
    case 0: {
      const btdb::sql::SelectStmt select = std::get<btdb::sql::SelectStmt>(stmt);
      query = SelectQuery{select.select_list, std::vector<std::string>{select.table_name}};
      break;
    }
    default:
      Panic("Unknown Statement Type");
  }
  return query;
}

std::unique_ptr<std::vector<std::string>> execute_plan(RawStmt& stmt,
                                                       std::vector<std::string>& tuples) {
  if (std::holds_alternative<btdb::sql::SelectStmt>(stmt)) {
    auto& select_stmt = std::get<btdb::sql::SelectStmt>(stmt);
    auto results = std::make_unique<std::vector<std::string>>();
    for (const auto& tuple : tuples) {
      results->emplace_back(tuple);
    }
    return results;
  }
  return nullptr;
}

int main() {
  printf("Starting btdb\n");

  auto tuples = std::make_unique<std::vector<std::string>>();
  tuples->emplace_back("hello");
  tuples->emplace_back("world");
  while (true) {
    printf("btdb> ");
    std::string line;
    if (!std::getline(std::cin, line)) {
      break;
    }
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    if (line == "\\q") {
      break;
    }
    btdb::sql::ParserContext parser(line);
    if (parser.Parse() != 0) {
      continue;
    }
    auto stmt = parser.stmt;
    auto query = AnalyzeAndRewriteStmt(stmt);
    auto results = execute_plan(stmt, *tuples);
    printf("Results:\n");
    for (const auto& result : *results.get()) {
      std::cout << "\t" << result << std::endl;
    }
  }
  if (std::cin.bad()) {
    fprintf(stderr, "I/O Error\n");
    exit(EXIT_FAILURE);
  }

  printf("Shutting down btdb\n");
  return 0;
}