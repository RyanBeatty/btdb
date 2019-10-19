/* simplest version of calculator */
%skeleton "lalr1.cc"
%require "3.4"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

// This code goes in parser.hpp
%code requires {
  #include "node.hpp"

  using btdb::sql::ParseNode;
  using btdb::sql::ParseTree;
  using btdb::sql::NIdentifier;
  using btdb::sql::NStringLit;

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
  #include <cassert>
  #include <stdlib.h>
  #include <iostream>
  #include <memory>
  #include <string.h>
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

// %type <std::vector<std::string>> column_exp
// %type <std::optional<btdb::sql::NWhereClause>> where_clause
%type <ParseNode*> expr

%%
%start stmt;

stmt: expr {
  ctx.tree = std::make_unique<ParseTree>($1);
}

// %start select_stmt;
// select_stmt: SELECT column_exp FROM STRING_GROUP where_clause ";"
// { 
//   btdb::sql::SelectStmt sel($2, $4, $5);
//   ctx.stmt = sel;
// };

// column_exp:
//   STRING_GROUP { $$ = std::vector<std::string>{$1}; }
//   | STRING_GROUP "," column_exp { $$ = $3; $$.push_back($1); }


// where_clause:
//   /* empty */ { $$ = std::nullopt; }
//   | WHERE expr { 
//     auto where = btdb::sql::NWhereClause(*$2.get());
//     btdb::sql::Node& expr = where;
//     btdb::sql::PrintParseTreeVisitor pp(expr);
//     std::cout << "test: " << pp.PrettyPrint() << std::endl;
//     //$$ = std::make_unique<btdb::sql::WhereClause>(btdb::sql::WhereClause{ $2, $4});
//     $$ = where;
//   }

%left "<" ">" "=" "!=" "<=" ">=";
%left "+" "-";
%left "*" "/";
expr:
  STRING_GROUP {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($1.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $1.c_str(), $1.length());
      $$ = (ParseNode*)identifier;
    }
  | STRING_LITERAL {
      NStringLit* str_lit = (NStringLit*)calloc(1, sizeof(NStringLit));
      assert(str_lit != NULL);
      str_lit->type = btdb::sql::NSTRING_LIT;
      str_lit->str_lit = (char*)calloc($1.length(), sizeof(char));
      assert(str_lit->str_lit != NULL);
      strncpy(str_lit->str_lit, $1.c_str(), $1.length());
      $$ = (ParseNode*)str_lit;
    }
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


%%

void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
