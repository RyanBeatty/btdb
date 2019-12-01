#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>

// Only define once.
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "analyzer.h"
#include "collections.h"
#include "node.h"
#include "sql/context.hpp"
#include "storage.h"
#include "types.h"
#include "utils.h"

struct Iterator {
  virtual void Open() = 0;
  virtual Tuple* GetNext() = 0;
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

Datum EvalExpr(ParseNode* node, Tuple* cur_tuple) {
  switch (node->type) {
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)node;
      assert(str_lit->str_lit != NULL);
      return MakeDatum(T_STRING, strdup(str_lit->str_lit));
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
      Datum* data = GetCol((Tuple*)cur_tuple, identifier->identifier);
      assert(data != NULL);
      return *data;
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
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
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
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
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
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
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
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
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
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
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
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
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
  Tuple* GetNext() {
    for (;;) {
      Tuple* cur_tpl = GetTuple(next_index);
      if (cur_tpl == NULL) {
        return NULL;
      }
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

      // Column projections.
      Tuple* result_tpl = NULL;
      CharPtrVecIt target = NULL;
      VEC_FOREACH(target, target_list) {
        Datum* data = GetCol(cur_tpl, *target);
        assert(data != NULL);
        result_tpl = SetCol(result_tpl, *target, *data);
      }
      return result_tpl;
    }
  }
  void Close() {}
};

struct InsertScan : Iterator {
  Tuple** tuples;

  InsertScan(Tuple** tuples) : tuples(tuples) {}

  void Open() {}

  Tuple* GetNext() {
    Tuple* cur_tuple = NULL;
    for (size_t i = 0; i < arrlen(tuples); ++i) {
      cur_tuple = tuples[i];
      assert(cur_tuple != NULL);
      InsertTuple(CopyTuple(cur_tuple));
    }
    return NULL;
  }

  void Close() {}
};

struct DeleteScan : Iterator {
  uint64_t next_index;
  ParseNode* where_clause;

  DeleteScan(ParseNode* where_clause) : next_index(0), where_clause(where_clause) {}

  void Open() {}

  Tuple* GetNext() {
    for (;;) {
      Tuple* cur_tpl = GetTuple(next_index);
      if (cur_tpl == NULL) {
        return NULL;
      }

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

      Tuple* new_tuple = CopyTuple(cur_tpl);
      DeleteHeapTuple(next_index);
      return new_tuple;
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
  Tuple* GetNext() {
    for (;;) {
      Tuple* cur_tpl = GetTuple(next_index);
      if (cur_tpl == NULL) {
        return NULL;
      }
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

        Datum* data = GetCol(cur_tpl, col->identifier);
        assert(data != NULL);
        Datum updated_value = EvalExpr(assign_expr->value_expr, cur_tpl);
        assert(updated_value.type == data->type);
        *data = updated_value;
      }
      return CopyTuple(cur_tpl);
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
  std::vector<Tuple*> tuples;
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

int main() {
  printf("Starting btdb\n");

  ColDesc* tuple_desc = NULL;
  ColDesc col1 = {.column_name = "bar", .type = T_STRING};
  ColDesc col2 = {.column_name = "baz", .type = T_BOOL};
  arrpush(tuple_desc, col1);
  arrpush(tuple_desc, col2);
  TableDef table_def = {.name = "foo", .tuple_desc = tuple_desc};
  arrpush(Tables, table_def);

  Tuple* t1 = NULL;
  t1 = SetCol(t1, "bar", MakeDatum(T_STRING, strdup("hello")));
  t1 = SetCol(t1, "baz", MakeDatum(T_BOOL, new bool(true)));

  Tuple* t2 = NULL;
  t2 = SetCol(t2, "bar", MakeDatum(T_STRING, strdup("world")));
  t2 = SetCol(t2, "baz", MakeDatum(T_BOOL, new bool(false)));

  InsertTuple(t1);
  InsertTuple(t2);
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
    ParserContext parser(line);
    if (parser.Parse() != 0) {
      continue;
    }
    Query* query = AnalyzeParseTree(parser.tree);
    if (query == NULL) {
      printf("Query not valid\n");
      continue;
    }
    auto plan_state = PlanQuery(query);
    auto results = execute_plan(plan_state);
    if (results.columns != NULL) {
      CharPtrVecIt it = NULL;
      VEC_FOREACH(it, results.columns) { printf("    %s", *it); }
      printf("\n");
      printf("===============\n");
      for (auto&& mtuple : results.tuples) {
        assert(mtuple != NULL);
        CharPtrVecIt it = NULL;
        VEC_FOREACH(it, results.columns) {
          Datum* data = GetCol(mtuple, *it);
          if (data != NULL) {
            // TODO(ryan): This is some hacky bs to be able to print this as a string.
            // I'm going to need to do an overhaul of alot of this code in the future.
            if (data->type == T_STRING) {
              printf("%s", (char*)data->data);
            } else if (data->type == T_BOOL) {
              printf("%s", (*((bool*)data->data) ? "true" : "false"));
            } else {
              Panic("Only support printing strings or bools");
            }
          }
          printf("\t");
        }
        printf("\n");
      }
    } else {
      printf("===============\n");
    }
  }
  if (std::cin.bad()) {
    Panic("I/O Error");
  }

  printf("Shutting down btdb\n");
  return 0;
}
