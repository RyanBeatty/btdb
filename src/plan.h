#ifndef PLAN_H
#define PLAN_H

#include "analyzer.h"
#include "collections.h"
#include "node.h"
#include "storage.h"

typedef enum PLanNodeType {
  N_PLAN_RESULT,
  N_PLAN_SEQ_SCAN,
  N_PLAN_MODIFY_SCAN,
  N_PLAN_SORT,
} PlanNodeType;

typedef struct PlanNode {
  PlanNodeType type;
  CharPtrVec* target_list;
  Tuple** results;  // 2d stb_arr.
  TableDef* table_def;
  struct PlanNode* left;
  struct PlanNode* right;
  Tuple* (*get_next_func)(struct PlanNode*);
} PlanNode;

typedef struct SeqScan {
  PlanNode plan;
  const char* table_name;
  size_t next_index;
  ParseNode* where_clause;
} SeqScan;

typedef struct ModifyScan {
  PlanNode plan;
  CmdType cmd;
  const char* table_name;
  size_t next_index;
  ParseNode* where_clause;
  Tuple** insert_tuples;  // 2d stb_arr.
  List* assign_exprs;
} ModifyScan;

typedef enum SortMethod { INSERTION_SORT } SortMethod;

typedef struct Sort {
  PlanNode plan;
  SortMethod method;
  // TODO(ryan): Allow for more general sorting expressions.
  NIdentifier* sort_col;
  Datum (*cmp_func)(Datum, Datum);
  size_t next_index;
  bool is_sorted;
} Sort;

PlanNode* PlanQuery(Query*);

#endif PLAN_H