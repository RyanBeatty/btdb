#ifndef ANALYZER_H
#define ANALYZER_H

#include "collections.h"
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

typedef struct Query {
  CmdType cmd;

  char* table_name;
  TableDef* table_def;
  char** target_list;
  ParseNode* where_clause;
  NAssignExpr** assign_expr_list;
  Tuple** values;
  NSortBy* sort;
} Query;

Query* MakeQuery(CmdType);
Query* AnalyzeParseTree(ParseNode*);
Query* AnalyzeSelectStmt(NSelectStmt*);
Query* AnalyzeInsertStmt(NInsertStmt*);
Query* AnalyzeDeleteStmt(NDeleteStmt*);
Query* AnalyzeUpdateStmt(NUpdateStmt*);
BType CheckType(ParseNode*, TableDef*);

#ifdef __cplusplus
}
#endif

#endif  // ANALYZER_H
