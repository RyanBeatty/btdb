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

std::unique_ptr<QueryPlan> parse_sql(const std::string& raw_text) {
  std::vector<std::string> tokens;
  std::istringstream iss(raw_text);
  std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(),
            std::back_inserter(tokens));

  if (tokens.size() != 4) {
    return nullptr;
  }

  if (tokens[0] != "select" || tokens[1] != "*" || tokens[2] != "from" ||
      tokens[3] != "foo;") {
    return nullptr;
  }

  return std::make_unique<QueryPlan>();
}

bool validate_plan(QueryPlan& plan) {
  plan.is_valid_ = true;
  return true;
}

std::unique_ptr<std::vector<std::string>> execute_plan(QueryPlan& plan,
                                                       std::vector<std::string>& tuples) {
  plan.is_valid_ = true;
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
    auto query_plan = parse_sql(line);
    if (query_plan == nullptr) {
      printf("Failed to parse sql\n");
      continue;
    }
    if (!validate_plan(*query_plan)) {
      printf("Invalid Plan\n");
      continue;
    }
    auto results = execute_plan(*query_plan, *tuples);
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