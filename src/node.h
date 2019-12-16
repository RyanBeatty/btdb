#ifndef NODE_H
#define NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint64_t indent;
} PrintContext;

PrintContext MakePrintContext();
void PrintObject(PrintContext*, const char*);
void EndObject(PrintContext*);
void PrintChild(PrintContext*, const char*, const char*);
void PrintIndent(PrintContext*);
void Indent(PrintContext*);
void Dedent(PrintContext*);

typedef enum ListType { T_PARSENODE, T_LIST } ListType;

typedef struct ListCell {
  void* data;
  struct ListCell* next;
} ListCell;

typedef struct List {
  ListType type;
  uint64_t length;
  ListCell* head;
} List;

List* make_list(ListType);
void push_list(List*, void*);
void free_list(List*);
void print_list(List*, PrintContext*);

#define FOR_EACH(cell, list) for (cell = list->head; cell != NULL; cell = cell->next)

typedef enum ParseNodeType {
  NBIN_EXPR,
  NIDENTIFIER,
  NSTRING_LIT,
  NSELECT_STMT,
  NINSERT_STMT,
  NDELETE_STMT,
  NUPDATE_STMT,
  NASSIGN_EXPR,
  NBOOL_LIT,
  NSORTBY
} ParseNodeType;

typedef enum SortDir {
  SORT_ASC, SORT_DESC
} SortDir;

typedef struct ParseNode {
  ParseNodeType type;
} ParseNode;

typedef enum BinExprOp {
  EQ,
  NEQ,
  GT,
  GE,
  LT,
  LE,
  PLUS,
  MINUS,
  MULT,
  DIV,
  AND,
  OR,
} BinExprOp;

const char* bin_expr_op_to_string(BinExprOp);

typedef struct NBinExpr {
  ParseNodeType type;

  BinExprOp op;
  ParseNode* lhs;
  ParseNode* rhs;
} NBinExpr;

typedef struct NIdentifier {
  ParseNodeType type;

  char* identifier;
} NIdentifier;

typedef struct NStringLit {
  ParseNodeType type;

  char* str_lit;
} NStringLit;

typedef struct NBoolLit {
  ParseNodeType type;

  bool bool_lit;
} NBoolLit;

typedef struct NSelectStmt {
  ParseNodeType type;

  List* target_list;
  ParseNode** from_clause;
  ParseNode* where_clause;
  ParseNode* sort_clause;
} NSelectStmt;

typedef struct NInsertStmt {
  ParseNodeType type;

  ParseNode* table_name;
  ParseNode** column_list;
  ParseNode*** values_list;
} NInsertStmt;

typedef struct NDeleteStmt {
  ParseNodeType type;

  ParseNode* table_name;
  ParseNode* where_clause;
} NDeleteStmt;

typedef struct NAssignExpr {
  ParseNodeType type;

  ParseNode* column;
  ParseNode* value_expr;
} NAssignExpr;

typedef struct NUpdateStmt {
  ParseNodeType type;

  ParseNode* table_name;
  List* assign_expr_list;
  ParseNode* where_clause;
} NUpdateStmt;

typedef struct NSortBy {
  ParseNodeType type;

  SortDir dir;
  ParseNode* sort_expr;
} NSortBy;

void free_parse_node(ParseNode*);
void print_parse_node(ParseNode*, PrintContext*);

#ifdef __cplusplus
}
#endif

#endif  // CONTEXT_HH
