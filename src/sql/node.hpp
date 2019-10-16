#ifndef NODE_HH
#define NODE_HH
#include <map>
#include <string>
#include <variant>
#include "parser.hpp"

namespace btdb {
namespace sql {

struct Node {
};

struct NExpr : Node {
};

struct NBinExpr : NExpr {
  NBinExpr(int op, NExpr& lhs, NExpr& rhs) : op(op), lhs(lhs), rhs(rhs) {}

  int op;
  NExpr& lhs;
  NExpr& rhs;
};

struct NStringLit : NExpr {
  NStringLit(std::string lit) : val(lit) {}

  std::string val;
};

struct NIdentifier : NExpr {
  NIdentifier(std::string identifier) : identifier(identifier) {}

  std::string identifier;
};

struct WhereClause {
  std::string column_name;
  std::string value_name;
};

struct SelectStmt {
  SelectStmt(const std::vector<std::string>& select_lists, const std::string& table_names,
             std::unique_ptr<WhereClause> where_clauses)
      : select_list(select_lists),
        table_name(table_names),
        where_clause(std::move(where_clauses)) {}
  ~SelectStmt() {}
  SelectStmt(SelectStmt& stmt) {
    select_list = stmt.select_list;
    table_name = stmt.table_name;
    where_clause = std::move(stmt.where_clause);
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
  std::unique_ptr<WhereClause> where_clause;
};

typedef std::variant<std::monostate, SelectStmt> RawStmt;

}  // namespace sql
}  // namespace btdb
#endif  // CONTEXT_HH
