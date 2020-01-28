#include <assert.h>

#include "stb_ds.h"

#include "analyzer.h"
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
    default: {
      Panic("Unknown statement type when validating");
      return NULL;
    }
  }
}

Query* AnalyzeSelectStmt(NSelectStmt* select) {
  assert(select != NULL);

  ParseNode** from_clause = select->from_clause;
  assert(from_clause != NULL);
  TableDef** join_list = NULL;
  for (size_t i = 0; i < arrlen(from_clause); ++i) {
    NIdentifier* table_name = (NIdentifier*)from_clause[i];
    assert(table_name != NULL);
    assert(table_name->type == NIDENTIFIER);
    assert(table_name->identifier != NULL);

    TableDef* table_def = FindTableDef(table_name->identifier);
    if (table_def == NULL) {
      return NULL;
    }
    arrpush(join_list, table_def);
  }

  // Validate target list contains valid references to columns.
  TargetRef** targets = NULL;
  ParseNode** target_list = select->target_list;
  assert(target_list != NULL);
  for (size_t i = 0; i < arrlen(target_list); ++i) {
    NIdentifier* col = (NIdentifier*)target_list[i];
    assert(col != NULL);
    assert(col->type == NIDENTIFIER);
    assert(col->identifier != NULL);
    TargetRef* ref = NULL;
    for (size_t j = 0; j < arrlen(join_list); ++j) {
      TableDef* table_def = join_list[j];
      for (size_t k = 0; k < arrlen(table_def->tuple_desc); ++k) {
        if (strcmp(table_def->tuple_desc[k].column_name, col->identifier) == 0) {
          ref = (TargetRef*)calloc(1, sizeof(TargetRef));
          ref->column_name = strdup(col->identifier);
          ref->join_list_index = j;
          break;
        }
      }
    }
    if (ref == NULL) {
      return NULL;
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
  return query;
}

BType CheckType(ParseNode* node, TableDef** join_list) {
  assert(node != NULL);
  assert(join_list != NULL);
  switch (node->type) {
    case NSTRING_LIT: {
      return T_STRING;
    }
    case NBOOL_LIT: {
      return T_BOOL;
    }
    case NIDENTIFIER: {
      // TODO(ryan): Not true in the future.
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
        Panic("Invalid column name in bin expr");
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
      switch (expr->op) {
        case AND:
        case OR: {
          if (lhs_type != T_BOOL || rhs_type != T_BOOL) {
            return T_UNKNOWN;
          }
          return T_BOOL;
        }
        case EQ:
        case NEQ: {
          if (lhs_type != rhs_type) {
            return T_UNKNOWN;
          }
          return T_BOOL;
        }
        case GT:
        case GE:
        case LT:
        case LE: {
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

Query* AnalyzeInsertStmt(NInsertStmt* insert) {
  assert(insert != NULL);
  assert(insert->type == NINSERT_STMT);

  NIdentifier* table_name = (NIdentifier*)insert->table_name;
  assert(table_name != NULL);
  assert(table_name->type == NIDENTIFIER);
  assert(table_name->identifier != NULL);

  TableDef* table_def = FindTableDef(table_name->identifier);
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

  Tuple** values = NULL;
  ParseNode*** values_list = insert->values_list;
  assert(values_list != NULL);
  for (size_t i = 0; i < arrlen(values_list); ++i) {
    ParseNode** value_items = values_list[i];
    assert(value_items != NULL);

    if (arrlen(value_items) != arrlen(target_list)) {
      return NULL;
    }

    Tuple* tuple = NULL;
    uint64_t col_index = 0;
    for (size_t j = 0; j < arrlen(value_items); ++j) {
      ParseNode* data = value_items[j];
      assert(data != NULL);
      // TODO(ryan): Allow for more general expressions here.
      // TODO(ryan): Hacky.
      TableDef** defs = NULL;
      arrpush(defs, table_def);
      BType type = CheckType(data, defs);
      if (type == T_UNKNOWN) {
        return NULL;
      }
      assert(type == T_BOOL || type == T_STRING);
      if (type == T_STRING) {
        NStringLit* str_lit = (NStringLit*)data;
        assert(str_lit->type == NSTRING_LIT);
        assert(str_lit->str_lit != NULL);
        const char* key = targets[col_index]->column_name;
        char* str_lit_copy = (char*)calloc(sizeof(char), strlen(str_lit->str_lit));
        tuple = SetCol(tuple, key, MakeDatum(T_STRING, strdup(str_lit->str_lit)));
      } else {
        NBoolLit* bool_lit = (NBoolLit*)data;
        assert(bool_lit->type == NBOOL_LIT);
        const char* key = targets[col_index]->column_name;
        bool* bool_lit_copy = (bool*)calloc(sizeof(bool), 1);
        *bool_lit_copy = bool_lit->bool_lit;
        tuple = SetCol(tuple, key, MakeDatum(T_BOOL, bool_lit_copy));
      }
      ++col_index;
    }
    arrpush(values, CopyTuple(tuple));
  }

  Query* query = MakeQuery(CMD_INSERT);
  query->table_name = table_name->identifier;
  arrpush(query->join_list, table_def);
  query->target_list = targets;
  query->values = values;
  return query;
}

Query* AnalyzeDeleteStmt(NDeleteStmt* delete_stmt) {
  assert(delete_stmt->type == NDELETE_STMT);

  NIdentifier* table_name = (NIdentifier*)delete_stmt->table_name;
  assert(delete_stmt->table_name != NULL);
  assert(delete_stmt->table_name->type == NIDENTIFIER);
  assert(table_name->identifier != NULL);

  TableDef* table_def = FindTableDef(table_name->identifier);
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
  query->table_name = table_name->identifier;
  arrpush(query->join_list, table_def);
  query->where_clause = delete_stmt->where_clause;
  return query;
}

Query* AnalyzeUpdateStmt(NUpdateStmt* update) {
  assert(update != NULL);
  assert(update->type == NUPDATE_STMT);

  NIdentifier* table_name = (NIdentifier*)update->table_name;
  assert(table_name != NULL);
  assert(table_name->type == NIDENTIFIER);
  assert(table_name->identifier != NULL);

  TableDef* table_def = FindTableDef(table_name->identifier);
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
  query->table_name = table_name->identifier;
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
  query->utility_stmt = (ParseNode*) create;
}
