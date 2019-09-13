/* recognize tokens for the calculator and print them out */
%{
  #include "parser.hpp"
  #include "context.hh"

%}

%option noyywrap nounput noinput batch



%%
";" return yy::parser::make_SEMICOLON();
"select" return yy::parser::make_SELECT();
"SELECT" return yy::parser::make_SELECT();
"from" return yy::parser::make_FROM();
"FROM" return yy::parser::make_FROM();
[a-zA-Z]+ return yy::parser::make_STRING_GROUP(yytext);
<<EOF>> return yy::parser::make_EOF ();
%%
