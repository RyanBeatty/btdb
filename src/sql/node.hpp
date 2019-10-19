#ifndef NODE_HH
#define NODE_HH
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

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
  virtual void visit(Node& node) = 0;

  virtual void visit(NExpr& expr) = 0;
  virtual void visit(NBinExpr& bin_expr) = 0;
  virtual void visit(NStringLit& string_lit) = 0;
  virtual void visit(NIdentifier& identifier) = 0;
  virtual void visit(NWhereClause& where_clause) = 0;
  virtual void visit(NSelect& select) = 0;
};

struct PrintParseTreeVisitor : ParseTreeVisitor {
  PrintParseTreeVisitor(Node& parse_tree);
  std::string PrettyPrint();
  virtual void visit(Node& node) override;
  void visit(NExpr& expr) override;
  void visit(NBinExpr& bin_expr) override;
  void visit(NStringLit& string_lit) override;
  void visit(NIdentifier& id) override;
  void visit(NWhereClause& where_clause) override;
  void visit(NSelect& select) override;

  Node& parse_tree;
  uint64_t ident;
  std::ostringstream oss;
};

struct Node {
  virtual void accept(ParseTreeVisitor& visitor) = 0;
  virtual ~Node(){};
};

struct NExpr : Node {
  virtual void accept(ParseTreeVisitor& visitor) override;
};

struct NBinExpr : NExpr {
  NBinExpr(int op, NExpr& lhs, NExpr& rhs) : op(op), lhs(lhs), rhs(rhs) {}

  virtual void accept(ParseTreeVisitor& visitor) override;
  int op;
  NExpr& lhs;
  NExpr& rhs;
};

struct NStringLit : NExpr {
  NStringLit(std::string lit) : val(lit) {}

  virtual void accept(ParseTreeVisitor& visitor) override;

  std::string val;
};

struct NIdentifier : NExpr {
  NIdentifier(std::string identifier) : identifier(identifier) {}
  virtual void accept(ParseTreeVisitor& visitor) override;

  std::string identifier;
};

struct NWhereClause : Node {
  NWhereClause(NExpr& expr) : expr(expr) {}
  NWhereClause& operator=(const NWhereClause& other) {
    expr = other.expr;
    return *this;
  }
  virtual void accept(ParseTreeVisitor& visitor) override;

  NExpr& expr;
};

struct NSelect : Node {
  virtual void accept(ParseTreeVisitor& visitor) override;

  std::vector<NIdentifier> select_list;
  NIdentifier table_name;
  std::optional<NWhereClause> where_clause;
};

struct WhereClause {};

struct SelectStmt {
  SelectStmt(const std::vector<std::string>& select_lists, const std::string& table_names,
             std::optional<NWhereClause> where_clauses)
      : select_list(select_lists),
        table_name(table_names),
        where_clause(std::move(where_clauses)) {}
  ~SelectStmt() {}
  SelectStmt(SelectStmt& stmt) {
    select_list = stmt.select_list;
    table_name = stmt.table_name;
    where_clause = stmt.where_clause;
  }
  SelectStmt(SelectStmt&& stmt) noexcept
      : select_list(stmt.select_list),
        table_name(stmt.table_name),
        where_clause(std::move(stmt.where_clause)) {}
  SelectStmt& operator=(SelectStmt&& stmt) {
    select_list = stmt.select_list;
    table_name = stmt.table_name;
    where_clause = std::move(stmt.where_clause);
    return *this;
  }

  std::vector<std::string> select_list;
  std::string table_name;
  std::optional<NWhereClause> where_clause;
};

typedef std::variant<std::monostate, SelectStmt> RawStmt;

}  // namespace sql
}  // namespace btdb
#endif  // CONTEXT_HH
