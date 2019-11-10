#ifndef ANALYZER_H
#define ANALYZER_H
#include <vector>

#include "collections.h"
#include "sql/context.hpp"
#include "types.h"

namespace btdb {

static std::vector<TableDef> Tables;

// struct SystemCatalog {
//   std::vector<TableDef> tables;

//   bool ValidateParseTree(ParseTree& tree);
//   bool ValidateSelectStmt(NSelectStmt* select);
//   bool ValidateInsertStmt(NInsertStmt* insert);
//   bool ValidateDeleteStmt(NDeleteStmt* delete_stmt);
//   bool ValidateUpdateStmt(NUpdateStmt* update);
//   BType CheckType(ParseNode* node, TableDef& table_def);
// };

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
  std::vector<std::vector<std::string>> assign_exprs;
  std::vector<Tuple> values;
};

Query* MakeQuery(CmdType);
Query* AnalyzeParseTree(ParseNode*);
Query* AnalyzeSelectStmt(NSelectStmt*);
Query* AnalyzeInsertStmt(NInsertStmt*);
Query* AnalyzeDeleteStmt(NDeleteStmt*);
Query* AnalyzeUpdateStmt(NUpdateStmt*);
BType CheckType(ParseNode*, TableDef&);


// Query* AnalyzeAndRewriteParseTree(ParseTree&);
// Query* AnalyzeAndRewriteSelectStmt(NSelectStmt*);
// Query* AnalyzeAndRewriteInsertStmt(NInsertStmt*);
// Query* AnalyzeAndRewriteDeleteStmt(NDeleteStmt*);
// Query* AnalyzeAndRewriteUpdateStmt(NUpdateStmt*);

}  // namespace btdb
#endif  // ANALYZER_H
