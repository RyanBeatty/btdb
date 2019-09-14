/* recognize tokens for the calculator and print them out */
%{
  #include "parser.hpp"
  #include "context.hpp"

%}

%option noyywrap nounput noinput batch



%%
"," return yy::parser::make_COMMA();
";" return yy::parser::make_SEMICOLON();
"select" return yy::parser::make_SELECT();
"SELECT" return yy::parser::make_SELECT();
"from" return yy::parser::make_FROM();
"FROM" return yy::parser::make_FROM();
[a-zA-Z]+ return yy::parser::make_STRING_GROUP(yytext);
<<EOF>> return yy::parser::make_EOF ();
%%

namespace btdb {
namespace sql {
void ParserContext::BeginScan(std::string sql) {
  buffer_state = yy_scan_string(sql.c_str());
}

void ParserContext::EndScan() {
  yy_delete_buffer(buffer_state);
}

}
}
