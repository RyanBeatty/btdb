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

void print_parse_node(ParseNode* node, PrintContext& ctx) {
  if (node == nullptr) {
    return;
  }

  switch (node->type) {
    case NIDENTIFIER: {
      NIdentifier* identifier = (NIdentifier*)(node);
      ctx.PrintObject("NIdentifier");
      ctx.PrintChild("identifier", identifier->identifier);
      ctx.EndObject();
      break;
    }
    case NBIN_EXPR: {
      NBinExpr* bin_expr = (NBinExpr*)(node);
      ctx.PrintObject("NBinExpr");
      if (bin_expr->lhs != nullptr) {
        ctx.PrintObject("lhs");
        print_parse_node(bin_expr->lhs, ctx);
        ctx.EndObject();
      }
      if (bin_expr->rhs != nullptr) {
        ctx.PrintObject("rhs");
        print_parse_node(bin_expr->rhs, ctx);
        ctx.EndObject();
      }
      ctx.EndObject();
      break;
    }
    case NSTRING_LIT: {
      NStringLit* str_lit = (NStringLit*)(node);
      ctx.PrintObject("NStringLit");
      ctx.PrintChild("str_lit", str_lit->str_lit);
      ctx.EndObject();
      break;
    }
    default: {
      Panic("Unkown Parse Node Type");
      break;
    }
  }
  return;
}

}  // namespace sql
}  // namespace btdb