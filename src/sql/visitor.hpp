#ifndef VISITOR_HH
#define VISITOR_HH
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include "parser.hpp"

namespace btdb {
namespace sql {

// Need forward decls in order for ParseTreeVisitor to compile.
struct Node;
struct NExpr;
struct NBinExpr;
struct NStringLit;
struct NIdentifier;
struct NWhereClause;
struct NSelect;

struct ParseTreeVisitor {
  virtual void visit(NExpr& expr) = 0;
  virtual void visit(NBinExpr& bin_expr) = 0;
  virtual void visit(NStringLit& string_lit) = 0;
  virtual void visit(NIdentifier& identifier) = 0;
  virtual void visit(NWhereClause& where_clause) = 0;
  virtual void visit(NSelect& select) = 0;
};

struct PrintParseTreeVisitor : ParseTreeVisitor {
  PrintParseTreeVisitor(Node& parse_tree) : parse_tree(parse_tree), ident(0) {}
  std::string PrettyPrint() {
    parse_tree.accept(*this);
    return oss.str();
  }
  void visit(NExpr& expr) override {}
  void visit(NBinExpr& bin_expr) override {}
  void visit(NStringLit& string_lit) override {
    for (auto i = 0; i <= ident; ++i) {
      oss << "\t";
    }
    oss << "NStringLit: " << string_lit.val << std::endl;
  }
  void visit(NIdentifier& id) override {
    for (auto i = 0; i <= ident; ++i) {
      oss << "\t";
    }
    oss << "NIdentifier: " << id.identifier << std::endl;
  }

  void visit(NWhereClause& where_clause) {}
  void visit(NSelect& select) {}

  Node& parse_tree;
  uint64_t ident;
  std::ostringstream oss;
};

}  // namespace sql
}  // namespace btdb
#endif  // CONTEXT_HH
