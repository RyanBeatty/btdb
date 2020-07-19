/* recognize tokens for the calculator and print them out */
%{
  #include <stdbool.h>
  #include <assert.h>
  #include <stdio.h>

  #include "parser.h"
  #include "driver.h"
//   #include "sql/context.hpp"

#define YY_DECL int yylex(void)

void yyerror(Parser* parser, const char* msg) {
  fprintf(stderr, "%s\n", msg);
}

%}

%option noyywrap nounput noinput batch

/* TODO(ryan): figure out how to do case-insensitive lexing */
%%
"," return TOK_COMMA;
";" return TOK_SEMICOLON;
"create" return TOK_CREATE;
"CREATE" return TOK_CREATE;
"table" return TOK_TABLE;
"TABLE" return TOK_TABLE;
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
("order"|"ORDER") return TOK_ORDER;
("by"|"BY") return TOK_BY;
("asc"|"ASC") return TOK_ASC;
("desc"|"DESC") return TOK_DESC;
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
"NULL" return TOK_NULL;
"null" return TOK_NULL;
"JOIN" return TOK_JOIN;
"join" return TOK_JOIN;
"LEFT" return TOK_LEFT;
"left" return TOK_LEFT;
"RIGHT" return TOK_RIGHT;
"right" return TOK_RIGHT;
"OUTER" return TOK_OUTER;
"outer" return TOK_OUTER;
"ON" return TOK_ON;
"on" return TOK_ON;
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
-?[0-9]+ {
    char* endptr;
    long int val;
    errno = 0;
    val = strtol(yytext, &endptr, 10);
    // If out of range for int or no digits in string.
    if (errno == ERANGE || (errno != 0 && val == 0)) {
        yyerror(NULL, "numeric literal out of range");
        return TOK_SCANNER_ERROR;
    }

    if (val != (int32_t) val) {
        yyerror(NULL, "numeric literal greater than 32 bytes");
        return TOK_SCANNER_ERROR;
    }
    yylval.int_lit = (int32_t) val;
    return TOK_INT_LITERAL;
}
" " {}
"\n" {}
<<EOF>> return TOK_EOF;
%%

Parser* InitParser(char* sql) {
    Parser* parser = calloc(1, sizeof(Parser));
    parser->buffer_state = yy_scan_string(sql);
    return parser;
}

void FreeParser(Parser* parser) {
    assert(parser != NULL);
    yy_delete_buffer(parser->buffer_state);
    free(parser);
}
