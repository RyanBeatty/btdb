#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Only define once.
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "analyzer.h"
#include "collections.h"
#include "node.h"
#include "plan.h"
#include "sql/driver.h"
#include "storage.h"
#include "types.h"
#include "utils.h"

typedef struct Result {
  CharPtrVec* columns;
  Tuple** tuples;  // stb_arr
} Result;

Result ExecPlan(PlanNode* plan) {
  Result results;
  results.columns = plan->target_list;
  results.tuples = NULL;
  Tuple* tuple = plan->get_next_func(plan);
  while (tuple != NULL) {
    arrpush(results.tuples, tuple);
    tuple = plan->get_next_func(plan);
  }
  return results;
}

int main() {
  printf("Starting btdb\n");

  ColDesc* tuple_desc = NULL;
  ColDesc col1 = {.column_name = "bar", .type = T_STRING};
  ColDesc col2 = {.column_name = "baz", .type = T_BOOL};
  arrpush(tuple_desc, col1);
  arrpush(tuple_desc, col2);
  TableDef table_def = {.name = "foo", .tuple_desc = tuple_desc};
  CreateTable(&table_def);

  Tuple* t1 = NULL;
  t1 = SetCol(t1, "bar", MakeDatum(T_STRING, strdup("hello")));
  bool* bool_lit = (bool*)calloc(sizeof(bool), 1);
  *bool_lit = true;
  t1 = SetCol(t1, "baz", MakeDatum(T_BOOL, bool_lit));

  Tuple* t2 = NULL;
  t2 = SetCol(t2, "bar", MakeDatum(T_STRING, strdup("world")));
  bool_lit = (bool*)calloc(sizeof(bool), 1);
  *bool_lit = false;
  t2 = SetCol(t2, "baz", MakeDatum(T_BOOL, bool_lit));

  InsertTuple(0, t1);
  InsertTuple(0, t2);

  ColDesc* table2_tuple_desc = NULL;
  ColDesc table2_col1 = {.column_name = "a", .type = T_STRING};
  arrpush(table2_tuple_desc, table2_col1);
  TableDef table_def2 = {.name = "b", .tuple_desc = table2_tuple_desc};
  CreateTable(&table_def2);

  Tuple* table2_t1 = NULL;
  table2_t1 = SetCol(table2_t1, "a", MakeDatum(T_STRING, strdup("asdf")));
  InsertTuple(1, table2_t1);

  while (true) {
    printf("btdb> ");
    char* line = NULL;
    size_t line_size = 0;
    if (getline(&line, &line_size, stdin) == -1) {
      break;
    }
    if (strcmp(line, "\\q\n") == 0) {
      break;
    }
    Parser* parser = InitParser(strdup(line));
    if (Parse(parser) != 0) {
      continue;
    }
    Query* query = AnalyzeParseTree(parser->tree);
    if (query == NULL) {
      printf("Query not valid\n");
      continue;
    }
    PlanNode* plan = PlanQuery(query);
    Result results = ExecPlan(plan);
    if (results.columns != NULL) {
      CharPtrVecIt it = NULL;
      VEC_FOREACH(it, results.columns) { printf("    %s", *it); }
      printf("\n");
      printf("===============\n");
      for (size_t i = 0; i < arrlen(results.tuples); ++i) {
        Tuple* mtuple = results.tuples[i];
        assert(mtuple != NULL);
        CharPtrVecIt it = NULL;
        VEC_FOREACH(it, results.columns) {
          Datum* data = GetCol(mtuple, *it);
          if (data != NULL) {
            // TODO(ryan): This is some hacky bs to be able to print this as a string.
            // I'm going to need to do an overhaul of alot of this code in the future.
            if (data->type == T_STRING) {
              printf("%s", (char*)data->data);
            } else if (data->type == T_BOOL) {
              printf("%s", (*((bool*)data->data) ? "true" : "false"));
            } else {
              Panic("Only support printing strings or bools");
            }
          }
          printf("\t");
        }
        printf("\n");
      }
    } else {
      printf("===============\n");
    }
  }
  // TODO(ryan): Print out IO error condition if any.

  printf("Shutting down btdb\n");
  return 0;
}
