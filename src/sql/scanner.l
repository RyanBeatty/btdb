/* recognize tokens for the calculator and print them out */
%{
  #include "parser.hpp"
  #include "context.hh"

%}

%option noyywrap nounput noinput batch debug



%%
"hello" return yy::parser::make_HELLO();
<<EOF>> return yy::parser::make_EOF ();
%%
