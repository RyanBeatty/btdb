#include "node.hpp"

#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace btdb {
namespace sql {

void Panic(const std::string& msg) {
  std::cerr << "Panic: " << msg << std::endl;
  exit(EXIT_FAILURE);
};

void free_parse_node(ParseNode* node) {
  if (node == nullptr) {
    return;
  }

  switch (node->type) {
    case NIDENTIFIER: {
      NIdentifier* identifier = (NIdentifier*)(node);
      assert(identifier->identifier != nullptr);
      free(identifier->identifier);
      free(identifier);
      break;
    }
    case NBIN_EXPR: {
      NBinExpr* bin_expr = (NBinExpr*)(node);
      if (bin_expr->lhs != nullptr) {
        free_parse_node(bin_expr->lhs);
      }
      if (bin_expr->rhs != nullptr) {
        free_parse_node(bin_expr->rhs);
      }
      free(bin_expr);
      break;
    }
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)(node);
      assert(str_lit->str_lit != nullptr);
      free(str_lit->str_lit);
      free(str_lit);
      break;
    }
    default: {
      Panic("Unkown Parse Node Type");
      break;
    }
  }
}

std::ostringstream& print_parse_node(ParseNode* node, std::ostringstream& oss) {
  if (node == nullptr) {
    return oss;
  }

  switch (node->type) {
    case NIDENTIFIER: {
      NIdentifier* identifier = (NIdentifier*)(node);
      oss << "NIdentifier{ "
          << "identifier=" << identifier->identifier << " }" << std::endl;
      break;
    }
    case NBIN_EXPR: {
      NBinExpr* bin_expr = (NBinExpr*)(node);
      oss << "NBinExpr{";
      if (bin_expr->lhs != nullptr) {
        oss << "lhs=";
        print_parse_node(bin_expr->lhs, oss);
      }
      if (bin_expr->rhs != nullptr) {
      }
      break;
    }
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)(node);
      oss << "NStringLit{ "
          << "str_lit=" << str_lit->str_lit << " }" << std::endl;
      break;
    }
    default: {
      Panic("Unkown Parse Node Type");
      break;
    }
  }
  return oss;
}

}  // namespace sql
}  // namespace btdb