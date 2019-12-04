#ifndef DRIVER_H
#define DRIVER_H
#include <string.h>

#include "node.h"

typedef struct yy_buffer_state* YY_BUFFER_STATE;

typedef struct Parser {
  YY_BUFFER_STATE buffer_state;
  ParseNode* tree;
} Parser;

Parser* InitParser(char*);
void FreeParser(Parser*);
int Parse(Parser*);

// // Give Flex the prototype of yylex we want ...
// #define YY_DECL yy::parser::symbol_type yylex(ParserContext& context)
// // ... and declare it for the parser's sake.
// YY_DECL;

// // Forward declare buffer state from scanner so parser can use this.
// // TODO: Is this the right thing to do?
// typedef struct yy_buffer_state* YY_BUFFER_STATE;

// struct ParserContext {
//   ParseNode* tree;
//   YY_BUFFER_STATE buffer_state;

//   ParserContext(std::string sql) { BeginScan(sql); }

//   ~ParserContext() { EndScan(); }

//   void BeginScan(std::string sql);
//   void EndScan();

//   int Parse() {
//     yy::parser parse(*this);
//     return parse();
//   }
// };

#endif  // DRIVER_H
