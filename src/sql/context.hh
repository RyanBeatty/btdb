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

struct SelectSmt {
  std::vector<std::string> select_list;
  std::string table_name;
};

struct ParserContext {
  SelectSmt result;
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
