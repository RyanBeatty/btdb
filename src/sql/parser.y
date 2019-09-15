/* simplest version of calculator */
%skeleton "lalr1.cc"
%require "3.4"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  namespace btdb {
    namespace sql {
  struct ParserContext;
    }}
}

%param {
  btdb::sql::ParserContext& ctx
}

%code{
  #include <stdio.h>
  #include <iostream>
  #include "context.hpp"
}

//%locations

//%define parse.trace
//%define parse.error verbose

%define api.token.prefix {TOK_}
%token
    EOF 0
    SELECT
    FROM
    SEMICOLON ";"
    COMMA ","
;
%token <std::string> STRING_GROUP

%type <std::vector<std::string>> column_exp

%%
%start select_stmt;
select_stmt: SELECT column_exp FROM STRING_GROUP ";"
{ 
  btdb::sql::SelectStmt sel;
  sel.select_list = $2;
  sel.table_name = $4;
  ctx.stmt = sel;
};

column_exp:
  STRING_GROUP { $$ = std::vector<std::string>{$1}; }
  | STRING_GROUP "," column_exp { $$ = $3; $$.push_back($1); }


%%

void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
