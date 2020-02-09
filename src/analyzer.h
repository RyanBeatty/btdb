#ifndef ANALYZER_H
#define ANALYZER_H

#include "node.h"
#include "storage.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CmdType {
  CMD_SELECT,
  CMD_INSERT,
  CMD_UPDATE,
  CMD_DELETE,
  CMD_UTILITY,
} CmdType;

typedef struct TargetRef {
  const char* column_name;
  size_t join_list_index;
  ParseNode* col_expr;  // This can be just a normal column identifier reference, or it could be a more complex expression.
} TargetRef;

typedef struct Query {
  CmdType cmd;

  char* table_name;
  TableDef** join_list;
  TargetRef** target_list;
  ParseNode* where_clause;
  NAssignExpr** assign_expr_list;
  Tuple** values;
  NSortBy* sort;
  ParseNode* utility_stmt;
} Query;

Query* MakeQuery(CmdType);
Query* AnalyzeParseTree(ParseNode*);
Query* AnalyzeSelectStmt(NSelectStmt*);
Query* AnalyzeInsertStmt(NInsertStmt*);
Query* AnalyzeDeleteStmt(NDeleteStmt*);
Query* AnalyzeUpdateStmt(NUpdateStmt*);
Query* AnalyzeCreateTableStmt(NCreateTable*);
void AnalyzeFromClause(Query*, ParseNode**);
BType CheckType(ParseNode*, TableDef**);
ParseNode* AnalyzeExprNode(ParseNode*, TableDef**);

#ifdef __cplusplus
}
#endif

#endif  // ANALYZER_H
