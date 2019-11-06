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
  using btdb::sql::NDeleteStmt;
  using btdb::sql::make_list;
  using btdb::sql::push_list;

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
    DELETE
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
%type <ParseNode*> expr where_clause select_stmt from_clause insert_stmt delete_stmt
%type <List*> target_list insert_column_list column_list insert_values_list insert_values_clause insert_value_items

%%
%start stmt;

stmt:
  select_stmt {
    ctx.tree = std::make_unique<ParseTree>($1);
  }
  | insert_stmt {
    ctx.tree = std::make_unique<ParseTree>($1);
  }
  | delete_stmt {
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
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($1.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $1.c_str(), $1.length());

      List* target_list = make_list(btdb::sql::T_PARSENODE);
      push_list(target_list, identifier);
      $$ = target_list;
    }
  | STRING_GROUP "," target_list { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($1.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $1.c_str(), $1.length());

      auto* target_list = $3;
      push_list(target_list, identifier);
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

insert_stmt: INSERT INTO STRING_GROUP insert_column_list insert_values_clause ";" {
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
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($1.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $1.c_str(), $1.length());

      List* column_list = make_list(btdb::sql::T_PARSENODE);
      push_list(column_list, identifier);
      $$ = column_list;
   }
  | column_list "," STRING_GROUP {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc($3.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $3.c_str(), $3.length());

      auto* column_list = $1;
      push_list(column_list, identifier);
      $$ = column_list;
  }

insert_values_clause: VALUES insert_values_list { $$ = $2; }
insert_values_list:
  "(" insert_value_items ")" {
      List* values_list = make_list(btdb::sql::T_LIST);
      push_list(values_list, $2);
      $$ = values_list;
  }
  | insert_values_list "," "(" insert_value_items ")" {
    auto* values_list = $1;
    push_list(values_list, $4);
    $$ = values_list;
  }

insert_value_items:
  expr {
      List* value_items = make_list(btdb::sql::T_PARSENODE);
      push_list(value_items, $1);
      $$ = value_items;
  }
  | insert_value_items "," expr {
      auto* value_items = $1;
      push_list(value_items, $3);
      $$ = value_items;
  }

delete_stmt: DELETE FROM STRING_GROUP ";" {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::sql::NIDENTIFIER;
  identifier->identifier = (char*)calloc($3.length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, $3.c_str(), $3.length());

  NDeleteStmt* delete_stmt = (NDeleteStmt*) calloc(1, sizeof(NDeleteStmt));
  delete_stmt->type = btdb::sql::NDELETE_STMT;
  delete_stmt->table_name = (ParseNode*) identifier;
  $$ = (ParseNode*) delete_stmt;
}


%%

void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
