#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>

#include "sql/context.hpp"

struct SystemCatalogue {};

struct BufferPool {
  std::vector<std::string> items;
};

struct SequentialScanIterator {
  BufferPool& buffer_pool_;
  uint64_t cur_tuple_id_;

  void open() { cur_tuple_id_ = 0; };
  void next(){};
  void close(){};
};

struct QueryPlan {
  bool is_valid_;
};

std::unique_ptr<std::vector<std::string>> execute_plan(btdb::sql::SelectSmt& plan,
                                                       std::vector<std::string>& tuples) {
  plan.table_name = "foo";
  auto results = std::make_unique<std::vector<std::string>>();
  for (const auto& tuple : tuples) {
    results->emplace_back(tuple);
  }
  return results;
}

int main() {
  printf("Starting btdb\n");

  auto tuples = std::make_unique<std::vector<std::string>>();
  tuples->emplace_back("hello");
  tuples->emplace_back("world");
  while (true) {
    printf("btdb> ");
    std::string line;
    if (!std::getline(std::cin, line)) {
      break;
    }
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    if (line == "\\q") {
      break;
    }
    btdb::sql::ParserContext parser(line);
    if (parser.Parse() != 0) {
      continue;
    }
    auto query_plan = parser.result;
    auto results = execute_plan(query_plan, *tuples);
    printf("Results:\n");
    for (const auto& result : *results.get()) {
      std::cout << "\t" << result << std::endl;
    }
  }
  if (std::cin.bad()) {
    fprintf(stderr, "I/O Error\n");
    exit(EXIT_FAILURE);
  }

  printf("Shutting down btdb\n");
  return 0;
}