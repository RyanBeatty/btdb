#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Only define once.
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "analyzer.h"
#include "collections.h"
#include "node.h"
#include "sql/driver.h"
#include "storage.h"
#include "types.h"
#include "utils.h"

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
      bool* bool_lit_copy = (bool*)calloc(1, sizeof(bool));
      *bool_lit_copy = bool_lit->bool_lit;
      return MakeDatum(T_BOOL, bool_lit_copy);
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
      Datum lhs_value = EvalExpr(expr->lhs, cur_tuple);
      Datum rhs_value = EvalExpr(expr->rhs, cur_tuple);
      switch (expr->op) {
        case AND: {
          return BoolAnd(lhs_value, rhs_value);
        }
        case OR: {
          return BoolOr(lhs_value, rhs_value);
        }
        case EQ: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != NULL);
          assert(rhs_value.data != NULL);
          if (lhs_value.type == T_BOOL) {
            return BoolEQ(lhs_value, rhs_value);
          } else if (lhs_value.type == T_STRING) {
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
            bool* bool_lit_copy = (bool*)calloc(1, sizeof(bool));
            *bool_lit_copy = *lhs_data == *rhs_data;
            return MakeDatum(T_BOOL, bool_lit_copy);
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
            return BoolNE(lhs_value, rhs_value);
          } else if (lhs_value.type == T_STRING) {
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
            bool* bool_lit_copy = (bool*)calloc(1, sizeof(bool));
            *bool_lit_copy = *lhs_data != *rhs_data;
            return MakeDatum(T_BOOL, bool_lit_copy);
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
            return BoolGT(lhs_value, rhs_value);
          } else if (lhs_value.type == T_STRING) {
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
            bool* bool_lit_copy = (bool*)calloc(1, sizeof(bool));
            *bool_lit_copy = *lhs_data > *rhs_data;
            return MakeDatum(T_BOOL, bool_lit_copy);
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
            return BoolGTE(lhs_value, rhs_value);
          } else if (lhs_value.type == T_STRING) {
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
            bool* bool_lit_copy = (bool*)calloc(1, sizeof(bool));
            *bool_lit_copy = *lhs_data >= *rhs_data;
            return MakeDatum(T_BOOL, bool_lit_copy);
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
            return BoolLT(lhs_value, rhs_value);
          } else if (lhs_value.type == T_STRING) {
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
            bool* bool_lit_copy = (bool*)calloc(1, sizeof(bool));
            *bool_lit_copy = *lhs_data < *rhs_data;
            return MakeDatum(T_BOOL, bool_lit_copy);
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
            return BoolLTE(lhs_value, rhs_value);
          } else if (lhs_value.type == T_STRING) {
            char* lhs_data = (char*)lhs_value.data;
            char* rhs_data = (char*)rhs_value.data;
            bool* bool_lit_copy = (bool*)calloc(1, sizeof(bool));
            *bool_lit_copy = *lhs_data <= *rhs_data;
            return MakeDatum(T_BOOL, bool_lit_copy);
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

typedef struct ScanState {
  uint64_t next_index;
  CharPtrVec* target_list;
  ParseNode* where_clause;
  Tuple** tuples;  // stb_arr
  List* assign_exprs;

  Tuple* (*scan_func)(struct ScanState*);
} ScanState;

ScanState* MakeScanState(CharPtrVec* target_list, ParseNode* where_clause, Tuple** tuples,
                         List* assign_exprs, Tuple* (*scan_func)(ScanState*)) {
  ScanState* state = (ScanState*)calloc(1, sizeof(ScanState));
  state->next_index = 0;
  state->target_list = target_list;
  state->where_clause = where_clause;
  state->tuples = tuples;
  state->assign_exprs = assign_exprs;
  state->scan_func = scan_func;
  return state;
}

Tuple* SequentialScan(ScanState* state) {
  for (;;) {
    Tuple* cur_tpl = GetTuple(state->next_index);
    if (cur_tpl == NULL) {
      return NULL;
    }
    ++state->next_index;

    // Evaluate predicate if any.
    if (state->where_clause != NULL) {
      Datum result_val = EvalExpr(state->where_clause, cur_tpl);
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
    VEC_FOREACH(target, state->target_list) {
      Datum* data = GetCol(cur_tpl, *target);
      assert(data != NULL);
      result_tpl = SetCol(result_tpl, *target, *data);
    }
    return result_tpl;
  }
}

Tuple* InsertScan(ScanState* state) {
  Tuple* cur_tuple = NULL;
  for (size_t i = 0; i < arrlen(state->tuples); ++i) {
    cur_tuple = state->tuples[i];
    assert(cur_tuple != NULL);
    InsertTuple(CopyTuple(cur_tuple));
  }
  return NULL;
}

Tuple* DeleteScan(ScanState* state) {
  for (;;) {
    Tuple* cur_tpl = GetTuple(state->next_index);
    if (cur_tpl == NULL) {
      return NULL;
    }

    // Evaluate predicate if any.
    if (state->where_clause != NULL) {
      Datum result_val = EvalExpr(state->where_clause, cur_tpl);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        ++state->next_index;
        continue;
      }
    }

    Tuple* new_tuple = CopyTuple(cur_tpl);
    DeleteHeapTuple(state->next_index);
    return new_tuple;
  }

  return NULL;
}

Tuple* UpdateScan(ScanState* state) {
  for (;;) {
    Tuple* cur_tpl = GetTuple(state->next_index);
    if (cur_tpl == NULL) {
      return NULL;
    }
    ++state->next_index;

    // Evaluate predicate if any.
    if (state->where_clause != NULL) {
      Datum result_val = EvalExpr(state->where_clause, cur_tpl);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        continue;
      }
    }

    ListCell* lc = NULL;
    FOR_EACH(lc, state->assign_exprs) {
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

typedef struct PlanState {
  CharPtrVec* target_list;
  ScanState* scan_state;
} PlanState;

PlanState PlanQuery(Query* query) {
  assert(query != NULL);
  PlanState plan_state;
  switch (query->cmd) {
    case CMD_SELECT: {
      plan_state.target_list = query->target_list;
      plan_state.scan_state =
          MakeScanState(query->target_list, query->where_clause, query->values,
                        query->assign_expr_list, SequentialScan);
      break;
    }
    case CMD_INSERT: {
      // TODO(ryan): This is also wonky.
      plan_state.target_list = query->target_list;
      plan_state.scan_state =
          MakeScanState(query->target_list, query->where_clause, query->values,
                        query->assign_expr_list, InsertScan);
      break;
    }
    case CMD_DELETE: {
      plan_state.target_list = NULL;
      plan_state.scan_state =
          MakeScanState(query->target_list, query->where_clause, query->values,
                        query->assign_expr_list, DeleteScan);
      break;
    }
    case CMD_UPDATE: {
      plan_state.target_list = NULL;
      plan_state.scan_state =
          MakeScanState(query->target_list, query->where_clause, query->values,
                        query->assign_expr_list, UpdateScan);
      break;
    }
    default:
      Panic("Unknown Query Command Type");
  }
  return plan_state;
}

typedef struct Result {
  CharPtrVec* columns;
  Tuple** tuples;  // stb_arr
} Result;

Result execute_plan(PlanState* plan_state) {
  Result results;
  results.columns = plan_state->target_list;
  results.tuples = NULL;
  ScanState* scan_state = plan_state->scan_state;
  Tuple* mtuple = scan_state->scan_func(scan_state);
  while (mtuple != NULL) {
    arrpush(results.tuples, mtuple);
    mtuple = scan_state->scan_func(scan_state);
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
  bool* bool_lit = (bool*)calloc(sizeof(bool), 1);
  *bool_lit = true;
  t1 = SetCol(t1, "baz", MakeDatum(T_BOOL, bool_lit));

  Tuple* t2 = NULL;
  t2 = SetCol(t2, "bar", MakeDatum(T_STRING, strdup("world")));
  bool_lit = (bool*)calloc(sizeof(bool), 1);
  *bool_lit = false;
  t2 = SetCol(t2, "baz", MakeDatum(T_BOOL, bool_lit));

  InsertTuple(t1);
  InsertTuple(t2);
  while (true) {
    printf("btdb> ");
    char* line = NULL;
    size_t line_size = 0;
    if (getline(&line, &line_size, stdin) == -1) {
      break;
    }
    if (strcmp(line, "\\q\n") == 0) {
      break;
    }
    Parser* parser = InitParser(strdup(line));
    if (Parse(parser) != 0) {
      continue;
    }
    Query* query = AnalyzeParseTree(parser->tree);
    if (query == NULL) {
      printf("Query not valid\n");
      continue;
    }
    PlanState plan_state = PlanQuery(query);
    Result results = execute_plan(&plan_state);
    if (results.columns != NULL) {
      CharPtrVecIt it = NULL;
      VEC_FOREACH(it, results.columns) { printf("    %s", *it); }
      printf("\n");
      printf("===============\n");
      for (size_t i = 0; i < arrlen(results.tuples); ++i) {
        Tuple* mtuple = results.tuples[i];
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
  // TODO(ryan): Print out IO error condition if any.

  printf("Shutting down btdb\n");
  return 0;
}
