#include <cassert>
#include <iostream>
#include <string>

#include "utils.h"

void Panic(const std::string& msg) {
  std::cerr << "Panic: " << msg << std::endl;
  exit(EXIT_FAILURE);
}
