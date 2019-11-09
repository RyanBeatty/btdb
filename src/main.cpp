#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "node.hpp"
#include "sql/context.hpp"
#include "types.h"
#include "utils.h"

namespace btdb {

struct SystemCatalog {
  std::vector<TableDef> tables;

  bool ValidateParseTree(ParseTree& tree) {
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

  bool ValidateSelectStmt(NSelectStmt* select) {
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

  bool ValidateInsertStmt(NInsertStmt* insert) {
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

  bool ValidateDeleteStmt(NDeleteStmt* delete_stmt) {
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

  bool ValidateUpdateStmt(NUpdateStmt* update) {
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

  BType CheckType(ParseNode* node, TableDef& table_def) {
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
};

struct SelectQuery {
  std::vector<std::string> target_list;
  std::vector<std::string> range_table;
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  ParseNode* where_clause;
};

struct InsertQuery {
  std::string table;
  std::vector<std::string> column_list;
  std::vector<Tuple> values_list;
};

struct DeleteQuery {
  std::string table;
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  ParseNode* where_clause;
};

struct UpdateQuery {
  std::string table_name;
  std::vector<std::vector<std::string>> assign_exprs;
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  ParseNode* where_clause;
};

typedef std::variant<SelectQuery, InsertQuery, DeleteQuery, UpdateQuery> Query;

Query AnalyzeAndRewriteSelectStmt(NSelectStmt* node) {
  assert(node != nullptr);
  assert(node->type == NSELECT_STMT);
  NSelectStmt* select = (NSelectStmt*)node;
  assert(select->table_name != nullptr && select->table_name->type == NIDENTIFIER);
  NIdentifier* identifier = (NIdentifier*)select->table_name;
  auto table_name = std::string(identifier->identifier);

  assert(select->target_list != nullptr);
  assert(select->target_list->type = T_PARSENODE);
  auto* target_list = select->target_list;
  std::vector<std::string> targets;
  ListCell* lc = nullptr;
  FOR_EACH(lc, target_list) {
    assert(lc->data != nullptr);
    NIdentifier* target = (NIdentifier*)lc->data;
    assert(target->type == NIDENTIFIER);
    targets.push_back(target->identifier);
  }

  return SelectQuery{targets, std::vector<std::string>{table_name}, select->where_clause};
}

Query AnalyzeAndRewriteInsertStmt(NInsertStmt* node) {
  assert(node != nullptr);
  assert(node->type == NINSERT_STMT);

  NIdentifier* table_name = (NIdentifier*)node->table_name;
  assert(table_name != nullptr);
  assert(table_name->type == NIDENTIFIER);
  assert(table_name->identifier != nullptr);

  std::vector<std::string> columns;
  auto* column_list = node->column_list;
  assert(column_list != nullptr);
  assert(column_list->type == T_PARSENODE);
  ListCell* lc = nullptr;
  FOR_EACH(lc, column_list) {
    assert(lc->data != nullptr);
    NIdentifier* col = (NIdentifier*)lc->data;
    assert(col->type == NIDENTIFIER);
    assert(col->identifier != nullptr);
    columns.push_back(col->identifier);
  }

  std::vector<Tuple> values;
  auto* values_list = node->values_list;
  assert(values_list->type == T_LIST);
  lc = nullptr;
  FOR_EACH(lc, values_list) {
    assert(lc->data != nullptr);
    List* value_items = (List*)lc->data;
    assert(value_items->type == T_PARSENODE);
    assert(value_items->length == column_list->length);

    Tuple tuple;
    uint64_t col_index = 0;
    ListCell* lc2 = nullptr;
    FOR_EACH(lc2, value_items) {
      assert(lc2->data != nullptr);
      // TODO(ryan): Allow for more general expressions here.
      NStringLit* str_lit = (NStringLit*)lc2->data;
      assert(str_lit->type == NSTRING_LIT);
      assert(str_lit->str_lit != nullptr);
      tuple[columns[col_index]] = str_lit->str_lit;
      ++col_index;
    }
    values.push_back(tuple);
  }

  return InsertQuery{table_name->identifier, columns, values};
}

Query AnalyzeAndRewriteDeleteStmt(NDeleteStmt* delete_stmt) {
  assert(delete_stmt != nullptr);
  assert(delete_stmt->type == NDELETE_STMT);
  assert(delete_stmt->table_name != nullptr);
  assert(delete_stmt->table_name->type == NIDENTIFIER);

  NIdentifier* identifier = (NIdentifier*)delete_stmt->table_name;
  assert(identifier->identifier != nullptr);
  auto table_name = std::string(identifier->identifier);

  return DeleteQuery{table_name, delete_stmt->where_clause};
}

Query AnalyzeAndRewriteUpdateStmt(NUpdateStmt* update) {
  assert(update != nullptr);
  assert(update->type == NUPDATE_STMT);
  assert(update->table_name != nullptr && update->table_name->type == NIDENTIFIER);
  NIdentifier* identifier = (NIdentifier*)update->table_name;
  auto table_name = std::string(identifier->identifier);

  assert(update->assign_expr_list != nullptr);
  assert(update->assign_expr_list->type = T_PARSENODE);
  auto* assign_expr_list = update->assign_expr_list;
  std::vector<std::vector<std::string>> assign_exprs;
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

    NStringLit* str_lit = (NStringLit*)assign_expr->value;
    assert(str_lit->type == NSTRING_LIT);
    assert(str_lit->str_lit != nullptr);

    std::vector<std::string> expr;
    expr.push_back(col->identifier);
    expr.push_back(str_lit->str_lit);
    assign_exprs.push_back(expr);
  }

  return UpdateQuery{table_name, assign_exprs, update->where_clause};
}

Query AnalyzeAndRewriteParseTree(ParseTree& tree) {
  assert(tree.tree != nullptr);
  ParseNode* node = tree.tree;
  switch (node->type) {
    case NSELECT_STMT: {
      return AnalyzeAndRewriteSelectStmt((NSelectStmt*)node);
    }
    case NINSERT_STMT: {
      return AnalyzeAndRewriteInsertStmt((NInsertStmt*)node);
    }
    case NDELETE_STMT: {
      return AnalyzeAndRewriteDeleteStmt((NDeleteStmt*)node);
    }
    case NUPDATE_STMT: {
      return AnalyzeAndRewriteUpdateStmt((NUpdateStmt*)node);
    }
    default: {
      Panic("Invalid statement type when analying");
      // Just return something so the compiler doesn't complain. Fix this later.
      return SelectQuery{};
    }
  }
}

struct Iterator {
  virtual void Open() = 0;
  virtual MTuple GetNext() = 0;
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

Datum ExecPred(ParseNode* node, const Tuple& cur_tuple) {
  switch (node->type) {
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)node;
      assert(str_lit->str_lit != nullptr);
      return Datum(T_STRING, new std::string(str_lit->str_lit));
    }
    case NIDENTIFIER: {
      // TODO(ryan): Not true in the future.
      NIdentifier* identifier = (NIdentifier*)node;
      assert(identifier->identifier != nullptr);
      auto it = cur_tuple.find(identifier->identifier);
      assert(it != cur_tuple.end());
      return Datum(T_STRING, new std::string(it->second));
    }
    case NBIN_EXPR: {
      NBinExpr* expr = (NBinExpr*)node;
      assert(expr->lhs != nullptr);
      assert(expr->rhs != nullptr);
      auto lhs_value = ExecPred(expr->lhs, cur_tuple);
      auto rhs_value = ExecPred(expr->rhs, cur_tuple);
      switch (expr->op) {
        case AND: {
          assert(lhs_value.type == T_BOOL);
          assert(rhs_value.type == T_BOOL);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          bool* lhs_data = (bool*)lhs_value.data;
          bool* rhs_data = (bool*)rhs_value.data;
          return Datum(T_BOOL, new bool(*lhs_data && *rhs_data));
        }
        case OR: {
          assert(lhs_value.type == T_BOOL);
          assert(rhs_value.type == T_BOOL);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          bool* lhs_data = (bool*)lhs_value.data;
          bool* rhs_data = (bool*)rhs_value.data;
          return Datum(T_BOOL, new bool(*lhs_data || *rhs_data));
        }
        case EQ: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data == *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data == *rhs_data));
          } else {
            Panic("Invalid type for eq");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case NEQ: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data != *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data != *rhs_data));
          } else {
            Panic("Invalid type for neq");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case GT: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data > *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data > *rhs_data));
          } else {
            Panic("Invalid type for gt");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case GE: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data >= *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data >= *rhs_data));
          } else {
            Panic("Invalid type for ge");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case LT: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data < *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data < *rhs_data));
          } else {
            Panic("Invalid type for lt");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        case LE: {
          assert(lhs_value.type == T_BOOL || lhs_value.type == T_STRING);
          assert(rhs_value.type == T_BOOL || rhs_value.type == T_STRING);
          assert(lhs_value.data != nullptr);
          assert(rhs_value.data != nullptr);
          if (lhs_value.type == T_BOOL) {
            bool* lhs_data = (bool*)lhs_value.data;
            bool* rhs_data = (bool*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data <= *rhs_data));
          } else if (lhs_value.type == T_STRING) {
            std::string* lhs_data = (std::string*)lhs_value.data;
            std::string* rhs_data = (std::string*)rhs_value.data;
            return Datum(T_BOOL, new bool(*lhs_data <= *rhs_data));
          } else {
            Panic("Invalid type for le");
            return Datum(T_UNKNOWN, nullptr);
          }
        }
        default: {
          Panic("Unknown or Unsupported BinExprOp!");
          return Datum(T_UNKNOWN, nullptr);
        }
      }
    }
    default: {
      Panic("Unknown ParseNode type!");
      return Datum(T_UNKNOWN, nullptr);
    }
  }
}

struct SequentialScan : Iterator {
  uint64_t next_index = 0;

  std::vector<std::string> target_list;
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  ParseNode* where_clause;

  SequentialScan(std::vector<std::string> target_list, ParseNode* where_clause)
      : target_list(target_list), where_clause(where_clause) {}

  void Open() {}
  MTuple GetNext() {
    while (next_index < Tuples.size()) {
      const auto& cur_tpl = Tuples[next_index];
      ++next_index;

      // Evaluate predicate if any.
      if (where_clause != nullptr) {
        auto result_val = ExecPred(where_clause, cur_tpl);
        assert(result_val.type == T_BOOL);
        assert(result_val.data != nullptr);
        bool* result = (bool*)result_val.data;
        if (!*result) {
          continue;
        }
      }

      // Column projections.
      Tuple result_tpl;
      for (const auto& target : target_list) {
        auto it = cur_tpl.find(target);
        assert(it != cur_tpl.end());
        result_tpl[it->first] = it->second;
      }
      return std::make_unique<Tuple>(result_tpl);
    }

    return nullptr;
  }
  void Close() {}
};

struct InsertScan : Iterator {
  std::vector<Tuple> tuples;

  InsertScan(std::vector<Tuple> tuples) : tuples(tuples) {}

  void Open() {}

  MTuple GetNext() {
    if (tuples.size() == 0) {
      return nullptr;
    }
    auto& tuple = tuples.back();
    Tuples.push_back(tuple);
    tuples.pop_back();
    // TODO(ryan): This is sort of wonky, figure out how to do insert scans.
    return std::make_unique<Tuple>(tuple);
  }

  void Close() {}
};

struct DeleteScan : Iterator {
  uint64_t next_index = 0;
  ParseNode* where_clause;

  DeleteScan(ParseNode* where_clause) : where_clause(where_clause) {}

  void Open() {}

  MTuple GetNext() {
    while (next_index < Tuples.size()) {
      const auto& cur_tpl = Tuples[next_index];

      // Evaluate predicate if any.
      if (where_clause != nullptr) {
        auto result_val = ExecPred(where_clause, cur_tpl);
        assert(result_val.type == T_BOOL);
        assert(result_val.data != nullptr);
        bool* result = (bool*)result_val.data;
        if (!*result) {
          ++next_index;
          continue;
        }
      }

      auto result = std::make_unique<Tuple>(cur_tpl);
      Tuples.erase(Tuples.begin() + next_index);
      return result;
    }

    return nullptr;
  }

  void Close() {}
};

struct UpdateScan : Iterator {
  uint64_t next_index = 0;

  std::vector<std::vector<std::string>> assign_exprs;
  // TODO(ryan): Memory will be deallocated in ParseTree desctructor. Figure out how to handle
  // ownership transfer eventually.
  ParseNode* where_clause;

  UpdateScan(std::vector<std::vector<std::string>> assign_exprs, ParseNode* where_clause)
      : assign_exprs(assign_exprs), where_clause(where_clause) {}

  void Open() {}
  MTuple GetNext() {
    while (next_index < Tuples.size()) {
      auto& cur_tpl = Tuples[next_index];
      ++next_index;

      // Evaluate predicate if any.
      if (where_clause != nullptr) {
        auto result_val = ExecPred(where_clause, cur_tpl);
        assert(result_val.type == T_BOOL);
        assert(result_val.data != nullptr);
        bool* result = (bool*)result_val.data;
        if (!*result) {
          continue;
        }
      }

      for (const auto& assign_expr : assign_exprs) {
        cur_tpl[assign_expr[0]] = assign_expr[1];
      }
      return std::make_unique<Tuple>(cur_tpl);
    }

    return nullptr;
  }
  void Close() {}
};

struct PlanState {
  std::vector<std::string> target_list;
  std::unique_ptr<Plan> plan;
};

PlanState PlanQuery(Query& query) {
  PlanState plan_state;
  switch (query.index()) {
    case 0: {
      const SelectQuery& select_query = std::get<SelectQuery>(query);
      auto plan = std::make_unique<SequentialScan>(
          SequentialScan(select_query.target_list, select_query.where_clause));
      plan_state.target_list = select_query.target_list;
      plan_state.plan = std::move(plan);
      break;
    }
    case 1: {
      const InsertQuery& insert_query = std::get<InsertQuery>(query);
      auto plan = std::make_unique<InsertScan>(InsertScan(insert_query.values_list));
      // TODO(ryan): This is also wonky.
      plan_state.target_list = insert_query.column_list;
      plan_state.plan = std::move(plan);
      break;
    }
    case 2: {
      const DeleteQuery& delete_query = std::get<DeleteQuery>(query);
      auto plan = std::make_unique<DeleteScan>(DeleteScan(delete_query.where_clause));
      plan_state.plan = std::move(plan);
      break;
    }
    case 3: {
      const UpdateQuery& update_query = std::get<UpdateQuery>(query);
      auto plan = std::make_unique<UpdateScan>(
          UpdateScan(update_query.assign_exprs, update_query.where_clause));
      plan_state.plan = std::move(plan);
      break;
    }
    default:
      Panic("Unknown Query Type");
  }
  return plan_state;
}

struct Result {
  std::vector<std::string> columns;
  std::vector<MTuple> tuples;
};

Result execute_plan(PlanState& plan_state) {
  Result results;
  results.columns = plan_state.target_list;
  auto* plan = plan_state.plan.get();
  auto mtuple = plan->GetNext();
  while (mtuple != nullptr) {
    results.tuples.push_back(std::move(mtuple));
    mtuple = plan->GetNext();
  }
  return results;
}

}  // namespace btdb

int main() {
  std::cout << "Starting btdb" << std::endl;

  btdb::TableDef table = {"foo", {"bar", "baz"}};
  auto catalog = btdb::SystemCatalog{{table}};
  btdb::Tuple t1;
  t1["bar"] = "hello";
  t1["baz"] = "the quick brown fox";
  btdb::Tuple t2;
  t2["bar"] = "world";
  t2["baz"] = "jumped over the lazy dog";
  btdb::Tuples.push_back(t1);
  btdb::Tuples.push_back(t2);
  while (true) {
    std::cout << "btdb> ";
    std::string line;
    if (!std::getline(std::cin, line)) {
      break;
    }
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    if (line == "\\q") {
      break;
    }
    btdb::sql::ParserContext parser(line);
    if (parser.Parse() != 0) {
      continue;
    }
    auto tree = std::move(parser.tree);
    if (!catalog.ValidateParseTree(*tree.get())) {
      std::cout << "Query not valid" << std::endl;
      continue;
    }
    auto query = btdb::AnalyzeAndRewriteParseTree(*tree.get());
    auto plan_state = btdb::PlanQuery(query);
    auto results = btdb::execute_plan(plan_state);
    for (const auto& column : results.columns) {
      std::cout << column << "\t";
    }
    std::cout << std::endl;
    std::cout << "===============" << std::endl;
    for (auto&& mtuple : results.tuples) {
      assert(mtuple != nullptr);
      for (const auto& column : results.columns) {
        auto it = mtuple->find(column);
        if (it != mtuple->end()) {
          std::cout << it->second;
        }
        std::cout << "\t";
      }
      std::cout << std::endl;
    }
  }
  if (std::cin.bad()) {
    btdb::Panic("I/O Error");
  }

  std::cout << "Shutting down btdb" << std::endl;
  return 0;
}
