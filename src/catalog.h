#ifndef CATALOG_H
#define CATALOG_H
#include <vector>

#include "sql/context.hpp"
#include "types.h"

namespace btdb {
struct SystemCatalog {
  std::vector<TableDef> tables;

  bool ValidateParseTree(ParseTree& tree);
  bool ValidateSelectStmt(NSelectStmt* select);
  bool ValidateInsertStmt(NInsertStmt* insert);
  bool ValidateDeleteStmt(NDeleteStmt* delete_stmt);
  bool ValidateUpdateStmt(NUpdateStmt* update);
  BType CheckType(ParseNode* node, TableDef& table_def);
};

}  // namespace btdb
#endif  // CATALOG_H
