#ifndef NODE_H
#define NODE_H
#include <stdbool.h>
#include <memory>

namespace btdb {

struct PrintContext {
  uint64_t indent;
};

PrintContext MakePrintContext();
void PrintObject(PrintContext*, const char*);
void EndObject(PrintContext*);
void PrintChild(PrintContext*, const char*, const char*);
void PrintIndent(PrintContext*);
void Indent(PrintContext*);
void Dedent(PrintContext*);

enum ListType { T_PARSENODE, T_LIST };

struct ListCell {
  void* data;
  ListCell* next;
};

struct List {
  ListType type;
  uint64_t length;
  ListCell* head;
};

List* make_list(ListType);
void push_list(List*, void*);
void free_list(List*);
void print_list(List*, PrintContext*);

#define FOR_EACH(cell, list) for (cell = list->head; cell != nullptr; cell = cell->next)

enum ParseNodeType {
  NBIN_EXPR,
  NIDENTIFIER,
  NSTRING_LIT,
  NSELECT_STMT,
  NINSERT_STMT,
  NDELETE_STMT,
  NUPDATE_STMT,
  NASSIGN_EXPR,
  NBOOL_LIT,
};

struct ParseNode {
  ParseNodeType type;
};

enum BinExprOp {
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
};

const char* bin_expr_op_to_string(BinExprOp);

struct NBinExpr {
  ParseNodeType type;

  BinExprOp op;
  ParseNode* lhs;
  ParseNode* rhs;
};

struct NIdentifier {
  ParseNodeType type;

  char* identifier;
};

struct NStringLit {
  ParseNodeType type;

  char* str_lit;
};

struct NBoolLit {
  ParseNodeType type;

  bool bool_lit;
};

struct NSelectStmt {
  ParseNodeType type;

  List* target_list;
  ParseNode* table_name;
  ParseNode* where_clause;
};

struct NInsertStmt {
  ParseNodeType type;

  ParseNode* table_name;
  List* column_list;
  List* values_list;
};

struct NDeleteStmt {
  ParseNodeType type;

  ParseNode* table_name;
  ParseNode* where_clause;
};

struct NAssignExpr {
  ParseNodeType type;

  ParseNode* column;
  ParseNode* value_expr;
};

struct NUpdateStmt {
  ParseNodeType type;

  ParseNode* table_name;
  List* assign_expr_list;
  ParseNode* where_clause;
};

void free_parse_node(ParseNode*);
void print_parse_node(ParseNode*, PrintContext*);

struct ParseTree {
  ParseTree(ParseNode* tree) : tree(tree) {}
  ~ParseTree() { free_parse_node(tree); }

  ParseNode* tree;
};

}  // namespace btdb
#endif  // CONTEXT_HH
