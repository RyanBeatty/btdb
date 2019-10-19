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
  #include "node.hpp"

  // Can't include btdb::sql stuff or else we get circular import,
  // so need to forward declare stuff.
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
    AND
    OR
    EQ "="
    NEQ "!="
    GT ">"
    GE ">="
    LT "<"
    LE "<="
    PLUS "+"
    MINUS "-"
    MULT "*"
    DIV "/"
;
%token <std::string> STRING_GROUP
%token <std::string> STRING_LITERAL

%type <std::vector<std::string>> column_exp
%type <std::unique_ptr<btdb::sql::WhereClause>> where_clause
%type <btdb::sql::NExpr> expr
// %type <void*> term
%type <void*> factor

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
  | WHERE expr { 
    //$$ = std::make_unique<btdb::sql::WhereClause>(btdb::sql::WhereClause{ $2, $4});
    $$ = nullptr;
  }

%left "<" ">" "=" "!=" "<=" ">=";
%left "+" "-";
%left "*" "/";
expr:
  STRING_GROUP { $$ = btdb::sql::NIdentifier($1); }
  | STRING_LITERAL { $$ = btdb::sql::NStringLit($1); }
  // | expr "=" expr { $$ = nullptr; }
  // | expr "!=" expr { $$ = nullptr; }
  // | expr ">" expr { $$ = nullptr; }
  // | expr ">=" expr { $$ = nullptr; }
  // | expr "<" expr { $$ = nullptr; }
  // | expr "<=" expr { $$ = nullptr; }
  // | expr "+" expr { $$ = nullptr; }
  // | expr "-" expr { $$ = nullptr; }
  // | expr "*" expr { $$ = nullptr; }
  // | expr "/" expr { $$ = nullptr; }
  // | expr AND expr { $$ = nullptr; }
  // | expr OR expr { $$ = nullptr; }




// expr:
//   term { $$ = $1; }
//   | term "+" expr { $$ = nullptr; }

// term:
//   factor { $$ = $1; }
//   | factor "*" term { $$ = nullptr; }

factor:
  // column ref case.
  STRING_GROUP { $$ = nullptr; }
  | STRING_LITERAL { $$ = nullptr; }



%%

void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
