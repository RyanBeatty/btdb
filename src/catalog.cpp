#include <algorithm>
#include <string>
#include <vector>

#include "catalog.h"
#include "sql/context.hpp"
#include "types.h"
#include "utils.h"

namespace btdb {

bool SystemCatalog::ValidateParseTree(ParseTree& tree) {
  assert(tree.tree != nullptr);
  ParseNode* node = tree.tree;

  switch (node->type) {
    case NSELECT_STMT: {
      return ValidateSelectStmt((NSelectStmt*)node);
    }
    case NINSERT_STMT: {
      return ValidateInsertStmt((NInsertStmt*)node);
    }
    case NDELETE_STMT: {
      return ValidateDeleteStmt((NDeleteStmt*)node);
    }
    case NUPDATE_STMT: {
      return ValidateUpdateStmt((NUpdateStmt*)node);
    }
    default: {
      Panic("Unknown statement type when validating");
      return false;
    }
  }
}

bool SystemCatalog::ValidateSelectStmt(NSelectStmt* select) {
  assert(select != nullptr);
  assert(select->table_name != nullptr && select->table_name->type == NIDENTIFIER);
  auto* table_id = (NIdentifier*)select->table_name;
  auto table_def_it = tables.begin();
  for (; table_def_it != tables.end(); ++table_def_it) {
    if (table_def_it->name == table_id->identifier) {
      break;
    }
  }
  if (table_def_it == tables.end()) {
    return false;
  }

  // Validate target list contains valid references to columns.
  auto* target_list = select->target_list;
  assert(target_list != nullptr);
  assert(target_list->type == T_PARSENODE);
  ListCell* lc = nullptr;
  FOR_EACH(lc, target_list) {
    assert(lc->data != nullptr);
    NIdentifier* col = (NIdentifier*)lc->data;
    assert(col->type == NIDENTIFIER);
    assert(col->identifier != nullptr);
    if (std::find(table_def_it->col_names.begin(), table_def_it->col_names.end(),
                  col->identifier) == table_def_it->col_names.end()) {
      return false;
    }
  }

  if (select->where_clause != nullptr) {
    if (CheckType(select->where_clause, *table_def_it) != T_BOOL) {
      return false;
    }
  }
  return true;
}

bool SystemCatalog::ValidateInsertStmt(NInsertStmt* insert) {
  assert(insert != nullptr);
  assert(insert->type == NINSERT_STMT);
  assert(insert->table_name != nullptr);
  assert(insert->column_list != nullptr);
  assert(insert->values_list != nullptr);

  // Validate insert table name exists.
  NIdentifier* table_name = (NIdentifier*)insert->table_name;
  assert(table_name->type == NIDENTIFIER);
  assert(table_name->identifier != nullptr);
  auto table_def_it = tables.begin();
  for (; table_def_it != tables.end(); ++table_def_it) {
    if (table_def_it->name == table_name->identifier) {
      break;
    }
  }
  if (table_def_it == tables.end()) {
    return false;
  }

  // Validate column list contains valid references to columns.
  auto* column_list = insert->column_list;
  assert(column_list != nullptr);
  assert(column_list->type == T_PARSENODE);
  ListCell* lc = nullptr;
  FOR_EACH(lc, column_list) {
    assert(lc->data != nullptr);
    NIdentifier* col = (NIdentifier*)lc->data;
    assert(col->type == NIDENTIFIER);
    assert(col->identifier != nullptr);
    if (std::find(table_def_it->col_names.begin(), table_def_it->col_names.end(),
                  col->identifier) == table_def_it->col_names.end()) {
      return false;
    }
  }

  auto* values_list = insert->values_list;
  assert(values_list->type == T_LIST);
  lc = nullptr;
  FOR_EACH(lc, values_list) {
    assert(lc->data != nullptr);
    List* value_items = (List*)lc->data;
    assert(value_items->type == T_PARSENODE);
    if (value_items->length != column_list->length) {
      return false;
    }

    ListCell* lc2 = nullptr;
    FOR_EACH(lc2, value_items) {
      assert(lc2->data != nullptr);
      // TODO(ryan): Allow for more general expressions here.
      NStringLit* str_lit = (NStringLit*)lc2->data;
      if (str_lit->type != NSTRING_LIT) {
        return false;
      }
    }
  }
  return true;
}

bool SystemCatalog::ValidateDeleteStmt(NDeleteStmt* delete_stmt) {
  assert(delete_stmt != nullptr);
  assert(delete_stmt->type == NDELETE_STMT);
  assert(delete_stmt->table_name != nullptr);

  // Validate table name exists and get definition.
  NIdentifier* table_name = (NIdentifier*)delete_stmt->table_name;
  assert(table_name->type == NIDENTIFIER);
  assert(table_name->identifier != nullptr);
  auto table_def_it = tables.begin();
  for (; table_def_it != tables.end(); ++table_def_it) {
    if (table_def_it->name == table_name->identifier) {
      break;
    }
  }
  if (table_def_it == tables.end()) {
    return false;
  }

  if (delete_stmt->where_clause != nullptr) {
    return CheckType(delete_stmt->where_clause, *table_def_it);
  }

  return true;
}

bool SystemCatalog::ValidateUpdateStmt(NUpdateStmt* update) {
  assert(update != nullptr);
  assert(update->type == btdb::NUPDATE_STMT);
  assert(update->table_name != nullptr);
  assert(update->assign_expr_list != nullptr);

  NIdentifier* table_name = (NIdentifier*)update->table_name;
  assert(table_name->identifier != nullptr);
  auto table_def_it = tables.begin();
  for (; table_def_it != tables.end(); ++table_def_it) {
    if (table_def_it->name == table_name->identifier) {
      break;
    }
  }
  if (table_def_it == tables.end()) {
    return false;
  }

  auto* assign_expr_list = update->assign_expr_list;
  assert(assign_expr_list != nullptr);
  assert(assign_expr_list->type == T_PARSENODE);
  ListCell* lc = nullptr;
  FOR_EACH(lc, assign_expr_list) {
    assert(lc->data != nullptr);
    NAssignExpr* assign_expr = (NAssignExpr*)lc->data;
    assert(assign_expr->type == NASSIGN_EXPR);
    assert(assign_expr->column != nullptr);
    assert(assign_expr->value != nullptr);

    NIdentifier* col = (NIdentifier*)assign_expr->column;
    assert(col->type == NIDENTIFIER);
    assert(col->identifier != nullptr);
    if (std::find(table_def_it->col_names.begin(), table_def_it->col_names.end(),
                  col->identifier) == table_def_it->col_names.end()) {
      return false;
    }

    NStringLit* str_lit = (NStringLit*)assign_expr->value;
    if (str_lit->type != NSTRING_LIT) {
      return false;
    }
    assert(str_lit->str_lit != nullptr);
  }

  if (update->where_clause != nullptr) {
    if (CheckType(update->where_clause, *table_def_it) != T_BOOL) {
      return false;
    }
  }
  return true;
}

BType SystemCatalog::CheckType(ParseNode* node, TableDef& table_def) {
  assert(node != nullptr);
  switch (node->type) {
    case NSTRING_LIT: {
      return T_STRING;
    }
    case NIDENTIFIER: {
      // TODO(ryan): Not true in the future.
      NIdentifier* identifier = (NIdentifier*)node;
      assert(identifier->identifier != nullptr);
      if (std::find(table_def.col_names.begin(), table_def.col_names.end(),
                    identifier->identifier) == table_def.col_names.end()) {
        Panic("Invalid column name in bin expr");
      }
      return T_STRING;
    }
    case NBIN_EXPR: {
      NBinExpr* expr = (NBinExpr*)node;
      assert(expr->lhs != nullptr);
      assert(expr->rhs != nullptr);
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

}  // namespace btdb
