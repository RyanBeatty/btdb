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
#include <unordered_map>

#include "sql/context.hpp"

using btdb::sql::RawStmt;

void Panic(const std::string& msg) {
  std::cerr << "Panic: " << msg << std::endl;
  exit(EXIT_FAILURE);
}

struct TableDef {
  std::string name;
  std::vector<std::string> col_names;
};

struct SystemCatalog {
  std::unordered_map<std::string, TableDef> tables;

  bool ValidateStmt(RawStmt& stmt) {
    switch (stmt.index()) {
      case 0: {
        const btdb::sql::SelectStmt select = std::get<btdb::sql::SelectStmt>(stmt);
        if (this->tables.find(select.table_name) == this->tables.end()) {
          return false;
        }
        break;
      }
      default:
        Panic("Unknown Statement Type");
    }

    return true;
  }
};

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

struct SequentialScan {};

typedef std::variant<SequentialScan> Plan;

Plan PlanQuery(Query& query) {
  Plan plan;
  switch (query.index()) {
    case 0: {
      const SelectQuery& select_query = std::get<SelectQuery>(query);
      plan = SequentialScan{};
      break;
    }
    default:
      Panic("Unknown Query Type");
  }
  return plan;
}

std::unique_ptr<std::vector<std::string>> execute_plan(Plan& plan,
                                                       std::vector<std::string>& tuples) {
  if (std::holds_alternative<SequentialScan>(plan)) {
    auto& select_stmt = std::get<SequentialScan>(plan);
    auto results = std::make_unique<std::vector<std::string>>();
    for (const auto& tuple : tuples) {
      results->emplace_back(tuple);
    }
    return results;
  }
  return nullptr;
}

int main() {
  std::cout << "Starting btdb" << std::endl;

  auto tuples = std::make_unique<std::vector<std::string>>();
  auto catalog = SystemCatalog{std::unordered_map<std::string, TableDef>{{"foo", {"bar"}}}};
  tuples->emplace_back("hello");
  tuples->emplace_back("world");
  while (true) {
    std::cout << "btdb> ";
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
    if (!catalog.ValidateStmt(stmt)) {
      std::cout << "Query not valid" << std::endl;
      continue;
    }
    auto query = AnalyzeAndRewriteStmt(stmt);
    auto plan = PlanQuery(query);
    auto results = execute_plan(plan, *tuples);
    std::cout << "Results:" << std::endl;
    for (const auto& result : *results.get()) {
      std::cout << "\t" << result << std::endl;
    }
  }
  if (std::cin.bad()) {
    Panic("I/O Error");
  }

  std::cout << "Shutting down btdb" << std::endl;
  return 0;
}