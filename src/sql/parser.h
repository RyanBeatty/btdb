/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_HOME_RBEATTY_PROJECTS_BTDB_SRC_SQL_PARSER_H_INCLUDED
# define YY_YY_HOME_RBEATTY_PROJECTS_BTDB_SRC_SQL_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 3 "parser.y"

  #include "node.h"

  // Can't include sql stuff or else we get circular import,
  // so need to forward declare stuff.
  struct Parser;
//   struct ParserContext;

#line 57 "/home/rbeatty/Projects/BTDB/src/sql/parser.h"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOK_EOF = 0,
    TOK_SELECT = 258,
    TOK_INSERT = 259,
    TOK_DELETE = 260,
    TOK_UPDATE = 261,
    TOK_INTO = 262,
    TOK_VALUES = 263,
    TOK_SET = 264,
    TOK_LPARENS = 265,
    TOK_RPARENS = 266,
    TOK_FROM = 267,
    TOK_SEMICOLON = 268,
    TOK_COMMA = 269,
    TOK_WHERE = 270,
    TOK_AND = 271,
    TOK_OR = 272,
    TOK_EQ = 273,
    TOK_NEQ = 274,
    TOK_GT = 275,
    TOK_GE = 276,
    TOK_LT = 277,
    TOK_LE = 278,
    TOK_PLUS = 279,
    TOK_MINUS = 280,
    TOK_MULT = 281,
    TOK_DIV = 282,
    TOK_STRING_GROUP = 283,
    TOK_STRING_LITERAL = 284,
    TOK_BOOLEAN_LITERAL = 285
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 35 "parser.y"

  char* str_lit;
  bool bool_lit;
  ParseNode* node;
  List* list_node;

#line 107 "/home/rbeatty/Projects/BTDB/src/sql/parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (struct Parser* parser);

#endif /* !YY_YY_HOME_RBEATTY_PROJECTS_BTDB_SRC_SQL_PARSER_H_INCLUDED  */
