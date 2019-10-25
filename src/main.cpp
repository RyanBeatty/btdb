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
#include "sql/node.hpp"

namespace btdb {

static std::vector<std::string> Tuples;

void Panic(const std::string& msg) {
  std::cerr << "Panic: " << msg << std::endl;
  exit(EXIT_FAILURE);
}

struct TableDef {
  std::string name;
  std::vector<std::string> col_names;
};

struct SystemCatalog {
  // std::unordered_map<std::string, TableDef> tables;
  std::vector<TableDef> tables;

  bool ValidateParseTree(sql::ParseTree& tree) {
    assert(tree.tree != nullptr);
    sql::ParseNode* node = tree.tree;
    assert(node->type == sql::NSELECT_STMT);
    sql::NSelectStmt* select = (sql::NSelectStmt*)node;
    assert(select->table_name != nullptr && select->table_name->type == sql::NIDENTIFIER);
    auto* table_id = (sql::NIdentifier*)select->table_name;
    auto table_def_it = tables.begin();
    for (; table_def_it != tables.end(); ++table_def_it) {
      if (table_def_it->name == table_id->identifier) {
        break;
      }
    }
    if (table_def_it == tables.end()) {
      return false;
    }
    auto* target_list = select->target_list;
    for (uint64_t i = 0; i < target_list->length; ++i) {
      auto* item = target_list->items[i];
      assert(item != nullptr);
      assert(item->type == sql::NIDENTIFIER);
      sql::NIdentifier* col = (sql::NIdentifier*)item;
      assert(col->identifier != nullptr);
      if (std::find(table_def_it->col_names.begin(), table_def_it->col_names.end(),
                    col->identifier) == table_def_it->col_names.end()) {
        return false;
      }
    }
    return true;
  }
};

struct SelectQuery {
  std::vector<std::string> target_list;
  std::vector<std::string> range_table;
};

typedef std::variant<SelectQuery> Query;

Query AnalyzeAndRewriteParseTree(sql::ParseTree& tree) {
  assert(tree.tree != nullptr);
  ParseNode* node = tree.tree;
  assert(node->type == sql::NSELECT_STMT);
  sql::NSelectStmt* select = (NSelectStmt*)node;
  assert(select->table_name != nullptr && select->table_name->type == sql::NIDENTIFIER);
  sql::NIdentifier* identifier = (sql::NIdentifier*)select->table_name;
  auto table_name = std::string(identifier->identifier);

  assert(select->target_list != nullptr);
  auto* target_list = select->target_list;
  std::vector<std::string> targets;
  for (uint64_t i = 0; i < target_list->length; ++i) {
    auto* item = target_list->items[i];
    assert(item != nullptr && item->type == sql::NIDENTIFIER);
    sql::NIdentifier* target = (sql::NIdentifier*)item;
    targets.push_back(target->identifier);
  }

  return SelectQuery{targets, std::vector<std::string>{table_name}};
}

// TODO: Figure out what a tuple will actually look like.
typedef std::unique_ptr<std::string> MTuple;

struct SequentialScan {
  uint64_t next_index = 0;
  void Open() {}
  MTuple GetNext() {
    if (next_index >= Tuples.size()) {
      return nullptr;
    } else {
      auto tpl = std::make_unique<std::string>(Tuples[next_index]);
      ++next_index;
      return tpl;
    }
  }
  void Close() {}
};

struct Iterator {
  virtual void Open() = 0;
  virtual MTuple GetNext() = 0;
  virtual void Close() = 0;
};

struct SequentialIterator : Iterator {
  SequentialScan scan;
  SequentialIterator() {}
  void Open() { scan.Open(); }
  MTuple GetNext() { return scan.GetNext(); }
  void Close() { return scan.Close(); }
};

typedef Iterator Plan;

std::unique_ptr<Plan> PlanQuery(Query& query) {
  std::unique_ptr<Plan> plan;
  switch (query.index()) {
    case 0: {
      const SelectQuery& select_query = std::get<SelectQuery>(query);
      plan = std::make_unique<SequentialIterator>(SequentialIterator{});
      break;
    }
    default:
      Panic("Unknown Query Type");
  }
  return plan;
}

std::unique_ptr<std::vector<std::string>> execute_plan(std::unique_ptr<Plan> plan) {
  auto results = std::make_unique<std::vector<std::string>>();
  auto str = plan->GetNext();
  while (str != nullptr) {
    results->emplace_back(*str);
    str = plan->GetNext();
  }
  return results;
}

}  // namespace btdb

int main() {
  std::cout << "Starting btdb" << std::endl;

  btdb::TableDef table = {"foo", {"bar"}};
  auto catalog = btdb::SystemCatalog{{table}};
  btdb::Tuples.emplace_back("hello");
  btdb::Tuples.emplace_back("world");
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
    auto tree = std::move(parser.tree);
    if (!catalog.ValidateParseTree(*tree.get())) {
      std::cout << "Query not valid" << std::endl;
      continue;
    }
    auto query = btdb::AnalyzeAndRewriteParseTree(*tree.get());
    auto plan = btdb::PlanQuery(query);
    auto results = btdb::execute_plan(std::move(plan));
    std::cout << "Results:" << std::endl;
    for (const auto& result : *results.get()) {
      std::cout << "\t" << result << std::endl;
    }
  }
  if (std::cin.bad()) {
    btdb::Panic("I/O Error");
  }

  std::cout << "Shutting down btdb" << std::endl;
  return 0;
}
