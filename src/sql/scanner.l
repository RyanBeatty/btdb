/* recognize tokens for the calculator and print them out */
%{
  #include <stdbool.h>

  #include "sql/parser.hpp"
  #include "sql/context.hpp"

%}

%option noyywrap nounput noinput batch

/* TODO(ryan): figure out how to do case-insensitive lexing */
%%
"," return yy::parser::make_COMMA();
";" return yy::parser::make_SEMICOLON();
"select" return yy::parser::make_SELECT();
"SELECT" return yy::parser::make_SELECT();
"INSERT" return yy::parser::make_INSERT();
"insert" return yy::parser::make_INSERT();
"INTO" return yy::parser::make_INTO();
"into" return yy::parser::make_INTO();
"VALUES" return yy::parser::make_VALUES();
"values" return yy::parser::make_VALUES();
"from" return yy::parser::make_FROM();
"FROM" return yy::parser::make_FROM();
"DELETE" return yy::parser::make_DELETE();
"delete" return yy::parser::make_DELETE();
"WHERE" return yy::parser::make_WHERE();
"where" return yy::parser::make_WHERE();
"UPDATE" return yy::parser::make_UPDATE();
"update" return yy::parser::make_UPDATE();
"SET" return yy::parser::make_SET();
"set" return yy::parser::make_SET();
"AND" return yy::parser::make_AND();
"and" return yy::parser::make_AND();
"OR" return yy::parser::make_OR();
"or" return yy::parser::make_OR();
"=" return yy::parser::make_EQ();
"!=" return yy::parser::make_NEQ();
">" return yy::parser::make_GT();
">=" return yy::parser::make_GE();
"<" return yy::parser::make_LT();
"<=" return yy::parser::make_LE();
"+" return yy::parser::make_PLUS();
"-" return yy::parser::make_MINUS();
"*" return yy::parser::make_MULT();
"/" return yy::parser::make_DIV();
"(" return yy::parser::make_LPARENS();
")" return yy::parser::make_RPARENS();
"true" return yy::parser::make_BOOLEAN_LITERAL(true);
"false" return yy::parser::make_BOOLEAN_LITERAL(false);
\'(\\.|[^'\\])*\' return yy::parser::make_STRING_LITERAL(yytext);
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
