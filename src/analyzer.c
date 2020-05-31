#include "analyzer.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

#include "stb_ds.h"
#include "storage.h"
#include "types.h"
#include "utils.h"

Query* MakeQuery(CmdType cmd) {
  Query* query = (Query*)calloc(1, sizeof(Query));
  query->cmd = cmd;
  return query;
}

Query* AnalyzeParseTree(ParseNode* node) {
  assert(node != NULL);
  switch (node->type) {
    case NSELECT_STMT: {
      return AnalyzeSelectStmt((NSelectStmt*)node);
    }
    case NINSERT_STMT: {
      return AnalyzeInsertStmt((NInsertStmt*)node);
    }
    case NDELETE_STMT: {
      return AnalyzeDeleteStmt((NDeleteStmt*)node);
    }
    case NUPDATE_STMT: {
      return AnalyzeUpdateStmt((NUpdateStmt*)node);
    }
    case NCREATE_TABLE: {
      return AnalyzeCreateTableStmt((NCreateTable*)node);
    }
    default: {
      Panic("Unknown statement type when validating");
      return NULL;
    }
  }
}

Query* AnalyzeSelectStmt(NSelectStmt* select) {
  assert(select != NULL);

  size_t last_table_def_index = 0;
  TableDef** join_list = AnalyzeJoinList(select->from_clause, NULL, &last_table_def_index);
  if (join_list == NULL) {
    return NULL;
  }

  // Validate target list contains valid references to columns.
  TargetRef** targets = NULL;
  ParseNode** target_list = select->target_list;
  assert(target_list != NULL);
  uint64_t anon_cols = 0;  // Need to do this for naming col exprs because all columns must
                           // have unique names. This is stupid.
  for (size_t i = 0; i < arrlen(target_list); ++i) {
    ParseNode* col_expr = target_list[i];
    assert(col_expr != NULL);

    // TODO(ryan): This and the below code do some redundant checking. Clean this up
    // eventually.
    if (CheckType(col_expr, join_list) == T_UNKNOWN) {
      return NULL;
    }

    TargetRef* ref = (TargetRef*)calloc(1, sizeof(TargetRef));
    ref->col_expr = col_expr;
    if (col_expr->type == NIDENTIFIER) {
      NIdentifier* col = (NIdentifier*)target_list[i];
      assert(col->identifier != NULL);
      for (size_t j = 0; j < arrlen(join_list); ++j) {
        TableDef* table_def = join_list[j];
        for (size_t k = 0; k < arrlen(table_def->tuple_desc); ++k) {
          if (strcmp(table_def->tuple_desc[k].column_name, col->identifier) == 0) {
            ref->column_name = strdup(col->identifier);
            ref->join_list_index = j;
            break;
          }
        }
      }
    }
    if (ref->column_name == NULL) {
      // If col_expr is anonymous, need to create name.
      int length = snprintf(NULL, 0, "?column%" PRIu64 "?", anon_cols);
      char* result_col_name = (char*)calloc(length + 1, sizeof(char));
      sprintf(result_col_name, "?column%" PRIu64 "?", anon_cols);
      ref->column_name = result_col_name;
      ++anon_cols;
    }
    arrpush(targets, ref);
  }

  if (select->where_clause != NULL) {
    if (CheckType(select->where_clause, join_list) != T_BOOL) {
      return NULL;
    }
  }

  if (select->sort_clause != NULL) {
    NSortBy* sort_by = (NSortBy*)select->sort_clause;
    assert(sort_by->type == NSORTBY);
    assert(sort_by->sort_expr != NULL);
    NIdentifier* identifier = (NIdentifier*)sort_by->sort_expr;
    assert(identifier->type == NIDENTIFIER);
    assert(identifier->identifier != NULL);
    bool found = false;
    for (size_t i = 0; i < arrlen(join_list); ++i) {
      TableDef* table_def = join_list[i];
      for (size_t j = 0; j < arrlen(table_def->tuple_desc); ++j) {
        if (strcmp(table_def->tuple_desc[j].column_name, identifier->identifier) == 0) {
          found = true;
          break;
        }
      }
    }
    if (!found) {
      return NULL;
    }
  }

  Query* query = MakeQuery(CMD_SELECT);
  query->join_list = join_list;
  query->target_list = targets;
  query->where_clause = select->where_clause;
  query->sort = (NSortBy*)select->sort_clause;
  query->join_tree = select->from_clause;
  return query;
}

BType CheckType(ParseNode* node, TableDef** join_list) {
  assert(node != NULL);
  assert(join_list != NULL);
  switch (node->type) {
    case NLITERAL: {
      NLiteral* literal = (NLiteral*)node;
      return literal->lit_type;
    }
    case NIDENTIFIER: {
      // TODO(ryan): Assuming that first col match is correct col, will not be true in the
      // future.
      NIdentifier* identifier = (NIdentifier*)node;
      assert(identifier->identifier != NULL);

      ColDesc* col_type = NULL;
      for (size_t i = 0; i < arrlen(join_list); ++i) {
        TableDef* table_def = join_list[i];
        for (size_t j = 0; j < arrlen(table_def->tuple_desc); ++j) {
          if (strcmp(table_def->tuple_desc[j].column_name, identifier->identifier) == 0) {
            col_type = &table_def->tuple_desc[j];
            break;
          }
        }
      }
      if (col_type == NULL) {
        return T_UNKNOWN;
      }
      return col_type->type;
    }
    case NBIN_EXPR: {
      NBinExpr* expr = (NBinExpr*)node;
      assert(expr->lhs != NULL);
      assert(expr->rhs != NULL);
      BType lhs_type = CheckType(expr->lhs, join_list);
      BType rhs_type = CheckType(expr->rhs, join_list);
      if (lhs_type == T_UNKNOWN || rhs_type == T_UNKNOWN) {
        return T_UNKNOWN;
      }
      if (lhs_type == T_NULL && rhs_type == T_NULL) {
        return T_UNKNOWN;
      }

      // Null type causes expression type to become whichever is the non null type.
      if (lhs_type == T_NULL) {
        lhs_type = rhs_type;
      } else if (rhs_type == T_NULL) {
        rhs_type = lhs_type;
      }

      NBinExpr* bin_expr = (NBinExpr*)node;
      // ASSUMPTION(ryan): We are assumping expression types must be the same.
      if (lhs_type == T_BOOL) {
        switch (expr->op) {
          case AND: {
            bin_expr->op_func = BoolAnd;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case OR: {
            bin_expr->op_func = BoolOr;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case EQ: {
            bin_expr->op_func = BoolEQ;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case NEQ: {
            bin_expr->op_func = BoolNE;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case GT: {
            bin_expr->op_func = BoolGT;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case GE: {
            bin_expr->op_func = BoolGTE;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case LT: {
            bin_expr->op_func = BoolLT;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case LE: {
            bin_expr->op_func = BoolLTE;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          default: {
            Panic("Unknown or Unsupported BinExprOp for bool!");
            return T_UNKNOWN;
          }
        }
      } else if (lhs_type == T_STRING) {
        switch (expr->op) {
          case EQ: {
            bin_expr->op_func = StrEQ;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case NEQ: {
            bin_expr->op_func = StrNE;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case GT: {
            bin_expr->op_func = StrGT;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case GE: {
            bin_expr->op_func = StrGTE;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case LT: {
            bin_expr->op_func = StrLT;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case LE: {
            bin_expr->op_func = StrLTE;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          default: {
            Panic("Unknown or Unsupported BinExprOp for str!");
            return T_UNKNOWN;
          }
        }
      } else if (lhs_type == T_INT) {
        switch (expr->op) {
          case EQ: {
            bin_expr->op_func = IntEQ;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case NEQ: {
            bin_expr->op_func = IntNE;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case GT: {
            bin_expr->op_func = IntGT;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case GE: {
            bin_expr->op_func = IntGTE;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case LT: {
            bin_expr->op_func = IntLT;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case LE: {
            bin_expr->op_func = IntLTE;
            bin_expr->return_type = T_BOOL;
            return T_BOOL;
          }
          case PLUS: {
            bin_expr->op_func = IntAdd;
            bin_expr->return_type = T_INT;
            return T_INT;
          }
          case MINUS: {
            bin_expr->op_func = IntSub;
            bin_expr->return_type = T_INT;
            return T_INT;
          }
          case MULT: {
            bin_expr->op_func = IntMult;
            bin_expr->return_type = T_INT;
            return T_INT;
          }
          case DIV: {
            bin_expr->op_func = IntDiv;
            bin_expr->return_type = T_INT;
            return T_INT;
          }
          default: {
            Panic("Unknown or Unsupported BinExprOp for int!");
            return T_UNKNOWN;
          }
        }
      } else {
        return T_UNKNOWN;
      }
    }
    default: {
      Panic("Unknown ParseNode type when analyzing expression!");
      return T_UNKNOWN;
    }
  }
}

// TODO: Passing in the last_table_def_index here is unfortunate but mostly a consequence of
// the join tree being two kinds of ParseNodes. I think Ideally I define some other parent
// struct that has both the parse node type and join_list_index fields in the same place.
TableDef** AnalyzeJoinList(ParseNode* node, TableDef** join_list,
                           size_t* last_table_def_index) {
  assert(node != NULL);
  assert(node->type == NJOIN || node->type == NRANGEVAR);
  if (node->type == NJOIN) {
    NJoin* join = (NJoin*)node;
    assert(join->left != NULL);
    size_t left_table_def_index = 0;
    join_list = AnalyzeJoinList(join->left, join_list, &left_table_def_index);
    if (join_list == NULL) {
      return NULL;
    }
    TableDef* left_table_def = join_list[left_table_def_index];
    TableDef* right_table_def = NULL;
    if (join->right != NULL) {
      size_t right_table_def_index = 0;
      join_list = AnalyzeJoinList(join->right, join_list, &right_table_def_index);
      if (join_list == NULL) {
        return NULL;
      }
      right_table_def = join_list[right_table_def_index];
    }

    // Create table def for joined table.
    // TODO(ryan): This code is super messy and breaks a bunch of abstraction barriers I
    // should really fix this at some point.
    ColDesc* tuple_desc = NULL;
    for (size_t j = 0; j < arrlen(left_table_def->tuple_desc); ++j) {
      ColDesc desc = {.column_name = left_table_def->tuple_desc[j].column_name,
                      left_table_def->tuple_desc[j].type};
      arrpush(tuple_desc, desc);
    }
    for (size_t j = 0; j < arrlen(right_table_def->tuple_desc); ++j) {
      ColDesc desc = {.column_name = right_table_def->tuple_desc[j].column_name,
                      right_table_def->tuple_desc[j].type};
      arrpush(tuple_desc, desc);
    }
    TableDef* table_def = calloc(1, sizeof(TableDef));
    table_def->tuple_desc = tuple_desc;
    if (right_table_def != NULL) {
      char* tablename = calloc(
          strlen(left_table_def->name) + strlen(right_table_def->name) + 1, sizeof(char));
      strcat(tablename, left_table_def->name);
      strcat(tablename, right_table_def->name);
      table_def->name = tablename;
    } else {
      char* tablename =
          calloc(strlen(left_table_def->name) + strlen("_intermediate") + 1, sizeof(char));
      strcat(tablename, left_table_def->name);
      strcat(tablename, "intermediate");
      table_def->name = tablename;
    }

    join->join_list_index = arrlen(join_list);
    *last_table_def_index = join->join_list_index;
    arrpush(join_list, table_def);

    // TODO: Also build joined table table def. i.e. If I join table A and B, make tabledef AB.
    // BUG: Shouldn't pass in join_list here, should pass in joined tabledef for this node.
    if (join->qual_cond != NULL && CheckType(join->qual_cond, join_list) != T_BOOL) {
      return NULL;
    }
    return join_list;
  } else {
    NRangeVar* range_var = (NRangeVar*)node;
    assert(range_var != NULL);
    assert(range_var->type == NRANGEVAR);
    assert(range_var->table_name != NULL);

    TableDef* table_def = FindTableDef(range_var->table_name);
    if (table_def == NULL) {
      return NULL;
    }
    range_var->join_list_index = arrlen(join_list);
    *last_table_def_index = range_var->join_list_index;
    arrpush(join_list, table_def);
    return join_list;
  }
}

Query* AnalyzeInsertStmt(NInsertStmt* insert) {
  assert(insert != NULL);
  assert(insert->type == NINSERT_STMT);

  NRangeVar* range_var = (NRangeVar*)insert->range_var;
  assert(range_var != NULL);
  assert(range_var->type == NRANGEVAR);
  assert(range_var->table_name != NULL);

  TableDef* table_def = FindTableDef(range_var->table_name);
  if (table_def == NULL) {
    return NULL;
  }

  // Validate target list contains valid references to columns.
  TargetRef** targets = NULL;
  ParseNode** target_list = insert->column_list;
  assert(target_list != NULL);
  for (size_t i = 0; i < arrlen(target_list); ++i) {
    NIdentifier* col = (NIdentifier*)target_list[i];
    assert(col != NULL);
    assert(col->type == NIDENTIFIER);
    assert(col->identifier != NULL);
    bool found = false;
    TargetRef* ref = NULL;
    for (size_t j = 0; j < arrlen(table_def->tuple_desc); ++j) {
      if (strcmp(table_def->tuple_desc[j].column_name, col->identifier) == 0) {
        ref = (TargetRef*)calloc(1, sizeof(TargetRef));
        ref->column_name = strdup(col->identifier);
        ref->join_list_index = 0;  // Right now the join list for inserts is only one entry.
        break;
      }
    }
    if (ref == NULL) {
      return NULL;
    }
    arrpush(targets, ref);
  }

  ParseNode*** values_list = insert->values_list;
  assert(values_list != NULL);
  for (size_t i = 0; i < arrlen(values_list); ++i) {
    ParseNode** value_items = values_list[i];
    assert(value_items != NULL);

    if (arrlen(value_items) != arrlen(target_list)) {
      return NULL;
    }

    // TODO(ryan): Hacky. I think I should be passing NULL to CheckType anyways.
    TableDef** defs = NULL;
    arrpush(defs, table_def);
    for (size_t j = 0; j < arrlen(value_items); ++j) {
      ParseNode* data = value_items[j];
      assert(data != NULL);
      BType type = CheckType(data, defs);
      // Need to allow NULL values to pass this check.
      if (type == T_UNKNOWN || (type != T_NULL && type != table_def->tuple_desc[j].type)) {
        return NULL;
      }
    }
  }

  Query* query = MakeQuery(CMD_INSERT);
  query->table_name = range_var->table_name;
  arrpush(query->join_list, table_def);
  query->target_list = targets;
  query->values = values_list;
  return query;
}

Query* AnalyzeDeleteStmt(NDeleteStmt* delete_stmt) {
  assert(delete_stmt->type == NDELETE_STMT);

  NRangeVar* range_var = (NRangeVar*)delete_stmt->range_var;
  assert(range_var != NULL);
  assert(range_var->type == NRANGEVAR);
  assert(range_var->table_name != NULL);

  TableDef* table_def = FindTableDef(range_var->table_name);
  if (table_def == NULL) {
    return NULL;
  }

  // TODO: Hacky
  TableDef** defs = NULL;
  arrpush(defs, table_def);
  if (delete_stmt->where_clause != NULL &&
      CheckType(delete_stmt->where_clause, defs) != T_BOOL) {
    return NULL;
  }

  Query* query = (Query*)MakeQuery(CMD_DELETE);
  query->table_name = range_var->table_name;
  arrpush(query->join_list, table_def);
  query->where_clause = delete_stmt->where_clause;
  return query;
}

Query* AnalyzeUpdateStmt(NUpdateStmt* update) {
  assert(update != NULL);
  assert(update->type == NUPDATE_STMT);

  NRangeVar* range_var = (NRangeVar*)update->range_var;
  assert(range_var != NULL);
  assert(range_var->type == NRANGEVAR);
  assert(range_var->table_name != NULL);

  TableDef* table_def = FindTableDef(range_var->table_name);
  if (table_def == NULL) {
    return NULL;
  }

  ParseNode** assign_expr_list = update->assign_expr_list;
  assert(update->assign_expr_list != NULL);
  for (size_t i = 0; i < arrlen(assign_expr_list); ++i) {
    NAssignExpr* assign_expr = (NAssignExpr*)assign_expr_list[i];
    assert(assign_expr != NULL);
    assert(assign_expr->type == NASSIGN_EXPR);
    assert(assign_expr->column != NULL);
    assert(assign_expr->value_expr != NULL);

    NIdentifier* col = (NIdentifier*)assign_expr->column;
    assert(col->type == NIDENTIFIER);
    assert(col->identifier != NULL);
    ColDesc* col_type = NULL;
    for (size_t i = 0; i < arrlen(table_def->tuple_desc); ++i) {
      if (strcmp(table_def->tuple_desc[i].column_name, col->identifier) == 0) {
        col_type = &table_def->tuple_desc[i];
        break;
      }
    }
    if (col_type == NULL) {
      return NULL;
    }

    // TODO: Hacky
    TableDef** defs = NULL;
    arrpush(defs, table_def);
    if (col_type->type != CheckType(assign_expr->value_expr, defs)) {
      return NULL;
    }
  }

  // TODO: Hacky
  TableDef** defs = NULL;
  arrpush(defs, table_def);
  if (update->where_clause != NULL && CheckType(update->where_clause, defs) != T_BOOL) {
    return NULL;
  }

  Query* query = (Query*)MakeQuery(CMD_UPDATE);
  query->table_name = range_var->table_name;
  arrpush(query->join_list, table_def);
  query->assign_expr_list = (NAssignExpr**)assign_expr_list;
  query->where_clause = update->where_clause;
  return query;
}

Query* AnalyzeCreateTableStmt(NCreateTable* create) {
  assert(create != NULL);
  assert(create->type == NCREATE_TABLE);

  NIdentifier* table_name = (NIdentifier*)create->table_name;
  assert(table_name != NULL);
  assert(table_name->type == NIDENTIFIER);
  assert(table_name->identifier != NULL);

  // NOTE: In this case we want to make sure the table doesn't already exist.
  TableDef* table_def = FindTableDef(table_name->identifier);
  if (table_def != NULL) {
    return NULL;
  }

  for (size_t i = 0; i < arrlen(create->column_defs); ++i) {
    NColumnDef* column_def = (NColumnDef*)create->column_defs[i];
    assert(column_def != NULL);
    assert(column_def->type == NCOLUMN_DEF);

    NIdentifier* col_type = (NIdentifier*)column_def->col_type;
    assert(col_type != NULL);
    assert(col_type->type == NIDENTIFIER);
    assert(col_type->identifier != NULL);
    BType col_type_resolved = StringToType(col_type->identifier);
    if (col_type_resolved == T_UNKNOWN) {
      return NULL;
    }
    column_def->col_type_id = col_type_resolved;

    NIdentifier* col_name = (NIdentifier*)column_def->col_name;
    assert(col_name != NULL);
    assert(col_name->type == NIDENTIFIER);
    assert(col_name->identifier != NULL);

    // NOTE(ryan): We assume that every table has unique column names for now.
    for (size_t j = 0; j < arrlen(TableDefs); ++j) {
      TableDef table_def = TableDefs[j];
      for (size_t k = 0; k < arrlen(table_def.tuple_desc); ++k) {
        ColDesc col_desc = table_def.tuple_desc[k];
        if (strcmp(col_name->identifier, col_desc.column_name) == 0) {
          return NULL;
        }
      }
    }
  }

  Query* query = MakeQuery(CMD_UTILITY);
  query->utility_stmt = (ParseNode*)create;
}
