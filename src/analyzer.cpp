#include "stb_ds.h"

#include "analyzer.h"
#include "collections.h"
#include "sql/context.hpp"
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

  NIdentifier* table_name = (NIdentifier*)select->table_name;
  assert(table_name != NULL);
  assert(table_name->type == NIDENTIFIER);
  assert(table_name->identifier != NULL);

  TableDef* table_def = FindTableDef(table_name->identifier);
  if (table_def == NULL) {
    return NULL;
  }

  // Validate target list contains valid references to columns.
  CharPtrVec* targets = MakeCharPtrVec();
  List* target_list = select->target_list;
  assert(target_list != NULL);
  assert(target_list->type == T_PARSENODE);
  ListCell* lc = NULL;
  FOR_EACH(lc, target_list) {
    assert(lc->data != NULL);
    NIdentifier* col = (NIdentifier*)lc->data;
    assert(col->type == NIDENTIFIER);
    assert(col->identifier != NULL);
    bool found = false;
    for (size_t i = 0; i < arrlen(table_def->tuple_desc); ++i) {
      if (strcmp(table_def->tuple_desc[i].column_name, col->identifier) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      return NULL;
    }
    PushBack(targets, col->identifier);
  }

  if (select->where_clause != NULL) {
    if (CheckType(select->where_clause, *table_def) != T_BOOL) {
      return NULL;
    }
  }

  Query* query = MakeQuery(CMD_SELECT);
  query->table_name = table_name->identifier;
  query->target_list = targets;
  query->where_clause = select->where_clause;
  return query;
}

BType CheckType(ParseNode* node, TableDef& table_def) {
  assert(node != NULL);
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
      for (size_t i = 0; i < arrlen(table_def.tuple_desc); ++i) {
        if (strcmp(table_def.tuple_desc[i].column_name, identifier->identifier) == 0) {
          col_type = &table_def.tuple_desc[i];
          break;
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
      auto lhs_type = CheckType(expr->lhs, table_def);
      auto rhs_type = CheckType(expr->rhs, table_def);
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
  CharPtrVec* targets = MakeCharPtrVec();
  List* target_list = insert->column_list;
  assert(target_list != NULL);
  assert(target_list->type == T_PARSENODE);
  ListCell* lc = NULL;
  FOR_EACH(lc, target_list) {
    assert(lc->data != NULL);
    NIdentifier* col = (NIdentifier*)lc->data;
    assert(col->type == NIDENTIFIER);
    assert(col->identifier != NULL);
    bool found = false;
    for (size_t i = 0; i < arrlen(table_def->tuple_desc); ++i) {
      if (strcmp(table_def->tuple_desc[i].column_name, col->identifier) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      return NULL;
    }
    PushBack(targets, col->identifier);
  }

  Tuple** values = NULL;
  List* values_list = insert->values_list;
  assert(values_list != NULL);
  assert(values_list->type == T_LIST);
  lc = NULL;
  FOR_EACH(lc, values_list) {
    assert(lc->data != NULL);
    List* value_items = (List*)lc->data;
    assert(value_items->type == T_PARSENODE);
    assert(value_items->length == target_list->length);

    Tuple* tuple = NULL;
    uint64_t col_index = 0;
    ListCell* lc2 = NULL;
    FOR_EACH(lc2, value_items) {
      assert(lc2->data != NULL);
      // TODO(ryan): Allow for more general expressions here.
      BType type = CheckType((ParseNode*)lc2->data, *table_def);
      if (type == T_UNKNOWN) {
        return NULL;
      }
      assert(type == T_BOOL || type == T_STRING);
      if (type == T_STRING) {
        NStringLit* str_lit = (NStringLit*)lc2->data;
        assert(str_lit->type == NSTRING_LIT);
        assert(str_lit->str_lit != NULL);
        const char* key = VEC_VALUE(targets, col_index);
        char* str_lit_copy = (char*)calloc(sizeof(char), strlen(str_lit->str_lit));
        tuple = SetCol(tuple, key, MakeDatum(T_STRING, strdup(str_lit->str_lit)));
      } else {
        NBoolLit* bool_lit = (NBoolLit*)lc2->data;
        assert(bool_lit->type == NBOOL_LIT);
        const char* key = VEC_VALUE(targets, col_index);
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

  if (delete_stmt->where_clause != NULL &&
      CheckType(delete_stmt->where_clause, *table_def) != T_BOOL) {
    return NULL;
  }

  Query* query = (Query*)MakeQuery(CMD_DELETE);
  query->table_name = table_name->identifier;
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

  assert(update->assign_expr_list != NULL);
  assert(update->assign_expr_list->type == T_PARSENODE);
  auto* assign_expr_list = update->assign_expr_list;
  ListCell* lc = NULL;
  FOR_EACH(lc, assign_expr_list) {
    assert(lc->data != NULL);
    NAssignExpr* assign_expr = (NAssignExpr*)lc->data;
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

    if (col_type->type != CheckType(assign_expr->value_expr, *table_def)) {
      return NULL;
    }
  }

  if (update->where_clause != NULL && CheckType(update->where_clause, *table_def) != T_BOOL) {
    return NULL;
  }

  Query* query = (Query*)MakeQuery(CMD_UPDATE);
  query->table_name = table_name->identifier;
  query->assign_expr_list = assign_expr_list;
  query->where_clause = update->where_clause;
  return query;
}
