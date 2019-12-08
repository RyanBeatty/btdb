
#include <assert.h>
#include <stdbool.h>

#include "stb_ds.h"

#include "analyzer.h"
#include "collections.h"
#include "plan.h"
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

Tuple* SequentialScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_SEQ_SCAN);
  SeqScan* scan = (SeqScan*)node;
  for (;;) {
    Tuple* cur_tpl = GetTuple(scan->next_index);
    if (cur_tpl == NULL) {
      return NULL;
    }
    ++scan->next_index;

    // Evaluate predicate if any.
    if (scan->where_clause != NULL) {
      Datum result_val = EvalExpr(scan->where_clause, cur_tpl);
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
    VEC_FOREACH(target, scan->plan.target_list) {
      Datum* data = GetCol(cur_tpl, *target);
      assert(data != NULL);
      result_tpl = SetCol(result_tpl, *target, *data);
    }
    return result_tpl;
  }
}

Tuple* InsertScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_MODIFY_SCAN);
  ModifyScan* scan = (ModifyScan*)node;
  assert(scan->cmd == CMD_INSERT);
  Tuple* cur_tuple = NULL;
  for (size_t i = 0; i < arrlen(scan->insert_tuples); ++i) {
    cur_tuple = scan->insert_tuples[i];
    assert(cur_tuple != NULL);
    InsertTuple(CopyTuple(cur_tuple));
  }
  return NULL;
}

Tuple* UpdateScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_MODIFY_SCAN);
  ModifyScan* scan = (ModifyScan*)node;
  assert(scan->cmd == CMD_UPDATE);
  for (;;) {
    Tuple* cur_tpl = GetTuple(scan->next_index);
    if (cur_tpl == NULL) {
      return NULL;
    }
    ++scan->next_index;

    // Evaluate predicate if any.
    if (scan->where_clause != NULL) {
      Datum result_val = EvalExpr(scan->where_clause, cur_tpl);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        continue;
      }
    }

    ListCell* lc = NULL;
    FOR_EACH(lc, scan->assign_exprs) {
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

Tuple* DeleteScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_MODIFY_SCAN);
  ModifyScan* scan = (ModifyScan*)node;
  assert(scan->cmd == CMD_DELETE);
  for (;;) {
    Tuple* cur_tpl = GetTuple(scan->next_index);
    if (cur_tpl == NULL) {
      return NULL;
    }

    // Evaluate predicate if any.
    if (scan->where_clause != NULL) {
      Datum result_val = EvalExpr(scan->where_clause, cur_tpl);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        ++scan->next_index;
        continue;
      }
    }

    Tuple* new_tuple = CopyTuple(cur_tpl);
    DeleteHeapTuple(scan->next_index);
    return new_tuple;
  }

  return NULL;
}

PlanNode* Plan(Query* query) {
  assert(query != NULL);
  switch (query->cmd) {
    case CMD_SELECT: {
      SeqScan* scan = calloc(1, sizeof(SeqScan));
      scan->plan.type = N_PLAN_SEQ_SCAN;
      scan->plan.get_next_func = SequentialScan;
      scan->plan.target_list = query->target_list;
      scan->where_clause = query->where_clause;
      return (PlanNode*)scan;
    }
    case CMD_INSERT: {
      ModifyScan* scan = calloc(1, sizeof(ModifyScan));
      scan->plan.type = N_PLAN_MODIFY_SCAN;
      scan->plan.get_next_func = InsertScan;
      scan->cmd = CMD_INSERT;
      scan->plan.target_list = query->target_list;
      scan->where_clause = query->where_clause;
      scan->insert_tuples = query->values;
      return (PlanNode*)scan;
    }
    case CMD_UPDATE: {
      ModifyScan* scan = calloc(1, sizeof(ModifyScan));
      scan->plan.type = N_PLAN_MODIFY_SCAN;
      scan->plan.get_next_func = UpdateScan;
      scan->cmd = CMD_UPDATE;
      scan->plan.target_list = query->target_list;
      scan->where_clause = query->where_clause;
      scan->assign_exprs = query->assign_expr_list;
      return (PlanNode*)scan;
    }
    case CMD_DELETE: {
      ModifyScan* scan = calloc(1, sizeof(ModifyScan));
      scan->plan.type = N_PLAN_MODIFY_SCAN;
      scan->plan.get_next_func = DeleteScan;
      scan->plan.target_list = query->target_list;
      scan->cmd = CMD_DELETE;
      scan->where_clause = query->where_clause;
      return (PlanNode*)scan;
    }
    default: {
      Panic("Unknown query command type durring planning");
      return NULL;
    }
  }
  return NULL;
}
