#include "node.h"
#include "utils.h"

#include "stb_ds.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PrintContext MakePrintContext() {
  PrintContext ctx = {0};
  return ctx;
}
void PrintObject(PrintContext* ctx, const char* key) {
  PrintIndent(ctx);
  printf("%s: {\n", key);
  Indent(ctx);
}

void EndObject(PrintContext* ctx) {
  PrintIndent(ctx);
  printf("}\n");
  Dedent(ctx);
}

void PrintChild(PrintContext* ctx, const char* key, const char* val) {
  PrintIndent(ctx);
  printf("%s: %s\n", key, val);
}

void PrintIndent(PrintContext* ctx) {
  for (uint64_t i = 0; i < ctx->indent; ++i) {
    printf("\t");
  }
}
void Indent(PrintContext* ctx) { ++ctx->indent; }
void Dedent(PrintContext* ctx) { --ctx->indent; }

List* make_list(ListType type) {
  List* list = (List*)calloc(1, sizeof(List));
  list->type = type;
  return list;
}

void push_list(List* list, void* data) {
  assert(list != NULL);
  assert(data != NULL);
  ListCell* cell = (ListCell*)calloc(1, sizeof(ListCell));
  cell->data = data;
  if (list->head == NULL) {
    list->head = cell;
    list->length = 1;
    return;
  }

  ListCell* ptr = list->head;
  for (; ptr->next != NULL; ptr = ptr->next) {
  }
  ptr->next = cell;
  ++list->length;
  return;
}

void free_list(List* list) {
  assert(list != NULL);
  switch (list->type) {
    case T_PARSENODE: {
      for (ListCell* ptr = list->head; ptr != NULL;) {
        assert(ptr->data != NULL);
        free_parse_node((ParseNode*)ptr->data);
        ListCell* tmp = ptr;
        ptr = ptr->next;
        free(tmp);
      }
      free(list);
      return;
    }
    case T_LIST: {
      for (ListCell* ptr = list->head; ptr != NULL;) {
        assert(ptr->data != NULL);
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

void free_parse_node_list(ParseNode** list) {
  if (list == NULL) {
    return ;
  }
  for (size_t i = 0; i < arrlen(list); ++i) {
    free_parse_node(list[i]);
  }
  return;
}

void print_list(List* list, PrintContext* ctx) {
  assert(list != NULL);
  switch (list->type) {
    case T_PARSENODE: {
      for (ListCell* ptr = list->head; ptr != NULL; ptr = ptr->next) {
        assert(ptr->data != NULL);
        print_parse_node((ParseNode*)ptr->data, ctx);
      }
      return;
    }
    case T_LIST: {
      uint64_t size = 0;
      for (ListCell* ptr = list->head; ptr != NULL; ptr = ptr->next, ++size) {
        assert(ptr->data != NULL);
        char str[100];
        memset(str, 0, 100);
        sprintf(str, "item %" PRIu64, size);
        PrintObject(ctx, str);
        print_list((List*)ptr->data, ctx);
        EndObject(ctx);
      }
      return;
    }
    default: {
      Panic("Invalid list type when freeing");
      return;
    }
  }
}

void print_parse_node_list(ParseNode** list, PrintContext* ctx) {
  assert(list != NULL);
  for (size_t i = 0; i < arrlen(list); ++i) {
    assert(list[i] != NULL);
    print_parse_node(list[i], ctx);
  }
}

void free_parse_node(ParseNode* node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
    case NIDENTIFIER: {
      NIdentifier* identifier = (NIdentifier*)(node);
      assert(identifier->identifier != NULL);
      free(identifier->identifier);
      free(identifier);
      break;
    }
    case NBIN_EXPR: {
      NBinExpr* bin_expr = (NBinExpr*)(node);
      if (bin_expr->lhs != NULL) {
        free_parse_node(bin_expr->lhs);
      }
      if (bin_expr->rhs != NULL) {
        free_parse_node(bin_expr->rhs);
      }
      free(bin_expr);
      break;
    }
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)(node);
      assert(str_lit->str_lit != NULL);
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
      assert(select->target_list != NULL);
      free_list(select->target_list);
      free_list(select->from_clause);
      free_parse_node(select->where_clause);
      free_parse_node(select->sort_clause);
      free(select);
      break;
    }
    case NINSERT_STMT: {
      NInsertStmt* insert = (NInsertStmt*)node;
      assert(insert->table_name != NULL);
      assert(insert->column_list != NULL);
      assert(insert->values_list != NULL);
      free_parse_node(insert->table_name);
      free_parse_node_list(insert->column_list);
      free_list(insert->values_list);
      free(insert);
      break;
    }
    case NDELETE_STMT: {
      NDeleteStmt* delete_stmt = (NDeleteStmt*)node;
      assert(delete_stmt->table_name != NULL);
      if (delete_stmt->where_clause != NULL) {
        free_parse_node(delete_stmt->where_clause);
      }
      free(delete_stmt);
      break;
    }
    case NASSIGN_EXPR: {
      NAssignExpr* assign_expr = (NAssignExpr*)node;
      assert(assign_expr->column != NULL);
      assert(assign_expr->value_expr != NULL);
      free_parse_node(assign_expr->column);
      free_parse_node(assign_expr->value_expr);
      free(assign_expr);
      break;
    }
    case NUPDATE_STMT: {
      NUpdateStmt* update = (NUpdateStmt*)node;
      assert(update->table_name != NULL);
      assert(update->assign_expr_list != NULL);
      free_parse_node(update->table_name);
      free_list(update->assign_expr_list);
      if (update->where_clause != NULL) {
        free_parse_node(update->where_clause);
      }
      free(update);
      break;
    }
    case NSORTBY: {
      NSortBy* sort_by = (NSortBy*) node;
      assert(sort_by->sort_expr != NULL);
      free_parse_node(sort_by->sort_expr);
      free(sort_by);
      break;
    }
    default: {
      Panic("Unkown Parse Node Type when freeing");
      break;
    }
  }
}

const char* bin_expr_op_to_string(BinExprOp op) {
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

const char* sort_dir_to_string(SortDir dir) {
  switch(dir) {
    case SORT_ASC:
      return "ASC";
    case SORT_DESC:
      return "DESC";
    default:
      Panic("Unknown SortDir");
  }
}

void print_parse_node(ParseNode* node, PrintContext* ctx) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
    case NIDENTIFIER: {
      NIdentifier* identifier = (NIdentifier*)(node);
      PrintObject(ctx, "NIdentifier");
      PrintChild(ctx, "identifier", identifier->identifier);
      EndObject(ctx);
      break;
    }
    case NBIN_EXPR: {
      NBinExpr* bin_expr = (NBinExpr*)(node);
      PrintObject(ctx, "NBinExpr");
      PrintChild(ctx, "op", bin_expr_op_to_string(bin_expr->op));
      if (bin_expr->lhs != NULL) {
        PrintObject(ctx, "lhs");
        print_parse_node(bin_expr->lhs, ctx);
        EndObject(ctx);
      }
      if (bin_expr->rhs != NULL) {
        PrintObject(ctx, "rhs");
        print_parse_node(bin_expr->rhs, ctx);
        EndObject(ctx);
      }
      EndObject(ctx);
      break;
    }
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)(node);
      PrintObject(ctx, "NStringLit");
      PrintChild(ctx, "str_lit", str_lit->str_lit);
      EndObject(ctx);
      break;
    }
    case NBOOL_LIT: {
      NBoolLit* bool_lit = (NBoolLit*)(node);
      PrintObject(ctx, "NBoolLit");
      PrintChild(ctx, "bool_lit", bool_lit->bool_lit ? "true" : "false");
      EndObject(ctx);
      break;
    }
    case NSELECT_STMT: {
      NSelectStmt* select = (NSelectStmt*)node;
      PrintObject(ctx, "NSelectStmt");
      assert(select->target_list != NULL);
      PrintObject(ctx, "target_list");
      print_list(select->target_list, ctx);
      EndObject(ctx);
      PrintObject(ctx, "from_clause");
      print_list(select->from_clause, ctx);
      EndObject(ctx);
      PrintObject(ctx, "where_clause");
      print_parse_node(select->where_clause, ctx);
      EndObject(ctx);
      PrintObject(ctx, "sort_clause");
      print_parse_node(select->sort_clause, ctx);
      EndObject(ctx);
      EndObject(ctx);
      break;
    }
    case NINSERT_STMT: {
      NInsertStmt* insert = (NInsertStmt*)node;
      assert(insert->table_name != NULL);
      assert(insert->column_list != NULL);
      assert(insert->values_list != NULL);
      PrintObject(ctx, "NInsertStmt");
      PrintObject(ctx, "table_name");
      print_parse_node(insert->table_name, ctx);
      EndObject(ctx);
      PrintObject(ctx, "column_list");
      print_parse_node_list(insert->column_list, ctx);
      EndObject(ctx);
      PrintObject(ctx, "values_list");
      print_list(insert->values_list, ctx);
      EndObject(ctx);
      EndObject(ctx);
      break;
    }
    case NDELETE_STMT: {
      NDeleteStmt* delete_stmt = (NDeleteStmt*)node;
      assert(delete_stmt->table_name != NULL);
      PrintObject(ctx, "NDeleteStmt");
      PrintObject(ctx, "table_name");
      print_parse_node(delete_stmt->table_name, ctx);
      EndObject(ctx);
      if (delete_stmt->where_clause != NULL) {
        PrintObject(ctx, "where_clause");
        print_parse_node(delete_stmt->where_clause, ctx);
        EndObject(ctx);
      }
      EndObject(ctx);
      break;
    }
    case NASSIGN_EXPR: {
      NAssignExpr* assign_expr = (NAssignExpr*)node;
      assert(assign_expr->column != NULL);
      assert(assign_expr->column != NULL);
      PrintObject(ctx, "NAssignExpr");
      PrintObject(ctx, "column");
      print_parse_node(assign_expr->column, ctx);
      EndObject(ctx);
      PrintObject(ctx, "value_expr");
      print_parse_node(assign_expr->value_expr, ctx);
      EndObject(ctx);
      EndObject(ctx);
      break;
    }
    case NUPDATE_STMT: {
      NUpdateStmt* update = (NUpdateStmt*)node;
      assert(update->table_name != NULL);
      assert(update->assign_expr_list != NULL);
      PrintObject(ctx, "NUpdateStmt");
      PrintObject(ctx, "table_name");
      print_parse_node(update->table_name, ctx);
      EndObject(ctx);
      PrintObject(ctx, "assign_expr_list");
      print_list(update->assign_expr_list, ctx);
      EndObject(ctx);
      if (update->where_clause != NULL) {
        PrintObject(ctx, "where_clause");
        print_parse_node(update->where_clause, ctx);
        EndObject(ctx);
      }
      EndObject(ctx);
      break;
    }
    case NSORTBY: {
      NSortBy* sort_by = (NSortBy*)node;
      assert(sort_by->sort_expr != NULL);
      PrintObject(ctx, "NSortBy");
      PrintChild(ctx, "dir", sort_dir_to_string(sort_by->dir));
      PrintObject(ctx, "sort_expr");
      print_parse_node(sort_by->sort_expr, ctx);
      EndObject(ctx);
      EndObject(ctx);
      break;
    }
    default: {
      Panic("Unkown Parse Node Type when printing");
      break;
    }
  }
  return;
}
