#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Only define once.
#define STB_DS_IMPLEMENTATION
#include "analyzer.h"
#include "node.h"
#include "plan.h"
#include "sql/driver.h"
#include "stb_ds.h"
#include "storage.h"
#include "types.h"
#include "utils.h"

typedef struct Result {
  const char** columns;
  Tuple2** tuples;  // stb_arr
} Result;

Result ExecPlan(PlanNode* plan) {
  Result results;
  results.columns = NULL;
  for (size_t i = 0; i < arrlen(plan->target_list); ++i) {
    arrpush(results.columns, plan->target_list[i]->column_name);
  }
  results.tuples = NULL;
  Tuple2* tuple = plan->get_next_func(plan);
  while (tuple != NULL) {
    arrpush(results.tuples, tuple);
    tuple = plan->get_next_func(plan);
  }
  return results;
}

void PrintResults(Result results) {
  if (results.columns != NULL) {
    for (size_t i = 0; i < arrlen(results.columns); ++i) {
      printf("    %s", results.columns[i]);
    }
    printf("\n");
    printf("===============\n");
    for (size_t i = 0; i < arrlen(results.tuples); ++i) {
      Tuple2* mtuple = results.tuples[i];
      assert(mtuple != NULL);
      for (size_t i = 0; i < arrlen(results.columns); ++i) {
        Datum* data = GetCol(mtuple, results.columns[i]);
        if (data != NULL) {
          // TODO(ryan): This is some hacky bs to be able to print this as a string.
          // I'm going to need to do an overhaul of alot of this code in the future.
          if (data->type == T_STRING) {
            printf("%s", (char*)data->data);
          } else if (data->type == T_BOOL) {
            printf("%s", (*((bool*)data->data) ? "true" : "false"));
          } else if (data->type == T_INT) {
            printf("%" PRId32, *((int32_t*)data->data));
          } else if (data->type == T_NULL) {
            printf("\t");
          } else {
            Panic("Only support printing strings, ints, or bools");
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

int main() {
  printf("Starting btdb\n");

  InitSystemTables();

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
    // TODO(ryan): This is kinda hacky...
    if (query->cmd == CMD_UTILITY) {
      ExecuteUtilityStmt(query);
      printf("UTILITY DONE\n");
    } else {
      PlanNode* plan = PlanQuery(query);
      Result results = ExecPlan(plan);
      PrintResults(results);
    }
  }
  // TODO(ryan): Print out IO error condition if any.

  printf("Shutting down btdb\n");
  return 0;
}
