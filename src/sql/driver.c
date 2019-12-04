#include "sql/driver.h"
#include "sql/parser.h"

int Parse(Parser* parser) { return yyparse(parser); }
