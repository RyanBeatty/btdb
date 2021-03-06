#ifndef NODE_H
#define NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "types.h"

typedef struct {
  uint64_t indent;
} PrintContext;

PrintContext MakePrintContext();
void PrintObject(PrintContext*, const char*);
void EndObject(PrintContext*);
void PrintChild(PrintContext*, const char*, const char*);
void PrintIndent(PrintContext*);
void Indent(PrintContext*);
void Dedent(PrintContext*);

typedef enum ParseNodeType {
  NBIN_EXPR,
  NIDENTIFIER,
  NSELECT_STMT,
  NINSERT_STMT,
  NDELETE_STMT,
  NUPDATE_STMT,
  NASSIGN_EXPR,
  NSORTBY,
  NCREATE_TABLE,
  NCOLUMN_DEF,
  NLITERAL,
  NRANGEVAR,
  NJOIN,
  NCREATE_INDEX,
  NLIST_TABLES_CMD
} ParseNodeType;

typedef enum SortDir { SORT_ASC, SORT_DESC } SortDir;

typedef struct ParseNode {
  ParseNodeType type;
} ParseNode;

typedef enum BinExprOp {
  EQ,
  NEQ,
  GT,
  GE,
  LT,
  LE,
  PLUS,
  MINUS,
  MULT,
  DIV,
  AND,
  OR,
} BinExprOp;

const char* bin_expr_op_to_string(BinExprOp);

typedef struct NBinExpr {
  ParseNodeType type;

  BinExprOp op;
  BType return_type;
  Datum (*op_func)(Datum, Datum);
  ParseNode* lhs;
  ParseNode* rhs;
} NBinExpr;

typedef struct NIdentifier {
  ParseNodeType type;

  char* identifier;
  size_t
      idx;  // To be filled in during analysis phase after parsing. Should be internal index/id
            // to whatever structure (column, table, etc) this identifier refers to.
} NIdentifier;

typedef struct NSelectStmt {
  ParseNodeType type;

  ParseNode** target_list;
  ParseNode* from_clause;
  ParseNode* where_clause;
  ParseNode* sort_clause;
} NSelectStmt;

typedef struct NRangeVar {
  ParseNodeType type;

  char* table_name;
  size_t join_list_index;  // Set during analyze phase.
} NRangeVar;

typedef struct NInsertStmt {
  ParseNodeType type;

  ParseNode* range_var;
  ParseNode** column_list;
  ParseNode*** values_list;
} NInsertStmt;

typedef struct NDeleteStmt {
  ParseNodeType type;

  ParseNode* range_var;
  ParseNode* where_clause;
} NDeleteStmt;

typedef struct NAssignExpr {
  ParseNodeType type;

  ParseNode* column;
  ParseNode* value_expr;
} NAssignExpr;

typedef struct NUpdateStmt {
  ParseNodeType type;

  ParseNode* range_var;
  ParseNode** assign_expr_list;
  ParseNode* where_clause;
} NUpdateStmt;

typedef struct NSortBy {
  ParseNodeType type;

  SortDir dir;
  ParseNode* sort_expr;
} NSortBy;

typedef struct NCreateTable {
  ParseNodeType type;

  ParseNode* table_name;
  ParseNode** column_defs;
} NCreateTable;

typedef struct NColumnDef {
  ParseNodeType type;

  ParseNode* col_name;
  ParseNode* col_type;
  BType col_type_id;
} NColumnDef;

typedef struct NCreateIndex {
  ParseNodeType type;

  ParseNode* table_name;
  ParseNode** column_list;
} NCreateIndex;

typedef struct NLiteral {
  ParseNodeType type;

  BType lit_type;

  union {
    bool bool_lit;
    int32_t int_lit;
    char* str_lit;
  } data;
} NLiteral;

typedef enum JoinMethod {
  JOIN_INNER,
  JOIN_OUTER,
  JOIN_LEFT,
  JOIN_RIGHT,
} JoinMethod;

typedef struct NJoin {
  ParseNodeType type;

  JoinMethod join_method;
  ParseNode* left;
  ParseNode* right;
  ParseNode* qual_cond;
  size_t join_list_index;  // Set during analyze phase.
} NJoin;

typedef struct NListTablesCmd {
  ParseNodeType type;
} NListTablesCmd;

void free_parse_node(ParseNode*);
void print_parse_node(ParseNode*, PrintContext*);

#ifdef __cplusplus
}
#endif

#endif  // CONTEXT_HH
