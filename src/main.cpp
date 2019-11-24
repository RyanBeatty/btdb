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
#include "storage.h"
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

Datum EvalExpr(ParseNode* node, const Tuple& cur_tuple) {
  switch (node->type) {
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)node;
      assert(str_lit->str_lit != NULL);
      return MakeDatum(T_STRING, new std::string(str_lit->str_lit));
    }
    case NBOOL_LIT: {
      NBoolLit* bool_lit = (NBoolLit*)node;
      assert(bool_lit != NULL);
      return MakeDatum(T_BOOL, new bool(bool_lit->bool_lit));
    }
    case NIDENTIFIER: {
      // TODO(ryan): Not true in the future.
      NIdentifier* identifier = (NIdentifier*)node;
      assert(identifier->identifier != NULL);
      auto it = cur_tuple.find(identifier->identifier);
      assert(it != cur_tuple.end());
      return it->second;
    }
    case NBIN_EXPR: {
      NBinExpr* expr = (NBinExpr*)node;
      assert(expr->lhs != NULL);
      assert(expr->rhs != NULL);
      auto lhs_value = EvalExpr(expr->lhs, cur_tuple);
      auto rhs_value = EvalExpr(expr->rhs, cur_tuple);
      switch (expr->op) {
        case AND: {
          assert(lhs_value.type == T_BOOL);
          assert(rhs_value.type == T_BOOL);
          assert(lhs_value.data != NULL);
          assert(rhs_value.data != NULL);
          bool* lhs_data = (bool*)lhs_value.data;
          bool* rhs_data = (bool*)rhs_value.data;
          return MakeDatum(T_BOOL, new bool(*lhs_data && *rhs_data));
        }
        case OR: {
          assert(lhs_value.type == T_BOOL);
          assert(rhs_value.type == T_BOOL);
          assert(lhs_value.data != NULL);
          assert(rhs_value.data != NULL);
          bool* lhs_data = (bool*)lhs_value.data;
          bool* rhs_data = (bool*)rhs_value.data;
          return MakeDatum(T_BOOL, new bool(*lhs_data || *rhs_data));
        }
        case EQ: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != NULL);
          assert(rhs_value.data != NULL);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data == *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data == *rhs_data));
          } else {
            Panic("Invalid type for eq");
            return MakeDatum(T_UNKNOWN, NULL);
          }
        }
        case NEQ: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != NULL);
          assert(rhs_value.data != NULL);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data != *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data != *rhs_data));
          } else {
            Panic("Invalid type for neq");
            return MakeDatum(T_UNKNOWN, NULL);
          }
        }
        case GT: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != NULL);
          assert(rhs_value.data != NULL);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data > *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data > *rhs_data));
          } else {
            Panic("Invalid type for gt");
            return MakeDatum(T_UNKNOWN, NULL);
          }
        }
        case GE: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != NULL);
          assert(rhs_value.data != NULL);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data >= *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data >= *rhs_data));
          } else {
            Panic("Invalid type for ge");
            return MakeDatum(T_UNKNOWN, NULL);
          }
        }
        case LT: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != NULL);
          assert(rhs_value.data != NULL);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data < *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data < *rhs_data));
          } else {
            Panic("Invalid type for lt");
            return MakeDatum(T_UNKNOWN, NULL);
          }
        }
        case LE: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != NULL);
          assert(rhs_value.data != NULL);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data <= *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return MakeDatum(T_BOOL, new bool(*lhs_data <= *rhs_data));
          } else {
            Panic("Invalid type for le");
            return MakeDatum(T_UNKNOWN, NULL);
          }
        }
        default: {
          Panic("Unknown or Unsupported BinExprOp!");
          return MakeDatum(T_UNKNOWN, NULL);
        }
      }
    }
    default: {
      Panic("Unknown ParseNode type!");
      return MakeDatum(T_UNKNOWN, NULL);
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
    for (;;) {
      TuplePtrVecIt it = GetTuple(next_index);
      if (it == NULL) {
        return NULL;
      }
      ++next_index;

      assert(*it != NULL);
      const Tuple& cur_tpl = **it;

      // Evaluate predicate if any.
      if (where_clause != NULL) {
        auto result_val = EvalExpr(where_clause, cur_tpl);
        assert(result_val.type == T_BOOL);
        assert(result_val.data != NULL);
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
  }
  void Close() {}
};

struct InsertScan : Iterator {
  std::vector<Tuple> tuples;

  InsertScan(std::vector<Tuple> tuples) : tuples(tuples) {}

  void Open() {}

  MTuple GetNext() {
    for (;;) {
      if (tuples.size() == 0) {
        return NULL;
      }
      auto& tuple = tuples.back();
      Tuple* new_tuple = new Tuple(tuple);
      InsertTuple(new_tuple);
      tuples.pop_back();
    }
  }

  void Close() {}
};

struct DeleteScan : Iterator {
  uint64_t next_index;
  ParseNode* where_clause;

  DeleteScan(ParseNode* where_clause) : next_index(0), where_clause(where_clause) {}

  void Open() {}

  MTuple GetNext() {
    for (;;) {
      TuplePtrVecIt it = GetTuple(next_index);
      if (it == NULL) {
        return NULL;
      }
      assert(*it != NULL);
      const Tuple& cur_tpl = **it;

      // Evaluate predicate if any.
      if (where_clause != NULL) {
        auto result_val = EvalExpr(where_clause, cur_tpl);
        assert(result_val.type == T_BOOL);
        assert(result_val.data != NULL);
        bool* result = (bool*)result_val.data;
        if (!*result) {
          ++next_index;
          continue;
        }
      }

      auto result = std::make_unique<Tuple>(cur_tpl);
      DeleteHeapTuple(next_index);
      return result;
    }

    return NULL;
  }

  void Close() {}
};

struct UpdateScan : Iterator {
  uint64_t next_index = 0;

  List* assign_exprs;
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  ParseNode* where_clause;

  UpdateScan(List* assign_exprs, ParseNode* where_clause)
      : assign_exprs(assign_exprs), where_clause(where_clause) {}

  void Open() {}
  MTuple GetNext() {
    for (;;) {
      TuplePtrVecIt it = GetTuple(next_index);
      if (it == NULL) {
        return NULL;
      }
      assert(*it != NULL);
      Tuple& cur_tpl = **it;
      ++next_index;

      // Evaluate predicate if any.
      if (where_clause != NULL) {
        auto result_val = EvalExpr(where_clause, cur_tpl);
        assert(result_val.type == T_BOOL);
        assert(result_val.data != NULL);
        bool* result = (bool*)result_val.data;
        if (!*result) {
          continue;
        }
      }

      ListCell* lc = NULL;
      FOR_EACH(lc, assign_exprs) {
        NAssignExpr* assign_expr = (NAssignExpr*)lc->data;
        assert(assign_expr->type == NASSIGN_EXPR);
        assert(assign_expr->column != NULL);
        assert(assign_expr->value_expr != NULL);

        NIdentifier* col = (NIdentifier*)assign_expr->column;
        assert(col->type == NIDENTIFIER);
        assert(col->identifier != NULL);

        auto it = cur_tpl.find(col->identifier);
        assert(it != cur_tpl.end());
        Datum updated_value = EvalExpr(assign_expr->value_expr, cur_tpl);
        assert(updated_value.type == it->second.type);
        it->second = updated_value;
      }
      return std::make_unique<Tuple>(cur_tpl);
    }

    return NULL;
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
      auto plan = std::make_unique<UpdateScan>(
          UpdateScan(query->assign_expr_list, query->where_clause));
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
  while (mtuple != NULL) {
    results.tuples.push_back(std::move(mtuple));
    mtuple = plan->GetNext();
  }
  return results;
}

}  // namespace btdb

int main() {
  printf("Starting btdb\n");

  btdb::Tuple* t1 = new btdb::Tuple({
      {"bar", btdb::MakeDatum(btdb::T_STRING, new std::string("hello"))},
      {"baz", btdb::MakeDatum(btdb::T_BOOL, new bool(true))},
  });
  btdb::Tuple* t2 = new btdb::Tuple({
      {"bar", btdb::MakeDatum(btdb::T_STRING, new std::string("world"))},
      {"baz", btdb::MakeDatum(btdb::T_BOOL, new bool(false))},
  });
  btdb::InsertTuple(t1);
  btdb::InsertTuple(t2);
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
    btdb::Query* query = btdb::AnalyzeParseTree(parser.tree);
    if (query == NULL) {
      printf("Query not valid\n");
      continue;
    }
    auto plan_state = btdb::PlanQuery(query);
    auto results = btdb::execute_plan(plan_state);
    if (results.columns != NULL) {
      btdb::CharPtrVecIt it = NULL;
      VEC_FOREACH(it, results.columns) { printf("    %s", *it); }
      printf("\n");
      printf("===============\n");
      for (auto&& mtuple : results.tuples) {
        assert(mtuple != NULL);
        btdb::CharPtrVecIt it = NULL;
        VEC_FOREACH(it, results.columns) {
          std::string column(*it);
          auto it = mtuple->find(column);
          if (it != mtuple->end()) {
            btdb::Datum data = it->second;
            // TODO(ryan): This is some hacky bs to be able to print this as a string.
            // I'm going to need to do an overhaul of alot of this code in the future.
            if (data.type == btdb::T_STRING) {
              std::cout << *((std::string*)data.data);
            } else if (data.type == btdb::T_BOOL) {
              std::cout << (*((bool*)data.data) ? "true" : "false");
            } else {
              btdb::Panic("Only support printing strings or bools");
            }
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
