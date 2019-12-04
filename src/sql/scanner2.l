/* recognize tokens for the calculator and print them out */
%{
  #include <stdbool.h>
  #include <assert.h>

  #include "sql/parser2.h"
  #include "sql/driver.h"
//   #include "sql/context.hpp"

#define YY_DECL int yylex(void)

%}

%option noyywrap nounput noinput batch

/* TODO(ryan): figure out how to do case-insensitive lexing */
%%
"," return TOK_COMMA;
";" return TOK_SEMICOLON;
"select" return TOK_SELECT;
"SELECT" return TOK_SELECT;
"INSERT" return TOK_INSERT;
"insert" return TOK_INSERT;
"INTO" return TOK_INTO;
"into" return TOK_INTO;
"VALUES" return TOK_VALUES;
"values" return TOK_VALUES;
"from" return TOK_FROM;
"FROM" return TOK_FROM;
"DELETE" return TOK_DELETE;
"delete" return TOK_DELETE;
"WHERE" return TOK_WHERE;
"where" return TOK_WHERE;
"UPDATE" return TOK_UPDATE;
"update" return TOK_UPDATE;
"SET" return TOK_SET;
"set" return TOK_SET;
"AND" return TOK_AND;
"and" return TOK_AND;
"OR" return TOK_OR;
"or" return TOK_OR;
"=" return TOK_EQ;
"!=" return TOK_NEQ;
">" return TOK_GT;
">=" return TOK_GE;
"<" return TOK_LT;
"<=" return TOK_LE;
"+" return TOK_PLUS;
"-" return TOK_MINUS;
"*" return TOK_MULT;
"/" return TOK_DIV;
"(" return TOK_LPARENS;
")" return TOK_RPARENS;
"true" {
    yylval.bool_lit = true;
    return TOK_BOOLEAN_LITERAL;
}
"false" {
    yylval.bool_lit = false;
    return TOK_BOOLEAN_LITERAL;
}
\'(\\.|[^'\\])*\' {
    yylval.str_lit = strdup(yytext);
    return TOK_STRING_LITERAL;
}
[a-zA-Z]+ {
    yylval.str_lit = strdup(yytext);
    return TOK_STRING_GROUP;
}
" " {}
"\n" {}
<<EOF>> return TOK_EOF;
%%

Parser* InitParser(char* sql) {
    Parser* parser = calloc(1, sizeof(Parser));
    parser->buffer_state = yy_scan_string(sql);
}

void FreeParser(Parser* parser) {
    assert(parser != NULL);
    yy_delete_buffer(parser->buffer_state);
    free(parser);
}

// void ParserContext::BeginScan(std::string sql) {
//   buffer_state = yy_scan_string(sql.c_str());
// }

// void ParserContext::EndScan() {
//   yy_delete_buffer(buffer_state);
// }

