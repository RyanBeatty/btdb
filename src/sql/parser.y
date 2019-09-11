/* simplest version of calculator */
%skeleton "lalr1.cc"
%require "3.4"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  struct ParserContext;
}

%param {
  ParserContext& ctx
}

%code{
#include <stdio.h>
#include "context.hh"

// extern int yylex();
//extern int yyparse();

// void yyerror(char *s)
// {
//   fprintf(stderr, "error: %s\n", s);
// }
    // yy::parser::symbol_type yylex (void);
}

//%locations

//%define parse.trace
//%define parse.error verbose

%define api.token.prefix {TOK_}
%token
    EOF 0
    HELLO
;

%%
%start unit;
unit: HELLO { printf("hello"); };

%%

void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}

int main()
{
  ParserContext ctx;
  //yyparse();
}
