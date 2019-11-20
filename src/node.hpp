#ifndef NODE_HH
#define NODE_HH
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <stdbool.h>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace btdb {

struct PrintContext {
  PrintContext() : indent(0) {}
  std::string Print() { return oss.str(); }

  void PrintObject(std::string key) {
    PrintIndent();
    oss << key << ": {" << std::endl;
    Indent();
  }
  void EndObject() {
    PrintIndent();
    oss << "}" << std::endl;
    Dedent();
  }
  void PrintChild(std::string key, std::string val) {
    PrintIndent();
    oss << key << ": " << val << std::endl;
  }
  void PrintIndent() {
    for (uint64_t i = 0; i < indent; ++i) {
      oss << "\t";
    }
  }
  void Indent() { ++indent; }
  void Dedent() { --indent; }

  uint64_t indent;
  std::ostringstream oss;
};

enum ListType { T_PARSENODE, T_LIST };

struct ListCell {
  void* data;
  ListCell* next;
};
static_assert(std::is_pod<ListCell>::value);

struct List {
  ListType type;
  uint64_t length;
  ListCell* head;
};
static_assert(std::is_pod<List>::value);

List* make_list(ListType type);
void push_list(List* list, void* data);
void free_list(List* list);
void print_list(List* list, PrintContext& ctx);

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
static_assert(std::is_pod<ParseNode>::value);

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

std::string bin_expr_op_to_string(BinExprOp op);

struct NBinExpr {
  ParseNodeType type;

  BinExprOp op;
  ParseNode* lhs;
  ParseNode* rhs;
};
static_assert(std::is_pod<NBinExpr>::value);

struct NIdentifier {
  ParseNodeType type;

  char* identifier;
};
static_assert(std::is_pod<NIdentifier>::value);

struct NStringLit {
  ParseNodeType type;

  char* str_lit;
};
static_assert(std::is_pod<NStringLit>::value);

struct NBoolLit {
  ParseNodeType type;

  bool bool_lit;
};
static_assert(std::is_pod<NBoolLit>::value);

struct NSelectStmt {
  ParseNodeType type;

  List* target_list;
  ParseNode* table_name;
  ParseNode* where_clause;
};
static_assert(std::is_pod<NSelectStmt>::value);

struct NInsertStmt {
  ParseNodeType type;

  ParseNode* table_name;
  List* column_list;
  List* values_list;
};
static_assert(std::is_pod<NInsertStmt>::value);

struct NDeleteStmt {
  ParseNodeType type;

  ParseNode* table_name;
  ParseNode* where_clause;
};
static_assert(std::is_pod<NDeleteStmt>::value);

struct NAssignExpr {
  ParseNodeType type;

  ParseNode* column;
  // TODO(ryan): Make arbitrary expression
  ParseNode* value;
};
static_assert(std::is_pod<NAssignExpr>::value);

struct NUpdateStmt {
  ParseNodeType type;

  ParseNode* table_name;
  List* assign_expr_list;
  ParseNode* where_clause;
};
static_assert(std::is_pod<NUpdateStmt>::value);

void free_parse_node(ParseNode* node);
void print_parse_node(ParseNode* node, PrintContext& ctx);

struct ParseTree {
  ParseTree(ParseNode* tree) : tree(tree) {}
  ~ParseTree() { free_parse_node(tree); }

  ParseNode* tree;
};

}  // namespace btdb
#endif  // CONTEXT_HH
