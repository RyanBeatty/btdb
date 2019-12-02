/* recognize tokens for the calculator and print them out */
%{
  #include <stdbool.h>

  #include "sql/parser.hpp"
  #include "sql/context.hpp"

%}

%option noyywrap nounput noinput batch

/* TODO(ryan): figure out how to do case-insensitive lexing */
%%
"," return yy::parser::token::TOK_COMMA;
";" return yy::parser::token::TOK_SEMICOLON;
"select" return yy::parser::token::TOK_SELECT;
"SELECT" return yy::parser::token::TOK_SELECT;
"INSERT" return yy::parser::token::TOK_INSERT;
"insert" return yy::parser::token::TOK_INSERT;
"INTO" return yy::parser::token::TOK_INTO;
"into" return yy::parser::token::TOK_INTO;
"VALUES" return yy::parser::token::TOK_VALUES;
"values" return yy::parser::token::TOK_VALUES;
"from" return yy::parser::token::TOK_FROM;
"FROM" return yy::parser::token::TOK_FROM;
"DELETE" return yy::parser::token::TOK_DELETE;
"delete" return yy::parser::token::TOK_DELETE;
"WHERE" return yy::parser::token::TOK_WHERE;
"where" return yy::parser::token::TOK_WHERE;
"UPDATE" return yy::parser::token::TOK_UPDATE;
"update" return yy::parser::token::TOK_UPDATE;
"SET" return yy::parser::token::TOK_SET;
"set" return yy::parser::token::TOK_SET;
"AND" return yy::parser::token::TOK_AND;
"and" return yy::parser::token::TOK_AND;
"OR" return yy::parser::token::TOK_OR;
"or" return yy::parser::token::TOK_OR;
"=" return yy::parser::token::TOK_EQ;
"!=" return yy::parser::token::TOK_NEQ;
">" return yy::parser::token::TOK_GT;
">=" return yy::parser::token::TOK_GE;
"<" return yy::parser::token::TOK_LT;
"<=" return yy::parser::token::TOK_LE;
"+" return yy::parser::token::TOK_PLUS;
"-" return yy::parser::token::TOK_MINUS;
"*" return yy::parser::token::TOK_MULT;
"/" return yy::parser::token::TOK_DIV;
"(" return yy::parser::token::TOK_LPARENS;
")" return yy::parser::token::TOK_RPARENS;
"true" return yy::parser::make_BOOLEAN_LITERAL(true);
"false" return yy::parser::make_BOOLEAN_LITERAL(false);
\'(\\.|[^'\\])*\' return yy::parser::make_STRING_LITERAL(strdup(yytext));
[a-zA-Z]+ return yy::parser::make_STRING_GROUP(strdup(yytext));
" " {}
<<EOF>> return yy::parser::make_EOF ();
%%

void ParserContext::BeginScan(std::string sql) {
  buffer_state = yy_scan_string(sql.c_str());
}

void ParserContext::EndScan() {
  yy_delete_buffer(buffer_state);
}

