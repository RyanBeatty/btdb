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

struct Datum {
  Datum(BType type, void* data) : type(type), data(data) {}

  BType type;
  // TODO(ryan): REMEMBER TO FIGURE OUT BEST WAY TO DELETE THIS, WE LEAK MEM HERE.
  void* data;
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

    // Validate target list contains valid references to columns.
    auto* target_list = select->target_list;
    assert(target_list != nullptr);
    assert(target_list->type == sql::T_PARSENODE);
    sql::ListCell* lc = nullptr;
    FOR_EACH(lc, target_list) {
      assert(lc->data != nullptr);
      sql::NIdentifier* col = (sql::NIdentifier*) lc->data;
      assert(col->type == sql::NIDENTIFIER);
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
    switch (node->type) {
      case sql::NSTRING_LIT: {
        return T_STRING;
      }
      case sql::NIDENTIFIER: {
        // TODO(ryan): Not true in the future.
        sql::NIdentifier* identifier = (NIdentifier*)node;
        assert(identifier->identifier != nullptr);
        if (std::find(table_def.col_names.begin(), table_def.col_names.end(),
                      identifier->identifier) == table_def.col_names.end()) {
          Panic("Invalid column name in bin expr");
        }
        return T_STRING;
      }
      case sql::NBIN_EXPR: {
        sql::NBinExpr* expr = (sql::NBinExpr*)node;
        assert(expr->lhs != nullptr);
        assert(expr->rhs != nullptr);
        auto lhs_type = CheckType(expr->lhs, table_def);
        auto rhs_type = CheckType(expr->rhs, table_def);
        if (lhs_type == T_UNKNOWN || rhs_type == T_UNKNOWN) {
          return T_UNKNOWN;
        }
        switch (expr->op) {
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
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  sql::ParseNode* where_clause;
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
  assert(select->target_list->type = sql::T_PARSENODE);
  auto* target_list = select->target_list;
  std::vector<std::string> targets;
  sql::ListCell* lc = nullptr;
  FOR_EACH(lc, target_list) {
    assert(lc->data != nullptr);
    sql::NIdentifier* target = (sql::NIdentifier*) lc->data;
    assert(target->type == sql::NIDENTIFIER);
    targets.push_back(target->identifier);
  }

  return SelectQuery{targets, std::vector<std::string>{table_name}, select->where_clause};
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

Datum ExecPred(ParseNode* node, const Tuple& cur_tuple) {
  switch (node->type) {
    case sql::NSTRING_LIT: {
      sql::NStringLit* str_lit = (sql::NStringLit*)node;
      assert(str_lit->str_lit != nullptr);
      return Datum(T_STRING, new std::string(str_lit->str_lit));
    }
    case sql::NIDENTIFIER: {
      // TODO(ryan): Not true in the future.
      sql::NIdentifier* identifier = (NIdentifier*)node;
      assert(identifier->identifier != nullptr);
      auto it = cur_tuple.find(identifier->identifier);
      assert(it != cur_tuple.end());
      return Datum(T_STRING, new std::string(it->second));
    }
    case sql::NBIN_EXPR: {
      sql::NBinExpr* expr = (sql::NBinExpr*)node;
      assert(expr->lhs != nullptr);
      assert(expr->rhs != nullptr);
      auto lhs_value = ExecPred(expr->lhs, cur_tuple);
      auto rhs_value = ExecPred(expr->rhs, cur_tuple);
      switch (expr->op) {
        case sql::AND: {
          assert(lhs_value.type == T_BOOL);
          assert(rhs_value.type == T_BOOL);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          bool* lhs_data = (bool*) lhs_value.data;
          bool* rhs_data = (bool*) rhs_value.data;
          return Datum(T_BOOL, new bool(*lhs_data && *rhs_data));
        }
        case sql::OR: {
          assert(lhs_value.type == T_BOOL);
          assert(rhs_value.type == T_BOOL);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          bool* lhs_data = (bool*) lhs_value.data;
          bool* rhs_data = (bool*) rhs_value.data;
          return Datum(T_BOOL, new bool(*lhs_data || *rhs_data));
        }
        case sql::EQ: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*) lhs_value.data;
            bool* rhs_data = (bool*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data == *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*) lhs_value.data;
            std::string* rhs_data = (std::string*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data == *rhs_data));
          } else {
            Panic("Invalid type for eq");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case sql::NEQ: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*) lhs_value.data;
            bool* rhs_data = (bool*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data != *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*) lhs_value.data;
            std::string* rhs_data = (std::string*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data != *rhs_data));
          } else {
            Panic("Invalid type for neq");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case sql::GT: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*) lhs_value.data;
            bool* rhs_data = (bool*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data > *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*) lhs_value.data;
            std::string* rhs_data = (std::string*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data > *rhs_data));
          } else {
            Panic("Invalid type for gt");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case sql::GE: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*) lhs_value.data;
            bool* rhs_data = (bool*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data >= *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*) lhs_value.data;
            std::string* rhs_data = (std::string*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data >= *rhs_data));
          } else {
            Panic("Invalid type for ge");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case sql::LT: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*) lhs_value.data;
            bool* rhs_data = (bool*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data < *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*) lhs_value.data;
            std::string* rhs_data = (std::string*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data < *rhs_data));
          } else {
            Panic("Invalid type for lt");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case sql::LE: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*) lhs_value.data;
            bool* rhs_data = (bool*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data <= *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*) lhs_value.data;
            std::string* rhs_data = (std::string*) rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data <= *rhs_data));
          } else {
            Panic("Invalid type for le");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        default: {
          Panic("Unknown or Unsupported BinExprOp!");
          return Datum(T_UNKNOWN, nullptr);
        }
      }
    }
    default: {
      Panic("Unknown ParseNode type!");
      return Datum(T_UNKNOWN, nullptr);
    }
  }
}

struct SequentialScan : Iterator {
  uint64_t next_index = 0;

  std::vector<std::string> target_list;
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  sql::ParseNode* where_clause;

  SequentialScan(std::vector<std::string> target_list, sql::ParseNode* where_clause)
      : target_list(target_list), where_clause(where_clause) {}

  void Open() {}
  MTuple GetNext() {
    while (next_index < Tuples.size()) {
      const auto& cur_tpl = Tuples[next_index];
      ++next_index;

      // Evaluate predicate if any.
      if (where_clause != nullptr) {
        auto result_val = ExecPred(where_clause, cur_tpl);
        assert(result_val.type == T_BOOL);
        assert(result_val.data != nullptr);
        bool* result = (bool*)result_val.data;
        if (!*result) {
          continue;
        }
      }

      // Column projections.
      Tuple result_tpl;
      for (const auto& target : target_list) {
        auto it = cur_tpl.find(target);
        assert(it != cur_tpl.end());
        result_tpl[it->first] = it->second;
      }
      return std::make_unique<Tuple>(result_tpl);
    }

    return nullptr;
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
      auto plan = std::make_unique<SequentialScan>(SequentialScan(select_query.target_list, select_query.where_clause));
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
