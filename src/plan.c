
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
    case NINT_LIT: {
      NIntLit* int_lit = (NIntLit*)node;
      assert(int_lit != NULL);
      int32_t* int_lit_copy = (int32_t*)calloc(1, sizeof(int32_t));
      *int_lit_copy = int_lit->int_lit;
      return MakeDatum(T_INT, int_lit_copy);
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
      return expr->op_func(lhs_value, rhs_value);
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

    return cur_tpl;
  }
}

Tuple* InsertScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_MODIFY_SCAN);
  ModifyScan* scan = (ModifyScan*)node;
  assert(scan->cmd == CMD_INSERT);
  for (size_t i = 0; i < arrlen(scan->insert_tuples); ++i) {
    ParseNode** insert_tuple_expr = scan->insert_tuples[i];
    Tuple* new_tuple = NULL;
    assert(arrlen(scan->plan.table_def->tuple_desc) == arrlen(insert_tuple_expr));
    for (size_t j = 0; j < arrlen(scan->plan.table_def->tuple_desc); ++j) {
      ParseNode* col_expr = insert_tuple_expr[j];
      ColDesc col_desc = scan->plan.table_def->tuple_desc[j];
      Datum data = EvalExpr(col_expr, NULL);
      new_tuple = SetCol(new_tuple, col_desc.column_name, data);
    }
    assert(new_tuple != NULL);
    InsertTuple(scan->plan.table_def->index, new_tuple);
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

    DeleteHeapTuple(scan->plan.table_def->index, scan->next_index);
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
    for (size_t i = 0; i < arrlen(join->cur_left_tuple); ++i) {
      result_tuple = SetCol(result_tuple, join->cur_left_tuple[i].column_name,
                            join->cur_left_tuple[i].data);
    }
    for (size_t i = 0; i < arrlen(right_tuple); ++i) {
      result_tuple = SetCol(result_tuple, right_tuple[i].column_name, right_tuple[i].data);
    }
    return result_tuple;
  }
}

Tuple* GetResult(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_RESULT);
  assert(node->left != NULL);
  ResultScan* scan = (ResultScan*)node;
  for (;;) {
    Tuple* cur_tuple = scan->plan.left->get_next_func(node->left);
    if (cur_tuple == NULL) {
      return NULL;
    }

    // Evaluate predicate if any.
    if (scan->where_clause != NULL) {
      Datum result_val = EvalExpr(scan->where_clause, cur_tuple);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        continue;
      }
    }

    // TODO(ryan): For now need to do this for update queries to work. Not sure if this is
    // right in the long term.
    if (scan->plan.target_list == NULL) {
      return cur_tuple;
    }

    // Column projections.
    Tuple* result_tpl = NULL;
    for (size_t i = 0; i < arrlen(scan->plan.target_list); ++i) {
      Datum data = EvalExpr(scan->plan.target_list[i]->col_expr, cur_tuple);
      result_tpl = SetCol(result_tpl, scan->plan.target_list[i]->column_name, data);
    }
    return result_tpl;
  }
}

PlanNode* PlanQuery(Query* query) {
  assert(query != NULL);
  ResultScan* result = calloc(1, sizeof(ResultScan));
  result->plan.type = N_PLAN_RESULT;
  result->plan.target_list = query->target_list;
  result->plan.table_def = query->join_list[0];
  result->plan.get_next_func = GetResult;
  result->where_clause = query->where_clause;
  PlanNode* plan = (PlanNode*)result;
  switch (query->cmd) {
    case CMD_SELECT: {
      assert(arrlen(query->join_list) > 0);
      SeqScan* scan = calloc(1, sizeof(SeqScan));
      scan->plan.type = N_PLAN_SEQ_SCAN;
      scan->plan.init_func = SequentialScanInit;
      scan->plan.get_next_func = SequentialScan;
      scan->plan.target_list = query->target_list;
      scan->plan.table_def = query->join_list[0];
      // scan->table_name = query->table_name;
      // scan->where_clause = query->where_clause;
      // plan->left = (PlanNode*)scan;

      PlanNode* left_plan = (PlanNode*)scan;
      for (size_t i = 1; i < arrlen(query->join_list); ++i) {
        SeqScan* scan = calloc(1, sizeof(SeqScan));
        scan->plan.type = N_PLAN_SEQ_SCAN;
        scan->plan.init_func = SequentialScanInit;
        scan->plan.get_next_func = SequentialScan;
        scan->plan.target_list = query->target_list;
        scan->plan.table_def = query->join_list[i];
        PlanNode* right_plan = (PlanNode*)scan;

        // Create table def for joined table.
        // TODO(ryan): This code is super messy and breaks a bunch of abstraction barriers I
        // should really fix this at some point.
        ColDesc* tuple_desc = NULL;
        for (size_t j = 0; j < arrlen(left_plan->table_def->tuple_desc); ++j) {
          ColDesc desc = {.column_name = left_plan->table_def->tuple_desc[j].column_name,
                          left_plan->table_def->tuple_desc[j].type};
          arrpush(tuple_desc, desc);
        }
        for (size_t j = 0; j < arrlen(right_plan->table_def->tuple_desc); ++j) {
          ColDesc desc = {.column_name = right_plan->table_def->tuple_desc[j].column_name,
                          right_plan->table_def->tuple_desc[j].type};
          arrpush(tuple_desc, desc);
        }
        TableDef* table_def = calloc(1, sizeof(TableDef));
        char* tablename = calloc(
            strlen(left_plan->table_def->name) + strlen(right_plan->table_def->name) + 1,
            sizeof(char));
        strcat(tablename, left_plan->table_def->name);
        strcat(tablename, right_plan->table_def->name);
        table_def->name = tablename;
        table_def->tuple_desc = tuple_desc;

        NestedLoop* nested_loop = calloc(1, sizeof(NestedLoop));
        nested_loop->plan.type = N_PLAN_NESTED_LOOP;
        nested_loop->plan.get_next_func = NestedLoopScan;
        nested_loop->plan.target_list = query->target_list;
        nested_loop->plan.table_def = table_def;
        nested_loop->plan.left = left_plan;
        nested_loop->plan.right = right_plan;
        nested_loop->cur_left_tuple = NULL;
        nested_loop->need_new_left_tuple = true;

        left_plan = (PlanNode*)nested_loop;
      }

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
        BType sort_col_type = GetColType(left_plan->table_def, sort->sort_col->identifier);
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
        } else if (sort_col_type == T_INT) {
          if (query->sort->dir == SORT_ASC) {
            sort->cmp_func = IntLT;
          } else if (query->sort->dir == SORT_DESC) {
            sort->cmp_func = IntGT;
          } else {
            Panic("invalid sort direction");
          }
        } else {
          Panic("Unknown sort by type");
        }
        sort->is_sorted = false;

        sort->plan.left = left_plan;
        left_plan = (PlanNode*)sort;
      }
      plan->left = left_plan;
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

void ExecuteUtilityStmt(Query* query) {
  assert(query != NULL);
  assert(query->cmd == CMD_UTILITY);
  assert(query->utility_stmt != NULL);
  assert(query->utility_stmt->type == NCREATE_TABLE);

  TableDef* table_def = (TableDef*)calloc(1, sizeof(TableDef));

  NCreateTable* create = (NCreateTable*)query->utility_stmt;
  NIdentifier* table_name = (NIdentifier*)create->table_name;
  table_def->name = (char*)calloc(strlen(table_name->identifier) + 1, sizeof(char));
  strcpy(table_def->name, table_name->identifier);

  ColDesc* tuple_desc = NULL;
  for (size_t i = 0; i < arrlen(create->column_defs); ++i) {
    NColumnDef* column_def = (NColumnDef*)create->column_defs[i];
    NIdentifier* col_name = (NIdentifier*)column_def->col_name;

    ColDesc col_desc;
    col_desc.column_name = (char*)calloc(strlen(col_name->identifier) + 1, sizeof(char));
    strcpy(col_desc.column_name, col_name->identifier);
    col_desc.type = column_def->col_type_id;
    arrpush(tuple_desc, col_desc);
  }

  table_def->tuple_desc = tuple_desc;
  CreateTable(table_def);
  return;
}
