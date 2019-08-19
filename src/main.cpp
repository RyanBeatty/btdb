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

struct QueryPlan {};

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

int main() {
  printf("Starting btdb\n");

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
  }
  if (std::cin.bad()) {
    fprintf(stderr, "I/O Error\n");
    exit(EXIT_FAILURE);
  }

  printf("Shutting down btdb\n");
  return 0;
}