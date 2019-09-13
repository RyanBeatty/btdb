/* simplest version of calculator */
%skeleton "lalr1.cc"
%require "3.4"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  struct ParserContext;
}

%param {
  ParserContext& ctx
}

%code{
  #include <stdio.h>
  #include <iostream>
  #include "context.hh"
}

//%locations

//%define parse.trace
//%define parse.error verbose

%define api.token.prefix {TOK_}
%token
    EOF 0
    SELECT
    FROM
    SEMICOLON ";"
;
%token <std::string> STRING_GROUP;

%%
%start selectstmt;
selectstmt: SELECT FROM STRING_GROUP ";"
{ 
  SelectSmt sel;
  sel.table_name = $3;
  ctx.result = sel;
};

%%

void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}

int main()
{
  ParserContext ctx;
  for (std::string line; std::getline(std::cin, line);) {
    ctx.BeginScan(line);
    ctx.Parse();
    ctx.EndScan();
    std::cout << ctx.result.table_name << std::endl;
  }
}
