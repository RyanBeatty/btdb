#ifndef ANALYZER_H
#define ANALYZER_H
#include <vector>

#include "collections.h"
#include "sql/context.hpp"
#include "storage.h"
#include "types.h"

namespace btdb {

enum CmdType {
  CMD_SELECT,
  CMD_INSERT,
  CMD_UPDATE,
  CMD_DELETE,
  CMD_UTILITY,
};

struct Query {
  CmdType cmd;

  char* table_name;
  CharPtrVec* target_list;
  ParseNode* where_clause;
  List* assign_expr_list;
  std::vector<Tuple> values;
};

Query* MakeQuery(CmdType);
Query* AnalyzeParseTree(ParseNode*);
Query* AnalyzeSelectStmt(NSelectStmt*);
Query* AnalyzeInsertStmt(NInsertStmt*);
Query* AnalyzeDeleteStmt(NDeleteStmt*);
Query* AnalyzeUpdateStmt(NUpdateStmt*);
BType CheckType(ParseNode*, TableDef&);

}  // namespace btdb
#endif  // ANALYZER_H
