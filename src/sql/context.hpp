#ifndef CONTEXT_HH
#define CONTEXT_HH
#include <map>
#include <string>
#include <variant>
#include "parser.hpp"

// Give Flex the prototype of yylex we want ...
#define YY_DECL yy::parser::symbol_type yylex(btdb::sql::ParserContext& context)
// ... and declare it for the parser's sake.
YY_DECL;

namespace btdb {
namespace sql {

// Forward declare buffer state from scanner so parser can use this.
// TODO: Is this the right thing to do?
typedef struct yy_buffer_state* YY_BUFFER_STATE;

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

struct ParserContext {
  RawStmt stmt;
  YY_BUFFER_STATE buffer_state;

  ParserContext(std::string sql) { BeginScan(sql); }

  ~ParserContext() { EndScan(); }

  void BeginScan(std::string sql);
  void EndScan();

  int Parse() {
    yy::parser parse(*this);
    return parse();
  }
};

}  // namespace sql
}  // namespace btdb
#endif  // CONTEXT_HH
