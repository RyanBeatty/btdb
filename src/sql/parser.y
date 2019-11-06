/* simplest version of calculator */
%skeleton "lalr1.cc"
%require "3.4"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

// This code goes in parser.hpp
%code requires {
  #include <memory>

  // Can't include btdb::sql stuff or else we get circular import,
  // so need to forward declare stuff.
  namespace btdb {
    namespace sql {
      struct ParserContext;
      struct WhereClause;
    }}
}

%param {
  btdb::sql::ParserContext& ctx
}

%code{
  #include <stdio.h>
  #include <iostream>
  #include <memory>
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
    WHERE
    EQUALS
;
%token <std::string> STRING_GROUP
%token <std::string> STRING_LITERAL

%type <std::vector<std::string>> column_exp
%type <std::unique_ptr<btdb::sql::WhereClause>> where_clause

%%
%start select_stmt;
select_stmt: SELECT column_exp FROM STRING_GROUP where_clause ";"
{ 
  btdb::sql::SelectStmt sel($2, $4, std::move($5));
  ctx.stmt = std::move(sel);
};

column_exp:
  STRING_GROUP { $$ = std::vector<std::string>{$1}; }
  | STRING_GROUP "," column_exp { $$ = $3; $$.push_back($1); }


where_clause:
  /* empty */ { $$ = nullptr; }
  | WHERE STRING_GROUP EQUALS STRING_LITERAL { 
    $$ = std::make_unique<btdb::sql::WhereClause>(btdb::sql::WhereClause{ $2, $4});
  }


%%

void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
