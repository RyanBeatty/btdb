#include "driver.h"

#include "parser.h"

int Parse(Parser* parser) { return yyparse(parser); }
