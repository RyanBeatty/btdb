#ifndef NODE_HH
#define NODE_HH
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace btdb {
namespace sql {

void Panic(const std::string& msg);

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

enum ListType {
  T_PARSENODE, T_LIST
};

struct ListCell {
  void *data;
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

#define FOR_EACH(cell, list) for(cell = list->head; cell != nullptr; cell = cell->next)

enum ParseNodeType { NBIN_EXPR, NIDENTIFIER, NSTRING_LIT, NSELECT_STMT, NINSERT_STMT };

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

void free_parse_node(ParseNode* node);
void print_parse_node(ParseNode* node, PrintContext& ctx);

struct ParseTree {
  ParseTree(ParseNode* tree) : tree(tree) {}
  ~ParseTree() { free_parse_node(tree); }

  ParseNode* tree;
};

// struct SelectStmt {
//   SelectStmt(const std::vector<std::string>& select_lists, const std::string& table_names,
//              std::optional<NWhereClause> where_clauses)
//       : select_list(select_lists),
//         table_name(table_names),
//         where_clause(std::move(where_clauses)) {}
//   ~SelectStmt() {}
//   SelectStmt(SelectStmt& stmt) {
//     select_list = stmt.select_list;
//     table_name = stmt.table_name;
//     where_clause = stmt.where_clause;
//   }
//   SelectStmt(SelectStmt&& stmt) noexcept
//       : select_list(stmt.select_list),
//         table_name(stmt.table_name),
//         where_clause(std::move(stmt.where_clause)) {}
//   SelectStmt& operator=(SelectStmt&& stmt) {
//     select_list = stmt.select_list;
//     table_name = stmt.table_name;
//     where_clause = std::move(stmt.where_clause);
//     return *this;
//   }

//   std::vector<std::string> select_list;
//   std::string table_name;
//   std::optional<NWhereClause> where_clause;
// };

// typedef std::variant<std::monostate, SelectStmt> RawStmt;

}  // namespace sql
}  // namespace btdb
#endif  // CONTEXT_HH
