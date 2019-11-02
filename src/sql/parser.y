/* simplest version of calculator */
%skeleton "lalr1.cc"
%require "3.4"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

// This code goes in parser.hpp
%code requires {
  #include "sql/node.hpp"

  using btdb::sql::ParseNode;
  using btdb::sql::ParseTree;
  using btdb::sql::NIdentifier;
  using btdb::sql::NStringLit;
  using btdb::sql::NBinExpr;
  using btdb::sql::List;
  using btdb::sql::NSelectStmt;
  using btdb::sql::NInsertStmt;

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
  #include "sql/context.hpp"
}

//%locations

//%define parse.trace
//%define parse.error verbose

%define api.token.prefix {TOK_}
%token
    EOF 0
    SELECT
    INSERT
    INTO
    VALUES
    LPARENS "("
    RPARENS ")"
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
%type <ParseNode*> expr where_clause select_stmt from_clause insert_stmt
%type <List*> target_list insert_column_list column_list insert_values_list values_list

%%
%start stmt;

stmt:
  select_stmt {
    ctx.tree = std::make_unique<ParseTree>($1);
  }
  | insert_stmt {
    ctx.tree = std::make_unique<ParseTree>($1);
  }

select_stmt: SELECT target_list from_clause where_clause ";" {
  NSelectStmt* select = (NSelectStmt*)calloc(1, sizeof(NSelectStmt));
  select->type = btdb::sql::NSELECT_STMT;
  select->target_list = $2;
  select->table_name = $3;
  select->where_clause = $4;
  $$ = (ParseNode*) select;
}

target_list:
  STRING_GROUP { 
      List* target_list = (List*)calloc(1, sizeof(List));
      // TODO: Don't hardcode the size of this;
      target_list->items = (ParseNode**)calloc(10, sizeof(ParseNode*));
      target_list->capacity = 10;

      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($1.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $1.c_str(), $1.length());

      target_list->items[0] = (ParseNode*) identifier;
      target_list->length = 1;
      $$ = target_list;
    }
  | STRING_GROUP "," target_list { 
      auto* target_list = $3;
      assert(target_list->length < target_list->capacity);

      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($1.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $1.c_str(), $1.length());

      target_list->items[target_list->length] = (ParseNode*) identifier;
      target_list->length++;
      // Don't actually think this is neccessary, but it is clear.
      $$ = target_list;
    }

from_clause:
  /* empty */ { $$ = nullptr; }
  | FROM STRING_GROUP {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($2.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $2.c_str(), $2.length());
      $$ = (ParseNode*)identifier;
    }

// from_list:
//   STRING_GROUP {
//       NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
//       assert(identifier != NULL);
//       identifier->type = btdb::sql::NIDENTIFIER;
//       identifier->identifier = (char*)calloc($1.length(), sizeof(char));
//       assert(identifier->identifier != NULL);
//       strncpy(identifier->identifier, $1.c_str(), $1.length());
//       $$ = (ParseNode*)identifier;
//   }
//   | from_list "," STRING_GROUP {

//   }


where_clause:
  /* empty */ { $$ = nullptr; }
  | WHERE expr {
      $$ = $2;
    }

%left AND OR;
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
      // TODO(ryan): Need to remove leading and trailing ' characters. figure out better way.
      assert($1.length() >= 2);
      $1 = $1.substr(1, $1.length() - 2);
      NStringLit* str_lit = (NStringLit*)calloc(1, sizeof(NStringLit));
      assert(str_lit != NULL);
      str_lit->type = btdb::sql::NSTRING_LIT;
      str_lit->str_lit = (char*)calloc($1.length(), sizeof(char));
      assert(str_lit->str_lit != NULL);
      strncpy(str_lit->str_lit, $1.c_str(), $1.length());
      $$ = (ParseNode*)str_lit;
    }
  | expr "=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::EQ;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "!=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::NEQ;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr ">" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::GT;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr ">=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::GE;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "<" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::LT;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "<=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::LE;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "+" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::PLUS;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "-" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::MINUS;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "*" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::MULT;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "/" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::DIV;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr AND expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::AND;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }  
  | expr OR expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::OR;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }

insert_stmt: INSERT INTO STRING_GROUP insert_column_list insert_values_list ";" {
  NInsertStmt* insert = (NInsertStmt*) calloc(1, sizeof(NInsertStmt));
  assert(insert != nullptr);
  insert->type = btdb::sql::NINSERT_STMT;

  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::sql::NIDENTIFIER;
  identifier->identifier = (char*)calloc($3.length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, $3.c_str(), $3.length());

  insert->table_name = (ParseNode*) identifier;
  insert->column_list = $4;
  insert->values_list = $5;

  $$ = (ParseNode*) insert;
}

insert_column_list: "(" column_list ")" { $$ = $2; }

column_list:
   STRING_GROUP {
      List* target_list = (List*)calloc(1, sizeof(List));
      // TODO: Don't hardcode the size of this;
      target_list->items = (ParseNode**)calloc(10, sizeof(ParseNode*));
      target_list->capacity = 10;

      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($1.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $1.c_str(), $1.length());

      target_list->items[0] = (ParseNode*) identifier;
      target_list->length = 1;
      $$ = target_list;
   }
  | column_list "," STRING_GROUP {
      auto* column_list = $1;
      assert(column_list->length < column_list->capacity);

      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($3.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $3.c_str(), $3.length());

      column_list->items[column_list->length] = (ParseNode*) identifier;
      column_list->length++;
      // Don't actually think this is neccessary, but it is clear.
      $$ = column_list;
  }

insert_values_list: VALUES "(" values_list ")" { $$ = $3; }

values_list:
  expr {
      List* target_list = (List*)calloc(1, sizeof(List));
      // TODO: Don't hardcode the size of this;
      target_list->items = (ParseNode**)calloc(10, sizeof(ParseNode*));
      target_list->capacity = 10;

      target_list->items[0] = $1;
      target_list->length = 1;
      $$ = target_list;
  }
  | values_list "," expr {
      auto* values_list = $1;
      assert(values_list->length < values_list->capacity);

      values_list->items[values_list->length] = $3;
      values_list->length++;
      // Don't actually think this is neccessary, but it is clear.
      $$ = values_list;
  }


%%

void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
