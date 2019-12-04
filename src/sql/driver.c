#include "sql/driver.h"
#include "sql/parser2.h"

int Parse(Parser* parser) { return yyparse(parser); }
