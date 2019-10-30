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

void Panic(const std::string& msg) {
  std::cerr << "Panic: " << msg << std::endl;
  exit(EXIT_FAILURE);
}

struct TableDef {
  std::string name;
  std::vector<std::string> col_names;
};

enum BType {
  T_UNKNOWN,
  T_STRING,
  T_BOOL,
};

struct SystemCatalog {
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

    if (select->where_clause != nullptr) {
      if (CheckType(select->where_clause, *table_def_it) != T_BOOL) {
        return false;
      }
    }
    return true;
  }

  BType CheckType(sql::ParseNode* node, TableDef& table_def) {
    assert(node != nullptr);
    switch(node->type) {
      case sql::NSTRING_LIT: {
        return T_STRING;
      }
      case sql::NIDENTIFIER: {
        // TODO(ryan): Not true in the future.
        sql::NIdentifier* identifier = (NIdentifier*) node;
        assert(identifier->identifier != nullptr);
        if (std::find(table_def.col_names.begin(), table_def.col_names.end(), identifier->identifier) == table_def.col_names.end()) {
          Panic("Invalid column name in bin expr");
        }
        return T_STRING;
      }
      case sql::NBIN_EXPR: {
        sql::NBinExpr* expr = (sql::NBinExpr*) node;
        assert(expr->lhs != nullptr);
        assert(expr->rhs != nullptr);
        auto lhs_type = CheckType(expr->lhs, table_def);
        auto rhs_type = CheckType(expr->rhs, table_def);
        if (lhs_type == T_UNKNOWN || rhs_type == T_UNKNOWN) {
          return T_UNKNOWN;
        }
        switch(expr->op) {
          case sql::AND:
          case sql::OR: {
            if (lhs_type != T_BOOL || rhs_type != T_BOOL) {
              return T_UNKNOWN;
            }
            return T_BOOL;
          }
          case sql::EQ:
          case sql::NEQ: {
            if (lhs_type != rhs_type) {
              return T_UNKNOWN;
            }
            return T_BOOL;
          }
          case sql::GT:
          case sql::GE:
          case sql::LT:
          case sql::LE: {
            if (lhs_type != T_STRING || rhs_type != T_STRING) {
              return T_UNKNOWN;
            }
            return T_BOOL;
          }
          default: {
            Panic("Unknown or Unsupported BinExprOp!");
            return T_UNKNOWN;
          }
        }
      }
      default: {
        Panic("Unknown ParseNode type!");
        return T_UNKNOWN;
      }
    }
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
typedef std::unordered_map<std::string, std::string> Tuple;
typedef std::unique_ptr<Tuple> MTuple;

static std::vector<Tuple> Tuples;

struct Iterator {
  virtual void Open() = 0;
  virtual MTuple GetNext() = 0;
  virtual void Close() = 0;
};

// struct SequentialIterator : Iterator {
//   SequentialScan scan;
//   SequentialIterator() {}
//   void Open() { scan.Open(); }
//   MTuple GetNext() { return scan.GetNext(); }
//   void Close() { return scan.Close(); }
// };

typedef Iterator Plan;


struct SequentialScan : Iterator {
  uint64_t next_index = 0;

  std::vector<std::string> target_list;

  SequentialScan(std::vector<std::string> target_list) : target_list(target_list) {}

  void Open() {}
  MTuple GetNext() {
    if (next_index >= Tuples.size()) {
      return nullptr;
    } else {
      Tuple result_tpl;
      const auto& cur_tpl = Tuples[next_index];
      for (const auto& target : target_list) {
        auto it = cur_tpl.find(target);
        assert(it != cur_tpl.end());
        result_tpl[it->first] = it->second;
      }
      ++next_index;
      return std::make_unique<Tuple>(result_tpl);
    }
  }
  void Close() {}
};


struct PlanState {
  std::vector<std::string> target_list;
  std::unique_ptr<Plan> plan;
};

PlanState PlanQuery(Query& query) {
  PlanState plan_state;
  switch (query.index()) {
    case 0: {
      const SelectQuery& select_query = std::get<SelectQuery>(query);
      auto plan = std::make_unique<SequentialScan>(SequentialScan(select_query.target_list));
      plan_state.target_list = select_query.target_list;
      plan_state.plan = std::move(plan);
      break;
    }
    default:
      Panic("Unknown Query Type");
  }
  return plan_state;
}

struct Result {
  std::vector<std::string> columns;
  std::vector<MTuple> tuples;
};

Result execute_plan(PlanState& plan_state) {
  Result results;
  results.columns = plan_state.target_list;
  auto* plan = plan_state.plan.get();
  auto mtuple = plan->GetNext();
  while (mtuple != nullptr) {
    results.tuples.push_back(std::move(mtuple));
    mtuple = plan->GetNext();
  }
  return results;
}

}  // namespace btdb

int main() {
  std::cout << "Starting btdb" << std::endl;

  btdb::TableDef table = {"foo", {"bar", "baz"}};
  auto catalog = btdb::SystemCatalog{{table}};
  btdb::Tuple t1;
  t1["bar"] = "hello";
  t1["baz"] = "the quick brown fox";
  btdb::Tuple t2;
  t2["bar"] = "world";
  t2["baz"] = "jumped over the lazy dog";
  btdb::Tuples.push_back(t1);
  btdb::Tuples.push_back(t2);
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
    auto plan_state = btdb::PlanQuery(query);
    auto results = btdb::execute_plan(plan_state);
    for (const auto& column : results.columns) {
      std::cout << column << "\t";
    }
    std::cout << std::endl;
    std::cout << "===============" << std::endl;
    for (auto&& mtuple : results.tuples) {
      assert(mtuple != nullptr);
      for (const auto& column : results.columns) {
        auto it = mtuple->find(column);
        if (it != mtuple->end()) {
          std::cout << it->second;
        }
        std::cout << "\t";
      }
      std::cout << std::endl;
    }
  }
  if (std::cin.bad()) {
    btdb::Panic("I/O Error");
  }

  std::cout << "Shutting down btdb" << std::endl;
  return 0;
}
