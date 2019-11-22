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

  using btdb::ParseNode;
  using btdb::ParseTree;
  using btdb::NIdentifier;
  using btdb::NStringLit;
  using btdb::NBoolLit;
  using btdb::NBinExpr;
  using btdb::List;
  using btdb::NSelectStmt;
  using btdb::NInsertStmt;
  using btdb::NDeleteStmt;
  using btdb::NUpdateStmt;
  using btdb::NAssignExpr;
  using btdb::make_list;
  using btdb::push_list;

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
  #include <stdbool.h>
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
    UPDATE
    INTO
    VALUES
    SET
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
%token <bool> BOOLEAN_LITERAL

// %type <std::vector<std::string>> column_exp
%type <ParseNode*> expr where_clause select_stmt from_clause insert_stmt delete_stmt update_stmt assign_expr
%type <List*> target_list insert_column_list column_list insert_values_list insert_values_clause insert_value_items update_assign_expr_list

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
  | update_stmt {
    ctx.tree = std::make_unique<ParseTree>($1);
  }

select_stmt: SELECT target_list from_clause where_clause ";" {
  NSelectStmt* select = (NSelectStmt*)calloc(1, sizeof(NSelectStmt));
  select->type = btdb::NSELECT_STMT;
  select->target_list = $2;
  select->table_name = $3;
  select->where_clause = $4;
  $$ = (ParseNode*) select;
}

target_list:
  STRING_GROUP { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::NIDENTIFIER;
      identifier->identifier = (char*)calloc($1.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $1.c_str(), $1.length());

      List* target_list = make_list(btdb::T_PARSENODE);
      target_list->type = btdb::T_PARSENODE;
      push_list(target_list, identifier);
      $$ = target_list;
    }
  | STRING_GROUP "," target_list { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::NIDENTIFIER;
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
      identifier->type = btdb::NIDENTIFIER;
      identifier->identifier = (char*)calloc($2.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $2.c_str(), $2.length());
      $$ = (ParseNode*)identifier;
    }

// from_list:
//   STRING_GROUP {
//       NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
//       assert(identifier != NULL);
//       identifier->type = btdb::NIDENTIFIER;
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
      identifier->type = btdb::NIDENTIFIER;
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
      str_lit->type = btdb::NSTRING_LIT;
      str_lit->str_lit = (char*)calloc($1.length(), sizeof(char));
      assert(str_lit->str_lit != NULL);
      strncpy(str_lit->str_lit, $1.c_str(), $1.length());
      $$ = (ParseNode*)str_lit;
    }
  | BOOLEAN_LITERAL {
    NBoolLit* bool_lit = (NBoolLit*)calloc(1, sizeof(NBoolLit));
    assert(bool_lit != NULL);
    bool_lit->type = btdb::NBOOL_LIT;
    bool_lit->bool_lit = $1;
    $$ = (ParseNode*)bool_lit;
  }
  | expr "=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::EQ;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "!=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::NEQ;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr ">" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::GT;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr ">=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::GE;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "<" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::LT;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "<=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::LE;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "+" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::PLUS;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "-" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::MINUS;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "*" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::MULT;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "/" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::DIV;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr AND expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::AND;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }  
  | expr OR expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::OR;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }

insert_stmt: INSERT INTO STRING_GROUP insert_column_list insert_values_clause ";" {
  NInsertStmt* insert = (NInsertStmt*) calloc(1, sizeof(NInsertStmt));
  assert(insert != nullptr);
  insert->type = btdb::NINSERT_STMT;

  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::NIDENTIFIER;
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
      identifier->type = btdb::NIDENTIFIER;
      identifier->identifier = (char*)calloc($1.length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, $1.c_str(), $1.length());

      List* column_list = make_list(btdb::T_PARSENODE);
      push_list(column_list, identifier);
      $$ = column_list;
   }
  | column_list "," STRING_GROUP {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::NIDENTIFIER;
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
      List* values_list = make_list(btdb::T_LIST);
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
      List* value_items = make_list(btdb::T_PARSENODE);
      push_list(value_items, $1);
      $$ = value_items;
  }
  | insert_value_items "," expr {
      auto* value_items = $1;
      push_list(value_items, $3);
      $$ = value_items;
  }

delete_stmt: DELETE FROM STRING_GROUP where_clause ";" {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::NIDENTIFIER;
  identifier->identifier = (char*)calloc($3.length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, $3.c_str(), $3.length());

  NDeleteStmt* delete_stmt = (NDeleteStmt*) calloc(1, sizeof(NDeleteStmt));
  delete_stmt->type = btdb::NDELETE_STMT;
  delete_stmt->table_name = (ParseNode*) identifier;
  delete_stmt->where_clause = $4;
  $$ = (ParseNode*) delete_stmt;
}

update_stmt: UPDATE STRING_GROUP SET update_assign_expr_list where_clause ";" {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::NIDENTIFIER;
  identifier->identifier = (char*)calloc($2.length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, $2.c_str(), $2.length());

  NUpdateStmt* update = (NUpdateStmt*) calloc(1, sizeof(NUpdateStmt));
  update->type = btdb::NUPDATE_STMT;
  update->table_name = (ParseNode*) identifier;
  update->assign_expr_list = $4;
  update->where_clause = $5;
  $$ = (ParseNode*) update;
}

update_assign_expr_list:
  assign_expr {
      List* value_items = make_list(btdb::T_PARSENODE);
      push_list(value_items, $1);
      $$ = value_items;
  }
  | update_assign_expr_list "," assign_expr {
      auto* value_items = $1;
      push_list(value_items, $3);
      $$ = value_items;
  }

// TODO(ryan): Change to handle boollit + other types. Should probably use expr here.
assign_expr: STRING_GROUP "=" expr {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::NIDENTIFIER;
  identifier->identifier = (char*)calloc($1.length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, $1.c_str(), $1.length());

  // TODO(ryan): Need to remove leading and trailing ' characters. figure out better way.
  // assert($3.length() >= 2);
  // $3 = $3.substr(1, $3.length() - 2);
  // NStringLit* str_lit = (NStringLit*)calloc(1, sizeof(NStringLit));
  // assert(str_lit != NULL);
  // str_lit->type = btdb::NSTRING_LIT;
  // str_lit->str_lit = (char*)calloc($3.length(), sizeof(char));
  // assert(str_lit->str_lit != NULL);
  // strncpy(str_lit->str_lit, $3.c_str(), $3.length());

  NAssignExpr* assign_expr = (NAssignExpr*) calloc(1, sizeof(NAssignExpr));
  assign_expr->type = btdb::NASSIGN_EXPR;
  assign_expr->column = (ParseNode*) identifier;
  assign_expr->value_expr = $3;
  $$ = (ParseNode*) assign_expr;
}


%%

void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
