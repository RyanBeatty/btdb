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

#include "analyzer.h"
#include "collections.h"
#include "node.hpp"
#include "sql/context.hpp"
#include "types.h"
#include "utils.h"

namespace btdb {

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
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)node;
      assert(str_lit->str_lit != nullptr);
      return Datum(T_STRING, new std::string(str_lit->str_lit));
    }
    case NIDENTIFIER: {
      // TODO(ryan): Not true in the future.
      NIdentifier* identifier = (NIdentifier*)node;
      assert(identifier->identifier != nullptr);
      auto it = cur_tuple.find(identifier->identifier);
      assert(it != cur_tuple.end());
      return Datum(T_STRING, new std::string(it->second));
    }
    case NBIN_EXPR: {
      NBinExpr* expr = (NBinExpr*)node;
      assert(expr->lhs != nullptr);
      assert(expr->rhs != nullptr);
      auto lhs_value = ExecPred(expr->lhs, cur_tuple);
      auto rhs_value = ExecPred(expr->rhs, cur_tuple);
      switch (expr->op) {
        case AND: {
          assert(lhs_value.type == T_BOOL);
          assert(rhs_value.type == T_BOOL);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          bool* lhs_data = (bool*)lhs_value.data;
          bool* rhs_data = (bool*)rhs_value.data;
          return Datum(T_BOOL, new bool(*lhs_data && *rhs_data));
        }
        case OR: {
          assert(lhs_value.type == T_BOOL);
          assert(rhs_value.type == T_BOOL);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          bool* lhs_data = (bool*)lhs_value.data;
          bool* rhs_data = (bool*)rhs_value.data;
          return Datum(T_BOOL, new bool(*lhs_data || *rhs_data));
        }
        case EQ: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data == *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data == *rhs_data));
          } else {
            Panic("Invalid type for eq");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case NEQ: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data != *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data != *rhs_data));
          } else {
            Panic("Invalid type for neq");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case GT: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data > *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data > *rhs_data));
          } else {
            Panic("Invalid type for gt");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case GE: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data >= *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data >= *rhs_data));
          } else {
            Panic("Invalid type for ge");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case LT: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data < *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data < *rhs_data));
          } else {
            Panic("Invalid type for lt");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case LE: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data <= *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
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
  uint64_t next_index;

  CharPtrVec* target_list;
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  ParseNode* where_clause;

  SequentialScan(CharPtrVec* target_list, ParseNode* where_clause)
      : next_index(0), target_list(target_list), where_clause(where_clause) {}

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
      CharPtrVecIt target = NULL;
      VEC_FOREACH(target, target_list) {
        auto it = cur_tpl.find(*target);
        assert(it != cur_tpl.end());
        result_tpl[it->first] = it->second;
      }
      return std::make_unique<Tuple>(result_tpl);
    }

    return nullptr;
  }
  void Close() {}
};

struct InsertScan : Iterator {
  std::vector<Tuple> tuples;

  InsertScan(std::vector<Tuple> tuples) : tuples(tuples) {}

  void Open() {}

  MTuple GetNext() {
    if (tuples.size() == 0) {
      return nullptr;
    }
    auto& tuple = tuples.back();
    Tuples.push_back(tuple);
    tuples.pop_back();
    // TODO(ryan): This is sort of wonky, figure out how to do insert scans.
    return std::make_unique<Tuple>(tuple);
  }

  void Close() {}
};

struct DeleteScan : Iterator {
  uint64_t next_index;
  ParseNode* where_clause;

  DeleteScan(ParseNode* where_clause) : next_index(0), where_clause(where_clause) {}

  void Open() {}

  MTuple GetNext() {
    while (next_index < Tuples.size()) {
      const auto& cur_tpl = Tuples[next_index];

      // Evaluate predicate if any.
      if (where_clause != nullptr) {
        auto result_val = ExecPred(where_clause, cur_tpl);
        assert(result_val.type == T_BOOL);
        assert(result_val.data != nullptr);
        bool* result = (bool*)result_val.data;
        if (!*result) {
          ++next_index;
          continue;
        }
      }

      auto result = std::make_unique<Tuple>(cur_tpl);
      Tuples.erase(Tuples.begin() + next_index);
      return result;
    }

    return nullptr;
  }

  void Close() {}
};

struct UpdateScan : Iterator {
  uint64_t next_index = 0;

  std::vector<std::vector<std::string>> assign_exprs;
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  ParseNode* where_clause;

  UpdateScan(std::vector<std::vector<std::string>> assign_exprs, ParseNode* where_clause)
      : assign_exprs(assign_exprs), where_clause(where_clause) {}

  void Open() {}
  MTuple GetNext() {
    while (next_index < Tuples.size()) {
      auto& cur_tpl = Tuples[next_index];
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

      for (const auto& assign_expr : assign_exprs) {
        cur_tpl[assign_expr[0]] = assign_expr[1];
      }
      return std::make_unique<Tuple>(cur_tpl);
    }

    return nullptr;
  }
  void Close() {}
};

struct PlanState {
  CharPtrVec* target_list;
  std::unique_ptr<Plan> plan;
};

PlanState PlanQuery(Query* query) {
  assert(query != NULL);
  PlanState plan_state;
  switch (query->cmd) {
    case CMD_SELECT: {
      auto plan = std::make_unique<SequentialScan>(
          SequentialScan(query->target_list, query->where_clause));
      plan_state.target_list = query->target_list;
      plan_state.plan = std::move(plan);
      break;
    }
    case CMD_INSERT: {
      auto plan = std::make_unique<InsertScan>(InsertScan(query->values));
      // TODO(ryan): This is also wonky.
      plan_state.target_list = query->target_list;
      plan_state.plan = std::move(plan);
      break;
    }
    case CMD_DELETE: {
      auto plan = std::make_unique<DeleteScan>(DeleteScan(query->where_clause));
      plan_state.target_list = NULL;
      plan_state.plan = std::move(plan);
      break;
    }
    case CMD_UPDATE: {
      auto plan =
          std::make_unique<UpdateScan>(UpdateScan(query->assign_exprs, query->where_clause));
      plan_state.target_list = NULL;
      plan_state.plan = std::move(plan);
      break;
    }
    default:
      Panic("Unknown Query Command Type");
  }
  return plan_state;
}

struct Result {
  CharPtrVec* columns;
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
  printf("Starting btdb\n");

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
    btdb::Query* query = btdb::AnalyzeParseTree(tree.get()->tree);
    if (query == NULL) {
      printf("Query not valid\n");
      continue;
    }
    auto plan_state = btdb::PlanQuery(query);
    auto results = btdb::execute_plan(plan_state);
    if (results.columns != NULL) {
      btdb::CharPtrVecIt it = NULL;
      VEC_FOREACH(it, results.columns) {
        printf("%s\t", *it);
      }
      printf("\n");
      printf("===============\n");
      for (auto&& mtuple : results.tuples) {
        assert(mtuple != nullptr);
        btdb::CharPtrVecIt it = NULL;
        VEC_FOREACH(it, results.columns) {
          std::string column(*it);
          auto it = mtuple->find(column);
          if (it != mtuple->end()) {
            std::cout << it->second;
          }
          std::cout << "\t";
        }
        std::cout << std::endl;
      }
    } else {
        printf("===============\n");
    }
  }
  if (std::cin.bad()) {
    btdb::Panic("I/O Error");
  }

  std::cout << "Shutting down btdb" << std::endl;
  return 0;
}
