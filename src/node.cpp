#include "node.hpp"
#include "utils.h"

#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace btdb {

List* make_list(ListType type) {
  List* list = (List*)calloc(1, sizeof(List));
  list->type = type;
  return list;
}

void push_list(List* list, void* data) {
  assert(list != nullptr);
  assert(data != nullptr);
  ListCell* cell = (ListCell*)calloc(1, sizeof(ListCell));
  cell->data = data;
  if (list->head == nullptr) {
    list->head = cell;
    list->length = 1;
    return;
  }

  ListCell* ptr = list->head;
  for (; ptr->next != nullptr; ptr = ptr->next) {
  }
  ptr->next = cell;
  ++list->length;
  return;
}

void free_list(List* list) {
  assert(list != nullptr);
  switch (list->type) {
    case T_PARSENODE: {
      for (ListCell* ptr = list->head; ptr != nullptr;) {
        assert(ptr->data != nullptr);
        free_parse_node((ParseNode*)ptr->data);
        ListCell* tmp = ptr;
        ptr = ptr->next;
        free(tmp);
      }
      free(list);
      return;
    }
    case T_LIST: {
      for (ListCell* ptr = list->head; ptr != nullptr;) {
        assert(ptr->data != nullptr);
        free_list((List*)ptr->data);
        ListCell* tmp = ptr;
        ptr = ptr->next;
        free(tmp);
      }
      free(list);
      return;
    }
    default: {
      Panic("Invalid list type when freeing");
      return;
    }
  }
}

void print_list(List* list, PrintContext& ctx) {
  assert(list != nullptr);
  switch (list->type) {
    case T_PARSENODE: {
      for (ListCell* ptr = list->head; ptr != nullptr; ptr = ptr->next) {
        assert(ptr->data != nullptr);
        print_parse_node((ParseNode*)ptr->data, ctx);
      }
      return;
    }
    case T_LIST: {
      uint64_t size = 0;
      for (ListCell* ptr = list->head; ptr != nullptr; ptr = ptr->next, ++size) {
        assert(ptr->data != nullptr);
        ctx.PrintObject("item " + std::to_string(size));
        print_list((List*)ptr->data, ctx);
        ctx.EndObject();
      }
      return;
    }
    default: {
      Panic("Invalid list type when freeing");
      return;
    }
  }
}

void free_parse_node(ParseNode* node) {
  if (node == nullptr) {
    return;
  }

  switch (node->type) {
    case NIDENTIFIER: {
      NIdentifier* identifier = (NIdentifier*)(node);
      assert(identifier->identifier != nullptr);
      free(identifier->identifier);
      free(identifier);
      break;
    }
    case NBIN_EXPR: {
      NBinExpr* bin_expr = (NBinExpr*)(node);
      if (bin_expr->lhs != nullptr) {
        free_parse_node(bin_expr->lhs);
      }
      if (bin_expr->rhs != nullptr) {
        free_parse_node(bin_expr->rhs);
      }
      free(bin_expr);
      break;
    }
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)(node);
      assert(str_lit->str_lit != nullptr);
      free(str_lit->str_lit);
      free(str_lit);
      break;
    }
    case NBOOL_LIT: {
      NBoolLit* bool_lit = (NBoolLit*)(node);
      free(bool_lit);
      break;
    }
    case NSELECT_STMT: {
      NSelectStmt* select = (NSelectStmt*)node;
      assert(select->target_list != nullptr);
      free_list(select->target_list);
      free_parse_node(select->table_name);
      free_parse_node(select->where_clause);
      free(select);
      break;
    }
    case NINSERT_STMT: {
      NInsertStmt* insert = (NInsertStmt*)node;
      assert(insert->table_name != nullptr);
      assert(insert->column_list != nullptr);
      assert(insert->values_list != nullptr);
      free_parse_node(insert->table_name);
      free_list(insert->column_list);
      free_list(insert->values_list);
      free(insert);
      break;
    }
    case NDELETE_STMT: {
      NDeleteStmt* delete_stmt = (NDeleteStmt*)node;
      assert(delete_stmt->table_name != nullptr);
      if (delete_stmt->where_clause != nullptr) {
        free_parse_node(delete_stmt->where_clause);
      }
      free(delete_stmt);
      break;
    }
    case NASSIGN_EXPR: {
      NAssignExpr* assign_expr = (NAssignExpr*)node;
      assert(assign_expr->column != nullptr);
      assert(assign_expr->value != nullptr);
      free_parse_node(assign_expr->column);
      free_parse_node(assign_expr->value);
      free(assign_expr);
      break;
    }
    case NUPDATE_STMT: {
      NUpdateStmt* update = (NUpdateStmt*)node;
      assert(update->table_name != nullptr);
      assert(update->assign_expr_list != nullptr);
      free_parse_node(update->table_name);
      free_list(update->assign_expr_list);
      if (update->where_clause != nullptr) {
        free_parse_node(update->where_clause);
      }
      free(update);
      break;
    }
    default: {
      Panic("Unkown Parse Node Type");
      break;
    }
  }
}

std::string bin_expr_op_to_string(BinExprOp op) {
  switch (op) {
    case EQ: {
      return "EQ";
    }
    case NEQ: {
      return "NEQ";
    }
    case GT: {
      return "GT";
    }
    case GE: {
      return "GE";
    }
    case LT: {
      return "LT";
    }
    case LE: {
      return "LE";
    }
    case PLUS: {
      return "PLUS";
    }
    case MINUS: {
      return "MINUS";
    }
    case MULT: {
      return "MULT";
    }
    case DIV: {
      return "DIV";
    }
    case AND: {
      return "AND";
    }
    case OR: {
      return "OR";
    }
    default: { Panic("Unknown BinExpOp"); }
  }
}

void print_parse_node(ParseNode* node, PrintContext& ctx) {
  if (node == nullptr) {
    return;
  }

  switch (node->type) {
    case NIDENTIFIER: {
      NIdentifier* identifier = (NIdentifier*)(node);
      ctx.PrintObject("NIdentifier");
      ctx.PrintChild("identifier", identifier->identifier);
      ctx.EndObject();
      break;
    }
    case NBIN_EXPR: {
      NBinExpr* bin_expr = (NBinExpr*)(node);
      ctx.PrintObject("NBinExpr");
      ctx.PrintChild("op", bin_expr_op_to_string(bin_expr->op));
      if (bin_expr->lhs != nullptr) {
        ctx.PrintObject("lhs");
        print_parse_node(bin_expr->lhs, ctx);
        ctx.EndObject();
      }
      if (bin_expr->rhs != nullptr) {
        ctx.PrintObject("rhs");
        print_parse_node(bin_expr->rhs, ctx);
        ctx.EndObject();
      }
      ctx.EndObject();
      break;
    }
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)(node);
      ctx.PrintObject("NStringLit");
      ctx.PrintChild("str_lit", str_lit->str_lit);
      ctx.EndObject();
      break;
    }
    case NBOOL_LIT: {
      NBoolLit* bool_lit = (NBoolLit*)(node);
      ctx.PrintObject("NBoolLit");
      ctx.PrintChild("bool_lit", bool_lit->bool_lit ? "true" : "false");
      ctx.EndObject();
      break;
    }
    case NSELECT_STMT: {
      NSelectStmt* select = (NSelectStmt*)node;
      ctx.PrintObject("NSelectStmt");
      assert(select->target_list != nullptr);
      ctx.PrintObject("target_list");
      print_list(select->target_list, ctx);
      ctx.EndObject();
      ctx.PrintObject("table_name");
      print_parse_node(select->table_name, ctx);
      ctx.EndObject();
      ctx.PrintObject("where_clause");
      print_parse_node(select->where_clause, ctx);
      ctx.EndObject();
      ctx.EndObject();
      break;
    }
    case NINSERT_STMT: {
      NInsertStmt* insert = (NInsertStmt*)node;
      assert(insert->table_name != nullptr);
      assert(insert->column_list != nullptr);
      assert(insert->values_list != nullptr);
      ctx.PrintObject("NInsertStmt");
      ctx.PrintObject("table_name");
      print_parse_node(insert->table_name, ctx);
      ctx.EndObject();
      ctx.PrintObject("column_list");
      print_list(insert->column_list, ctx);
      ctx.EndObject();
      ctx.PrintObject("values_list");
      print_list(insert->values_list, ctx);
      ctx.EndObject();
      ctx.EndObject();
      break;
    }
    case NDELETE_STMT: {
      NDeleteStmt* delete_stmt = (NDeleteStmt*)node;
      assert(delete_stmt->table_name != nullptr);
      ctx.PrintObject("NDeleteStmt");
      ctx.PrintObject("table_name");
      print_parse_node(delete_stmt->table_name, ctx);
      ctx.EndObject();
      if (delete_stmt->where_clause != nullptr) {
        ctx.PrintObject("where_clause");
        print_parse_node(delete_stmt->where_clause, ctx);
        ctx.EndObject();
      }
      ctx.EndObject();
      break;
    }
    case NASSIGN_EXPR: {
      NAssignExpr* assign_expr = (NAssignExpr*)node;
      assert(assign_expr->column != nullptr);
      assert(assign_expr->column != nullptr);
      ctx.PrintObject("NAssignExpr");
      ctx.PrintObject("column");
      print_parse_node(assign_expr->column, ctx);
      ctx.EndObject();
      ctx.PrintObject("expr");
      print_parse_node(assign_expr->value, ctx);
      ctx.EndObject();
      ctx.EndObject();
      break;
    }
    case NUPDATE_STMT: {
      NUpdateStmt* update = (NUpdateStmt*)node;
      assert(update->table_name != nullptr);
      assert(update->assign_expr_list != nullptr);
      ctx.PrintObject("NUpdateStmt");
      ctx.PrintObject("table_name");
      print_parse_node(update->table_name, ctx);
      ctx.EndObject();
      ctx.PrintObject("assign_expr_list");
      print_list(update->assign_expr_list, ctx);
      ctx.EndObject();
      if (update->where_clause != nullptr) {
        ctx.PrintObject("where_clause");
        print_parse_node(update->where_clause, ctx);
        ctx.EndObject();
      }
      ctx.EndObject();
      break;
    }
    default: {
      Panic("Unkown Parse Node Type");
      break;
    }
  }
  return;
}

}  // namespace btdb