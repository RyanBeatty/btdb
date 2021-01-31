
#include "plan.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "analyzer.h"
#include "stb_ds.h"
#include "utils.h"

Datum EvalExpr(ParseNode* node, Tuple* cur_tuple, TableDef* table_def) {
  switch (node->type) {
    case NLITERAL: {
      NLiteral* literal = (NLiteral*)node;
      switch (literal->lit_type) {
        case T_BOOL: {
          bool* bool_lit_copy = (bool*)calloc(1, sizeof(bool));
          *bool_lit_copy = literal->data.bool_lit;
          return MakeDatum(T_BOOL, bool_lit_copy);
        }
        case T_INT: {
          int32_t* int_lit_copy = (int32_t*)calloc(1, sizeof(int32_t));
          *int_lit_copy = literal->data.int_lit;
          return MakeDatum(T_INT, int_lit_copy);
        }
        case T_STRING: {
          return MakeDatum(T_STRING, strdup(literal->data.str_lit));
        }
        case T_NULL: {
          return MakeDatum(T_NULL, NULL);
        }
        default: {
          Panic("Unknown literal type in EvalExpr");
          return MakeDatum(T_UNKNOWN, NULL);
        }
      }
    }
    case NIDENTIFIER: {
      // TODO(ryan): Not true in the future.
      NIdentifier* identifier = (NIdentifier*)node;
      assert(identifier->identifier != NULL);
      Datum data = GetCol(cur_tuple, identifier->identifier, table_def);
      return data;
    }
    case NBIN_EXPR: {
      NBinExpr* expr = (NBinExpr*)node;
      assert(expr->lhs != NULL);
      assert(expr->rhs != NULL);

      Datum lhs_value = EvalExpr(expr->lhs, cur_tuple, table_def);
      Datum rhs_value = EvalExpr(expr->rhs, cur_tuple, table_def);
      // Theres probably a way I can add some info/state in the analyzing phase to avoid this
      // check here.
      if (lhs_value.type == T_NULL || rhs_value.type == T_NULL) {
        return MakeDatum(T_NULL, NULL);
      }
      return expr->op_func(lhs_value, rhs_value);
    }
    default: {
      Panic("Unknown ParseNode type!");
      return MakeDatum(T_UNKNOWN, NULL);
    }
  }
}

void InitChildren(PlanNode* node) {
  assert(node != NULL);
  if (node->left != NULL) {
    node->left->init_func(node->left);
  }
  if (node->right != NULL) {
    node->right->init_func(node->right);
  }
}

void SequentialScanInit(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_SEQ_SCAN);
  SeqScan* scan = (SeqScan*)node;
  CursorInit(&scan->cursor, scan->plan.table_def);
  InitChildren(node);
}

Tuple* SequentialScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_SEQ_SCAN);
  SeqScan* scan = (SeqScan*)node;
  for (;;) {
    Tuple* cur_tpl2 = CursorSeekNext(&scan->cursor);
    if (cur_tpl2 == NULL) {
      return NULL;
    }

    // Evaluate predicate if any.
    if (scan->where_clause != NULL) {
      Datum result_val = EvalExpr(scan->where_clause, cur_tpl2, scan->plan.table_def);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        continue;
      }
    }

    return cur_tpl2;
  }
}

void IdxScanInit(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_INDEX_SCAN);
  IndexScan* scan = (IndexScan*)node;
  IndexCursorInit(&scan->cursor, scan->index_def,
                  EvalExpr(scan->boundry_search_key_value, NULL, NULL));
}

Tuple* IdxScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_INDEX_SCAN);
  IndexScan* scan = (IndexScan*)node;
  for (;;) {
    Tuple* tuple = NULL;
    if (IndexCursorInvalidPos(&scan->cursor)) {
      tuple = BTreeFirst(&scan->cursor);
    } else {
      tuple = BTreeGetNext(&scan->cursor);
    }
    if (tuple == NULL) {
      return NULL;
    }

    // Evaluate predicate if any.
    if (scan->where_clause != NULL) {
      Datum result_val = EvalExpr(scan->where_clause, tuple, scan->plan.table_def);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        continue;
      }
    }

    return tuple;
  }
}

Tuple* InsertScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_MODIFY_SCAN);
  ModifyScan* scan = (ModifyScan*)node;
  assert(scan->cmd == CMD_INSERT);
  for (size_t i = 0; i < arrlenu(scan->insert_tuples); ++i) {
    ParseNode** insert_tuple_expr = scan->insert_tuples[i];
    Tuple* new_tuple = MakeTuple(scan->plan.table_def);
    assert(arrlenu(scan->plan.table_def->tuple_desc) == arrlenu(insert_tuple_expr));
    for (size_t j = 0; j < arrlenu(scan->plan.table_def->tuple_desc); ++j) {
      ParseNode* col_expr = insert_tuple_expr[j];
      ColDesc col_desc = scan->plan.table_def->tuple_desc[j];
      Datum data = EvalExpr(col_expr, NULL, NULL);
      new_tuple = SetCol(new_tuple, col_desc.column_name, data, scan->plan.table_def);
    }
    assert(new_tuple != NULL);
    CursorInsertTuple(&scan->cursor, new_tuple);
  }
  return NULL;
}

Tuple* UpdateScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_MODIFY_SCAN);
  ModifyScan* scan = (ModifyScan*)node;
  assert(scan->cmd == CMD_UPDATE);
  Tuple** updated_tuples = NULL;
  for (;;) {
    Tuple* cur_tpl = CursorSeekNext(&scan->cursor);
    if (cur_tpl == NULL) {
      break;
    }

    // Evaluate predicate if any.
    if (scan->where_clause != NULL) {
      Datum result_val = EvalExpr(scan->where_clause, cur_tpl, scan->plan.table_def);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        continue;
      }
    }

    Tuple* updated_tuple = CopyTuple(cur_tpl);
    for (size_t i = 0; i < arrlenu(scan->assign_exprs); ++i) {
      NAssignExpr* assign_expr = scan->assign_exprs[i];
      assert(assign_expr != NULL);
      assert(assign_expr->type == NASSIGN_EXPR);
      assert(assign_expr->column != NULL);
      assert(assign_expr->value_expr != NULL);

      NIdentifier* col = (NIdentifier*)assign_expr->column;
      assert(col->type == NIDENTIFIER);
      assert(col->identifier != NULL);

      // Eval on cur_tpl to prevent seeing own updates.
      Datum updated_value = EvalExpr(assign_expr->value_expr, cur_tpl, scan->plan.table_def);
      updated_tuple =
          SetCol(updated_tuple, col->identifier, updated_value, scan->plan.table_def);
    }
    arrpush(updated_tuples, updated_tuple);
  }

  for (size_t i = 0; i < arrlenu(updated_tuples); ++i) {
    Tuple* updated_tuple = updated_tuples[i];
    CursorUpdateTupleById(&scan->cursor, updated_tuple, updated_tuple->self_tid);
  }

  return NULL;
}

Tuple* DeleteScan(PlanNode* node) {
  assert(node != NULL);
  assert(node->type == N_PLAN_MODIFY_SCAN);
  ModifyScan* scan = (ModifyScan*)node;
  assert(scan->cmd == CMD_DELETE);
  for (;;) {
    Tuple* cur_tpl = CursorSeekNext(&scan->cursor);
    if (cur_tpl == NULL) {
      return NULL;
    }

    // Evaluate predicate if any.
    if (scan->where_clause != NULL) {
      Datum result_val = EvalExpr(scan->where_clause, cur_tpl, scan->plan.table_def);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        continue;
      }
    }

    CursorDeleteTupleById(&scan->cursor, cur_tpl->self_tid);
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
    for (size_t i = 0; i < arrlenu(sort->plan.results); ++i) {
      Tuple* insert_tuple = sort->plan.results[i];
      for (size_t j = 0; j < i; ++j) {
        Tuple* cur_tuple = sort->plan.results[j];

        Datum left = GetCol(insert_tuple, sort->sort_col->identifier, sort->plan.table_def);
        Datum right = GetCol(cur_tuple, sort->sort_col->identifier, sort->plan.table_def);
        Datum result = sort->cmp_func(left, right);
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

  if (sort->next_index >= arrlenu(sort->plan.results)) {
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

  bool no_result_for_cur_left_tuple = false;
  for (;;) {
    if (join->need_new_left_tuple) {
      join->cur_left_tuple = join->plan.left->get_next_func(join->plan.left);
      join->need_new_left_tuple = false;
      if (join->cur_left_tuple == NULL) {
        return NULL;
      }
      join->plan.right->init_func(join->plan.right);
      no_result_for_cur_left_tuple = true;
    }

    Tuple* right_tuple = join->plan.right->get_next_func(join->plan.right);
    if (right_tuple == NULL) {
      join->need_new_left_tuple = true;
      switch (join->join_method) {
        case JOIN_INNER: {
          continue;
        }
        case JOIN_LEFT:
        case JOIN_RIGHT: {
          // If we have not found any result for the cur left tuple, need to make sure we
          // insert an entry in results.
          if (no_result_for_cur_left_tuple) {
            Tuple* result_tuple = MakeTuple(join->plan.table_def);
            // Right side columns are already null'ed, so fill in left side columns.
            for (size_t i = 0; i < arrlenu(join->plan.left->table_def->tuple_desc); ++i) {
              const char* col_name = join->plan.left->table_def->tuple_desc[i].column_name;
              Datum col_data =
                  GetCol(join->cur_left_tuple, col_name, join->plan.left->table_def);
              result_tuple = SetCol(result_tuple, col_name, col_data, join->plan.table_def);
            }
            return result_tuple;
          } else {
            continue;
          }
        }
        default: {
          Panic("Unknown join method when nested looping");
          return NULL;
        }
      }
    }

    // have both left and right, compute new result tuple.
    Tuple* result_tuple = MakeTuple(join->plan.table_def);
    for (size_t i = 0; i < arrlenu(join->plan.left->table_def->tuple_desc); ++i) {
      const char* col_name = join->plan.left->table_def->tuple_desc[i].column_name;
      Datum col_data = GetCol(join->cur_left_tuple, col_name, join->plan.left->table_def);
      result_tuple = SetCol(result_tuple, col_name, col_data, join->plan.table_def);
    }
    for (size_t i = 0; i < arrlenu(join->plan.right->table_def->tuple_desc); ++i) {
      const char* col_name = join->plan.right->table_def->tuple_desc[i].column_name;
      Datum col_data = GetCol(right_tuple, col_name, join->plan.right->table_def);
      result_tuple = SetCol(result_tuple, col_name, col_data, join->plan.table_def);
    }

    if (join->qual_condition != NULL) {
      Datum result_val = EvalExpr(join->qual_condition, result_tuple, join->plan.table_def);
      assert(result_val.type == T_BOOL);
      assert(result_val.data != NULL);
      bool* result = (bool*)result_val.data;
      if (!*result) {
        continue;
      }
    }

    no_result_for_cur_left_tuple = false;
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
      Datum result_val = EvalExpr(scan->where_clause, cur_tuple, scan->plan.left->table_def);
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
    Tuple* result_tpl = MakeTuple(scan->plan.table_def);
    for (size_t i = 0; i < arrlenu(scan->plan.target_list); ++i) {
      Datum data =
          EvalExpr(scan->plan.target_list[i]->col_expr, cur_tuple, scan->plan.left->table_def);
      result_tpl = SetCol(result_tpl, scan->plan.target_list[i]->column_name, data,
                          scan->plan.table_def);
    }
    return result_tpl;
  }
}

PlanNode* PlanJoin(Query* query, ParseNode* join_tree, ParseNode* where_clause) {
  assert(query != NULL);
  assert(join_tree != NULL);
  switch (join_tree->type) {
    case NJOIN: {
      NJoin* join_node = (NJoin*)join_tree;

      PlanNode* left_plan = PlanJoin(query, join_node->left, where_clause);
      PlanNode* right_plan = PlanJoin(query, join_node->right, where_clause);

      NestedLoop* nested_loop = calloc(1, sizeof(NestedLoop));
      nested_loop->plan.type = N_PLAN_NESTED_LOOP;
      nested_loop->plan.get_next_func = NestedLoopScan;
      nested_loop->plan.init_func = InitChildren;
      nested_loop->plan.target_list = query->target_list;
      nested_loop->plan.table_def = query->join_list[join_node->join_list_index];
      nested_loop->plan.left = left_plan;
      nested_loop->plan.right = right_plan;
      nested_loop->cur_left_tuple = NULL;
      nested_loop->need_new_left_tuple = true;
      nested_loop->join_method = join_node->join_method;
      nested_loop->qual_condition = join_node->qual_cond;

      if (nested_loop->join_method == JOIN_RIGHT) {
        nested_loop->plan.left = right_plan;
        nested_loop->plan.right = left_plan;
      }

      return (PlanNode*)nested_loop;
    }
    case NRANGEVAR: {
      NRangeVar* range_var = (NRangeVar*)join_tree;

      // If we have a where clause on the query, check to see if we can use an index scan.
      if (where_clause != NULL) {
        assert(where_clause->type == NBIN_EXPR);
        NBinExpr* expr = (NBinExpr*)where_clause;

        // For now we only support using index scans when the where clause is a simple binary
        // expr and one of the terms of the expression is a column reference covered by an
        // index. We also only support simple expressions where the side that is not an
        // identifier is a simple value. This is to make finding the initial/boundry key to
        // search for in the index easier.
        NIdentifier* col_ref = NULL;
        ParseNode* boundry_search_key_value = NULL;
        if (expr->lhs != NULL && expr->lhs->type == NIDENTIFIER && expr->rhs != NULL &&
            expr->rhs->type != NIDENTIFIER && expr->rhs->type != NBIN_EXPR) {
          col_ref = (NIdentifier*)expr->lhs;
          boundry_search_key_value = expr->rhs;
        } else if (expr->rhs != NULL && expr->rhs->type == NIDENTIFIER && expr->lhs != NULL &&
                   expr->lhs->type != NIDENTIFIER && expr->lhs->type != NBIN_EXPR) {
          col_ref = (NIdentifier*)expr->rhs;
          boundry_search_key_value = expr->lhs;
        }

        // One of the expression terms is an column identifier, check to see if an index covers
        // this column.
        if (col_ref != NULL) {
          TableDef* table_def = query->join_list[range_var->join_list_index];
          size_t i = 0;
          IndexDef* index_def = GetIndexDef(i);
          while (index_def != NULL) {
            if (index_def->table_def_idx == table_def->index) {
              TableDef* index_table_def = GetTableDef(index_def->index_table_def_idx);
              assert(index_table_def != NULL);
              // We only support single column indexes for now for correctness.
              if (arrlenu(index_table_def->tuple_desc) == 1 &&
                  strcmp(index_table_def->tuple_desc[0].column_name, col_ref->identifier) ==
                      0) {
                break;
              }
            }
            ++i;
            index_def = GetIndexDef(i);
          }

          if (index_def != NULL) {
            IndexScan* scan = (IndexScan*)calloc(1, sizeof(IndexScan));
            scan->plan.type = N_PLAN_INDEX_SCAN;
            scan->plan.init_func = IdxScanInit;
            scan->plan.get_next_func = IdxScan;
            scan->plan.target_list = query->target_list;
            scan->plan.table_def = query->join_list[range_var->join_list_index];
            scan->where_clause = where_clause;
            scan->index_def = index_def;
            scan->boundry_search_key_value = boundry_search_key_value;
            return (PlanNode*)scan;
          }
        }
      }

      SeqScan* scan = calloc(1, sizeof(SeqScan));
      scan->plan.type = N_PLAN_SEQ_SCAN;
      scan->plan.init_func = SequentialScanInit;
      scan->plan.get_next_func = SequentialScan;
      scan->plan.target_list = query->target_list;
      scan->plan.table_def = query->join_list[range_var->join_list_index];
      return (PlanNode*)scan;
    }
    default: {
      Panic("Unknown join node type during planning!");
      return NULL;
    }
  }
}

PlanNode* PlanQuery(Query* query) {
  assert(query != NULL);
  ResultScan* result = calloc(1, sizeof(ResultScan));
  result->plan.type = N_PLAN_RESULT;
  result->plan.target_list = query->target_list;
  result->plan.table_def = query->join_list[arrlenu(query->join_list) - 1];
  result->plan.get_next_func = GetResult;
  result->plan.init_func = InitChildren;
  result->where_clause = query->where_clause;
  PlanNode* plan = (PlanNode*)result;
  switch (query->cmd) {
    case CMD_SELECT: {
      assert(arrlenu(query->join_list) > 0);

      PlanNode* left_plan = PlanJoin(query, query->join_tree, query->where_clause);
      assert(left_plan != NULL);

      if (query->sort != NULL) {
        Sort* sort = calloc(1, sizeof(Sort));
        sort->plan.type = N_PLAN_SORT;
        sort->plan.get_next_func = SortScan;
        sort->plan.init_func = InitChildren;
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
        sort->plan.table_def = left_plan->table_def;
        left_plan = (PlanNode*)sort;
      }
      plan->left = left_plan;
      return plan;
    }
    case CMD_INSERT: {
      assert(arrlenu(query->join_list) == 1);
      ModifyScan* scan = calloc(1, sizeof(ModifyScan));
      scan->plan.type = N_PLAN_MODIFY_SCAN;
      scan->plan.get_next_func = InsertScan;
      scan->plan.init_func = InitChildren;
      scan->cmd = CMD_INSERT;
      scan->plan.target_list = query->target_list;
      scan->plan.table_def = query->join_list[0];
      scan->table_name = query->table_name;
      scan->where_clause = query->where_clause;
      scan->insert_tuples = query->values;
      CursorInit(&scan->cursor, scan->plan.table_def);

      plan->left = (PlanNode*)scan;
      return plan;
    }
    case CMD_UPDATE: {
      assert(arrlenu(query->join_list) == 1);
      ModifyScan* scan = calloc(1, sizeof(ModifyScan));
      scan->plan.type = N_PLAN_MODIFY_SCAN;
      scan->plan.get_next_func = UpdateScan;
      scan->plan.init_func = InitChildren;
      scan->cmd = CMD_UPDATE;
      scan->plan.target_list = query->target_list;
      scan->plan.table_def = query->join_list[0];
      scan->table_name = query->table_name;
      scan->where_clause = query->where_clause;
      scan->assign_exprs = query->assign_expr_list;
      CursorInit(&scan->cursor, scan->plan.table_def);

      plan->left = (PlanNode*)scan;
      return plan;
    }
    case CMD_DELETE: {
      assert(arrlenu(query->join_list) == 1);
      ModifyScan* scan = calloc(1, sizeof(ModifyScan));
      scan->plan.type = N_PLAN_MODIFY_SCAN;
      scan->plan.get_next_func = DeleteScan;
      scan->plan.init_func = InitChildren;
      scan->plan.target_list = query->target_list;
      scan->plan.table_def = query->join_list[0];
      scan->cmd = CMD_DELETE;
      scan->table_name = query->table_name;
      scan->where_clause = query->where_clause;
      CursorInit(&scan->cursor, scan->plan.table_def);

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

  switch (query->utility_stmt->type) {
    case NCREATE_TABLE: {
      TableDef* table_def = (TableDef*)calloc(1, sizeof(TableDef));

      NCreateTable* create = (NCreateTable*)query->utility_stmt;
      NIdentifier* table_name = (NIdentifier*)create->table_name;
      char* table_def_name = (char*)calloc(strlen(table_name->identifier) + 1, sizeof(char));
      strcpy(table_def_name, table_name->identifier);
      table_def->name = table_def_name;

      ColDesc* tuple_desc = NULL;
      for (size_t i = 0; i < arrlenu(create->column_defs); ++i) {
        NColumnDef* column_def = (NColumnDef*)create->column_defs[i];
        NIdentifier* col_name = (NIdentifier*)column_def->col_name;

        ColDesc col_desc;
        char* column_name = (char*)calloc(strlen(col_name->identifier) + 1, sizeof(char));
        strcpy(column_name, col_name->identifier);
        col_desc.column_name = column_name;
        col_desc.type = column_def->col_type_id;
        arrpush(tuple_desc, col_desc);
      }

      table_def->tuple_desc = tuple_desc;
      CreateTable(table_def);
      return;
    }
    case NCREATE_INDEX: {
      // TODO: Some of this code should arguably move to the analysis phase since we are
      // duplicating some work. That would potentially require more plumbing though, so avoid
      // doing this for now.
      NCreateIndex* create_index = (NCreateIndex*)query->utility_stmt;

      // Get table def of target table.
      NIdentifier* table_name = (NIdentifier*)create_index->table_name;
      TableDef* table_def = FindTableDef(table_name->identifier);
      assert(table_def != NULL);

      // Build array of columns being indexed.
      size_t* col_idxs = NULL;
      ParseNode** column_list = create_index->column_list;
      assert(column_list != NULL);
      for (size_t i = 0; i < arrlenu(column_list); ++i) {
        NIdentifier* col = (NIdentifier*)column_list[i];
        arrpush(col_idxs, col->idx);
      }

      IndexDef* index_def = CreateBTreeIndex(table_def, col_idxs);

      Cursor cursor;
      CursorInit(&cursor, IndexDefGetParentTableDef(index_def));
      Tuple* tuple = CursorSeekNext(&cursor);
      while (tuple != NULL) {
        BTreeIndexInsert(index_def, tuple);
        tuple = CursorSeekNext(&cursor);
      }
      return;
    }
    default: {
      Panic("Unknown Utility Statement Type");
      return;
    }
  }
}
