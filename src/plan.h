#ifndef PLAN_H
#define PLAN_H

#include "analyzer.h"
#include "node.h"
#include "storage.h"

typedef enum PLanNodeType {
  N_PLAN_RESULT,
  N_PLAN_SEQ_SCAN,
  N_PLAN_MODIFY_SCAN,
  N_PLAN_SORT,
  N_PLAN_NESTED_LOOP,
  N_PLAN_INDEX_SCAN
} PlanNodeType;

typedef struct PlanNode {
  PlanNodeType type;
  TargetRef** target_list;  // std_arr.
  Tuple** results;          // 2d stb_arr.
  TableDef* table_def;
  struct PlanNode* left;
  struct PlanNode* right;
  void (*init_func)(struct PlanNode*);
  Tuple* (*get_next_func)(struct PlanNode*);
} PlanNode;

typedef struct SeqScan {
  PlanNode plan;
  const char* table_name;
  Cursor cursor;
  ParseNode* where_clause;
} SeqScan;

typedef struct IndexScan {
  PlanNode plan;
  IndexCursor cursor;
  ParseNode* where_clause;
  IndexDef* index_def;
} IndexScan;

typedef struct ModifyScan {
  PlanNode plan;
  CmdType cmd;
  const char* table_name;
  Cursor cursor;
  ParseNode* where_clause;
  ParseNode*** insert_tuples;  // 2d stb_arr.
  NAssignExpr** assign_exprs;
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

typedef struct NestedLoop {
  PlanNode plan;
  Tuple* cur_left_tuple;
  bool need_new_left_tuple;
  JoinMethod join_method;
  ParseNode* qual_condition;
} NestedLoop;

typedef struct ResultScan {
  PlanNode plan;
  ParseNode* where_clause;
} ResultScan;

PlanNode* PlanQuery(Query*);

void ExecuteUtilityStmt(Query*);

#endif  // PLAN_H