
#include <assert.h>
#include <stdbool.h>

#include "stb_ds.h"

#include "analyzer.h"
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

void SequentialScanInit(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_SEQ_SCAN);
  SeqScan* scan = (SeqScan*)node;
  scan->next_index = 0;  
}

Tuple* SequentialScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_SEQ_SCAN);
  SeqScan* scan = (SeqScan*)node;
  for (;;) {
    Tuple* cur_tpl = GetTuple(scan->plan.table_def->index, scan->next_index);
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
    for (size_t i = 0; i < arrlen(scan->plan.target_list); ++i) {
      const char* col_name = scan->plan.target_list[i]->column_name;
      Datum* data = GetCol(cur_tpl, col_name);
      assert(data != NULL);
      result_tpl = SetCol(result_tpl, col_name, *data);
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
    InsertTuple(scan->plan.table_def->index, CopyTuple(cur_tuple));
  }
  return NULL;
}

Tuple* UpdateScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_MODIFY_SCAN);
  ModifyScan* scan = (ModifyScan*)node;
  assert(scan->cmd == CMD_UPDATE);
  for (;;) {
    Tuple* cur_tpl = GetTuple(scan->plan.table_def->index, scan->next_index);
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

    for (size_t i = 0; i < arrlen(scan->assign_exprs); ++i) {
      NAssignExpr* assign_expr = scan->assign_exprs[i];
      assert(assign_expr != NULL);
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
    Tuple* cur_tpl = GetTuple(scan->plan.table_def->index, scan->next_index);
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
    DeleteHeapTuple(scan->plan.table_def->index, scan->next_index);
    return new_tuple;
  }

  return NULL;
}

Tuple* SortScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_SORT);
  Sort* sort = (Sort*)node;

  if (!sort->is_sorted) {
    Tuple* cur_tuple = sort->plan.left->get_next_func(sort->plan.left);
    while (cur_tuple != NULL) {
      arrpush(sort->plan.results, cur_tuple);
      cur_tuple = sort->plan.left->get_next_func(sort->plan.left);
    }

    assert(sort->method == INSERTION_SORT);
    for (size_t i = 0; i < arrlen(sort->plan.results); ++i) {
      Tuple* insert_tuple = sort->plan.results[i];
      for (size_t j = 0; j < i; ++j) {
        Tuple* cur_tuple = sort->plan.results[j];

        Datum* left = GetCol(insert_tuple, sort->sort_col->identifier);
        Datum* right = GetCol(cur_tuple, sort->sort_col->identifier);
        assert(left != NULL);
        assert(right != NULL);
        Datum result = sort->cmp_func(*left, *right);
        if (*(bool*)result.data) {
          Tuple* swap = cur_tuple;
          sort->plan.results[j] = insert_tuple;
          sort->plan.results[i] = swap;
          insert_tuple = swap;
        }
      }
    }

    sort->is_sorted = true;
  }

  if (sort->next_index >= arrlen(sort->plan.results)) {
    return NULL;
  }

  Tuple* cur_tuple = sort->plan.results[sort->next_index];
  ++sort->next_index;
  return cur_tuple;
}

Tuple* NestedLoopScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_NESTED_LOOP);
  assert(node->left != NULL);
  assert(node->right != NULL);
  NestedLoop* join = (NestedLoop*)node;

  for (;;) {
    if (join->need_new_left_tuple) {
      join->cur_left_tuple = join->plan.left->get_next_func(join->plan.left);
      join->need_new_left_tuple = false;
      if (join->cur_left_tuple == NULL) {
        return NULL;
      }
      join->plan.right->init_func(join->plan.right);
    }

    Tuple* right_tuple = join->plan.right->get_next_func(join->plan.right);
    if (right_tuple == NULL) {
      join->need_new_left_tuple = true;
      continue;
    }

    // have both left and right, compute new result tuple.
    Tuple* result_tuple = NULL;
    return result_tuple; 
  }
}

Tuple* GetResult(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_RESULT);
  assert(node->left != NULL);
  return node->left->get_next_func(node->left);
}

PlanNode* PlanQuery(Query* query) {
  assert(query != NULL);
  PlanNode* plan = calloc(1, sizeof(PlanNode));
  plan->type = N_PLAN_RESULT;
  plan->target_list = query->target_list;
  plan->table_def = query->join_list[0];
  plan->get_next_func = GetResult;
  switch (query->cmd) {
    case CMD_SELECT: {
      // TODO(ryan): Special case joins for now because its easier to think about.
      // Eventually this should be cleaned up.
      if (arrlen(query->join_list) == 1) {
        SeqScan* scan = calloc(1, sizeof(SeqScan));
        scan->plan.type = N_PLAN_SEQ_SCAN;
        scan->plan.init_func = SequentialScanInit;
        scan->plan.get_next_func = SequentialScan;
        scan->plan.target_list = query->target_list;
        assert(arrlen(query->join_list) == 1);
        scan->plan.table_def = query->join_list[0];
        scan->table_name = query->table_name;
        scan->where_clause = query->where_clause;
        plan->left = (PlanNode*)scan;

        if (query->sort != NULL) {
          Sort* sort = calloc(1, sizeof(Sort));
          sort->plan.type = N_PLAN_SORT;
          sort->plan.get_next_func = SortScan;
          sort->plan.target_list = query->target_list;
          sort->method = INSERTION_SORT;

          // TODO(ryan): Allow for more robust sort expressions.
          // TODO(ryan): Move comparison function selection into analyzation step because we
          // do type checking there anyways.
          sort->sort_col = (NIdentifier*)query->sort->sort_expr;
          BType sort_col_type = GetColType(scan->plan.table_def, sort->sort_col->identifier);
          assert(sort_col_type != T_UNKNOWN);
          if (sort_col_type == T_STRING) {
            if (query->sort->dir == SORT_ASC) {
              sort->cmp_func = StrLT;
            } else if (query->sort->dir == SORT_DESC) {
              sort->cmp_func = StrGT;
            } else {
              Panic("invalid sort direction");
            }
          } else if (sort_col_type == T_BOOL) {
            if (query->sort->dir == SORT_ASC) {
              sort->cmp_func = BoolLT;
            } else if (query->sort->dir == SORT_DESC) {
              sort->cmp_func = BoolGT;
            } else {
              Panic("invalid sort direction");
            }
          }
          sort->is_sorted = false;

          sort->plan.left = (PlanNode*)scan;
          plan->left = (PlanNode*)sort;
        }
      } else {

      }
      return plan;
    }
    case CMD_INSERT: {
      assert(arrlen(query->join_list) == 1);
      ModifyScan* scan = calloc(1, sizeof(ModifyScan));
      scan->plan.type = N_PLAN_MODIFY_SCAN;
      scan->plan.get_next_func = InsertScan;
      scan->cmd = CMD_INSERT;
      scan->plan.target_list = query->target_list;
      scan->plan.table_def = query->join_list[0];
      scan->table_name = query->table_name;
      scan->where_clause = query->where_clause;
      scan->insert_tuples = query->values;

      plan->left = (PlanNode*)scan;
      return plan;
    }
    case CMD_UPDATE: {
      assert(arrlen(query->join_list) == 1);
      ModifyScan* scan = calloc(1, sizeof(ModifyScan));
      scan->plan.type = N_PLAN_MODIFY_SCAN;
      scan->plan.get_next_func = UpdateScan;
      scan->cmd = CMD_UPDATE;
      scan->plan.target_list = query->target_list;
      scan->plan.table_def = query->join_list[0];
      scan->table_name = query->table_name;
      scan->where_clause = query->where_clause;
      scan->assign_exprs = query->assign_expr_list;

      plan->left = (PlanNode*)scan;
      return plan;
    }
    case CMD_DELETE: {
      assert(arrlen(query->join_list) == 1);
      ModifyScan* scan = calloc(1, sizeof(ModifyScan));
      scan->plan.type = N_PLAN_MODIFY_SCAN;
      scan->plan.get_next_func = DeleteScan;
      scan->plan.target_list = query->target_list;
      scan->plan.table_def = query->join_list[0];
      scan->cmd = CMD_DELETE;
      scan->table_name = query->table_name;
      scan->where_clause = query->where_clause;

      plan->left = (PlanNode*)scan;
      return plan;
    }
    default: {
      Panic("Unknown query command type durring planning");
      return NULL;
    }
  }
  return NULL;
}
