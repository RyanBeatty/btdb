/* simplest version of calculator */
// This code goes in parser.hpp
%code requires {
  #include "node.h"

  // Can't include sql stuff or else we get circular import,
  // so need to forward declare stuff.
  struct Parser;
//   struct ParserContext;
}

%parse-param {
  struct Parser* parser
}

%code{
  #include <assert.h>
  #include <stdlib.h>
  #include <stdbool.h>
  #include <string.h>
  #include <stdio.h>

  #include "stb_ds.h"
  
  #include "sql/driver.h"

  extern int yylex(void);
  // Because we use %parse-param, the signature of yyerror changes.
  void yyerror(Parser*, const char*);
}

//%locations

//%define parse.trace
//%define parse.error verbose

%union {
  char* str_lit;
  bool bool_lit;
  int32_t int_lit;
  ParseNode* node;
  ParseNode** list_node;
  ParseNode*** list_list_node;
  SortDir sort_dir;
}

%define api.token.prefix {TOK_}
%token
    EOF 0
    SCANNER_ERROR
    SELECT
    INSERT
    DELETE
    UPDATE
    INTO
    VALUES
    ORDER
    BY
    ASC
    DESC
    SET
    CREATE
    TABLE
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
%token <str_lit> STRING_GROUP STRING_LITERAL
%token <bool_lit> BOOLEAN_LITERAL
%token <int_lit> INT_LITERAL

// %type <std::vector<std::string>> column_exp
%type <node> expr where_clause select_stmt insert_stmt delete_stmt update_stmt assign_expr sort_clause create_table_stmt
%type <list_node> target_list insert_column_list column_list insert_value_items from_list from_clause update_assign_expr_list table_expr
%type <list_list_node> insert_values_clause insert_values_list 
%type <sort_dir> sort_direction

%%
%start stmt;

stmt:
  select_stmt {
    parser->tree = $1;
  }
  | insert_stmt {
    parser->tree = $1;
  }
  | delete_stmt {
    parser->tree = $1;
  }
  | update_stmt {
    parser->tree = $1;
  }
  | create_table_stmt {
    parser->tree = $1;
  }

select_stmt: SELECT target_list from_clause where_clause sort_clause ";" {
  NSelectStmt* select = (NSelectStmt*)calloc(1, sizeof(NSelectStmt));
  select->type = NSELECT_STMT;
  select->target_list = $2;
  select->from_clause = $3;
  select->where_clause = $4;
  select->sort_clause = $5;
  $$ = (ParseNode*) select;
}

target_list:
  STRING_GROUP { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = $1;

      ParseNode** target_list = NULL;
      arrpush(target_list, (ParseNode*)identifier);
      $$ = target_list;
    }
  | target_list "," STRING_GROUP { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = $3;

      ParseNode** target_list = $1;
      arrpush(target_list, (ParseNode*)identifier);
      $$ = target_list;
    }

from_clause:
  /* empty */ { $$ = NULL; }
  | FROM from_list {
      $$ = $2;
    }

from_list:
  STRING_GROUP { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = $1;

      ParseNode** from_list = NULL;
      arrpush(from_list, (ParseNode*)identifier);
      $$ = from_list;
    }
  | from_list "," STRING_GROUP { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = $3;

      ParseNode** from_list = $1;
      arrpush(from_list, (ParseNode*)identifier);
      $$ = from_list;
    }

where_clause:
  /* empty */ { $$ = NULL; }
  | WHERE expr {
      $$ = $2;
    }

sort_clause:
        { $$ = NULL; }
  | ORDER BY expr sort_direction {
    NSortBy* sort_by = calloc(1, sizeof(NSortBy));
    sort_by->type = NSORTBY;
    sort_by->dir = $4;
    sort_by->sort_expr = $3;
    $$ = (ParseNode*)sort_by; 
  }

sort_direction:
  /* empty */ { $$ = SORT_ASC; }
  | ASC { $$ = SORT_ASC; }
  | DESC { $$ = SORT_DESC; }

%left AND OR;
%left "<" ">" "=" "!=" "<=" ">=";
%left "+" "-";
%left "*" "/";
expr:
  STRING_GROUP {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = $1;
      $$ = (ParseNode*)identifier;
    }
  | STRING_LITERAL {
      // TODO(ryan): Need to remove leading and trailing ' characters. figure out better way.
      size_t len = strlen($1);
      assert(len >= 2);
      NStringLit* str_lit = (NStringLit*)calloc(1, sizeof(NStringLit));
      assert(str_lit != NULL);
      str_lit->type = NSTRING_LIT;
      str_lit->str_lit = (char*)calloc(len - 2 + 1, sizeof(char));
      assert(str_lit->str_lit != NULL);
      strncpy(str_lit->str_lit, $1 + 1, len - 2);
      $$ = (ParseNode*)str_lit;
    }
  | BOOLEAN_LITERAL {
    NBoolLit* bool_lit = (NBoolLit*)calloc(1, sizeof(NBoolLit));
    assert(bool_lit != NULL);
    bool_lit->type = NBOOL_LIT;
    bool_lit->bool_lit = $1;
    $$ = (ParseNode*)bool_lit;
  }
  | expr "=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = EQ;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "!=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = NEQ;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr ">" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = GT;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr ">=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = GE;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "<" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = LT;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "<=" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = LE;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "+" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = PLUS;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "-" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = MINUS;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "*" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = MULT;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr "/" expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = DIV;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }
  | expr AND expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = AND;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }  
  | expr OR expr { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = OR;
      bin_expr->lhs = $1;
      bin_expr->rhs = $3;
      $$ = (ParseNode*)bin_expr;
    }

insert_stmt: INSERT INTO STRING_GROUP insert_column_list insert_values_clause ";" {
  NInsertStmt* insert = (NInsertStmt*) calloc(1, sizeof(NInsertStmt));
  assert(insert != NULL);
  insert->type = NINSERT_STMT;

  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = NIDENTIFIER;
  identifier->identifier = $3;

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
      identifier->type = NIDENTIFIER;
      identifier->identifier = $1;

      ParseNode** column_list = NULL;
      arrpush(column_list, (ParseNode*)identifier);
      $$ = column_list;
   }
  | column_list "," STRING_GROUP {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = $3;

      ParseNode** column_list = $1;
      arrpush(column_list, (ParseNode*)identifier);
      $$ = column_list;
  }

insert_values_clause: VALUES insert_values_list { $$ = $2; }

insert_values_list:
  "(" insert_value_items ")" {
      ParseNode*** values_list = NULL;
      arrpush(values_list, $2);
      $$ = values_list;
  }
  | insert_values_list "," "(" insert_value_items ")" {
    ParseNode*** values_list = $1;
    arrpush(values_list, $4);
    $$ = values_list;
  }

insert_value_items:
  expr {
      ParseNode** value_items = NULL;
      arrpush(value_items, $1);
      $$ = value_items;
  }
  | insert_value_items "," expr {
      ParseNode** value_items = $1;
      arrpush(value_items, $3);
      $$ = value_items;
  }

delete_stmt: DELETE FROM STRING_GROUP where_clause ";" {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = NIDENTIFIER;
  identifier->identifier = $3;

  NDeleteStmt* delete_stmt = (NDeleteStmt*) calloc(1, sizeof(NDeleteStmt));
  delete_stmt->type = NDELETE_STMT;
  delete_stmt->table_name = (ParseNode*) identifier;
  delete_stmt->where_clause = $4;
  $$ = (ParseNode*) delete_stmt;
}

update_stmt: UPDATE STRING_GROUP SET update_assign_expr_list where_clause ";" {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = NIDENTIFIER;
  identifier->identifier = $2;

  NUpdateStmt* update = (NUpdateStmt*) calloc(1, sizeof(NUpdateStmt));
  update->type = NUPDATE_STMT;
  update->table_name = (ParseNode*) identifier;
  update->assign_expr_list = $4;
  update->where_clause = $5;
  $$ = (ParseNode*) update;
}

update_assign_expr_list:
  assign_expr {
      ParseNode** value_items = NULL;
      arrpush(value_items, $1);
      $$ = value_items;
  }
  | update_assign_expr_list "," assign_expr {
      ParseNode** value_items = $1;
      arrpush(value_items, $3);
      $$ = value_items;
  }

// TODO(ryan): Change to handle boollit + other types. Should probably use expr here.
assign_expr: STRING_GROUP "=" expr {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = NIDENTIFIER;
  identifier->identifier = $1;

  NAssignExpr* assign_expr = (NAssignExpr*) calloc(1, sizeof(NAssignExpr));
  assign_expr->type = NASSIGN_EXPR;
  assign_expr->column = (ParseNode*) identifier;
  assign_expr->value_expr = $3;
  $$ = (ParseNode*) assign_expr;
}

create_table_stmt:
  CREATE TABLE STRING_GROUP "(" table_expr ")" ";" {
    NIdentifier* table_name = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(table_name != NULL);
    table_name->type = NIDENTIFIER;
    table_name->identifier = $3;

    NCreateTable* create_table = calloc(1, sizeof(NCreateTable));
    create_table->type = NCREATE_TABLE;
    create_table->table_name = (ParseNode*)table_name;
    create_table->column_defs = $5;
    $$ = (ParseNode*)create_table;
  }

table_expr: 
  STRING_GROUP STRING_GROUP {
    NIdentifier* col_name = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(col_name != NULL);
    col_name->type = NIDENTIFIER;
    col_name->identifier = $1;

    NIdentifier* col_type = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(col_type != NULL);
    col_type->type = NIDENTIFIER;
    col_type->identifier = $2;

    NColumnDef* column_def = calloc(1, sizeof(NColumnDef));
    column_def->type = NCOLUMN_DEF;
    column_def->col_name = (ParseNode*)col_name;
    column_def->col_type = (ParseNode*)col_type;

    ParseNode** column_defs = NULL;
    arrpush(column_defs, (ParseNode*)column_def);
    $$ = column_defs;
  }
  | table_expr "," STRING_GROUP STRING_GROUP {
    NIdentifier* col_name = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(col_name != NULL);
    col_name->type = NIDENTIFIER;
    col_name->identifier = $3;

    NIdentifier* col_type = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(col_type != NULL);
    col_type->type = NIDENTIFIER;
    col_type->identifier = $4;

    NColumnDef* column_def = calloc(1, sizeof(NColumnDef));
    column_def->type = NCOLUMN_DEF;
    column_def->col_name = (ParseNode*)col_name;
    column_def->col_type = (ParseNode*)col_type;

    ParseNode** column_defs = $1;
    arrpush(column_defs, (ParseNode*)column_def);
    $$ = column_defs;
  }


%%

