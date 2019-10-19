#include "node.hpp"

#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace btdb {
namespace sql {

PrintParseTreeVisitor::PrintParseTreeVisitor(Node& parse_tree)
    : parse_tree(parse_tree), ident(0) {}

std::string PrintParseTreeVisitor::PrettyPrint() {
  parse_tree.accept(*this);
  return oss.str();
}
void PrintParseTreeVisitor::visit(NExpr& expr) {}
void PrintParseTreeVisitor::visit(NBinExpr& bin_expr) {}
void PrintParseTreeVisitor::visit(NStringLit& string_lit) {
  for (auto i = 0; i <= ident; ++i) {
    oss << "\t";
  }
  oss << "NStringLit: " << string_lit.val << std::endl;
}
void PrintParseTreeVisitor::visit(NIdentifier& id) {
  for (auto i = 0; i <= ident; ++i) {
    oss << "\t";
  }
  oss << "NIdentifier: " << id.identifier << std::endl;
}

void PrintParseTreeVisitor::visit(NWhereClause& where_clause) {}
void PrintParseTreeVisitor::visit(NSelect& select) {}

void NExpr::accept(ParseTreeVisitor& visitor) { visitor.visit(*this); }

// NBinExpr::NBinExpr(int op, NExpr& lhs, NExpr& rhs) : op(op), lhs(lhs), rhs(rhs) {}
void NBinExpr::accept(ParseTreeVisitor& visitor) { visitor.visit(*this); }

// NStringLit::NStringLit(std::string lit) : val(lit) {}
void NStringLit::accept(ParseTreeVisitor& visitor) { visitor.visit(*this); }

// NIdentifier::NIdentifier(std::string identifier) : identifier(identifier) {}
void NIdentifier::accept(ParseTreeVisitor& visitor) { visitor.visit(*this); }

void NWhereClause::accept(ParseTreeVisitor& visitor) { visitor.visit(*this); }

void NSelect::accept(ParseTreeVisitor& visitor) { visitor.visit(*this); }

// struct WhereClause {};

// struct SelectStmt {
//   SelectStmt(const std::vector<std::string>& select_lists, const std::string& table_names,
//              std::unique_ptr<WhereClause> where_clauses)
//       : select_list(select_lists),
//         table_name(table_names),
//         where_clause(std::move(where_clauses)) {}
//   ~SelectStmt() {}
//   SelectStmt(SelectStmt& stmt) {
//     select_list = stmt.select_list;
//     table_name = stmt.table_name;
//     where_clause = std::move(stmt.where_clause);
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
//   std::unique_ptr<WhereClause> where_clause;
// };

// typedef std::variant<std::monostate, SelectStmt> RawStmt;

}  // namespace sql
}  // namespace btdb