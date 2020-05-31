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
    TOK_SCANNER_ERROR = 258,
    TOK_SELECT = 259,
    TOK_INSERT = 260,
    TOK_DELETE = 261,
    TOK_UPDATE = 262,
    TOK_INTO = 263,
    TOK_VALUES = 264,
    TOK_ORDER = 265,
    TOK_BY = 266,
    TOK_ASC = 267,
    TOK_DESC = 268,
    TOK_SET = 269,
    TOK_CREATE = 270,
    TOK_TABLE = 271,
    TOK_LPARENS = 272,
    TOK_RPARENS = 273,
    TOK_FROM = 274,
    TOK_SEMICOLON = 275,
    TOK_COMMA = 276,
    TOK_WHERE = 277,
    TOK_AND = 278,
    TOK_OR = 279,
    TOK_EQ = 280,
    TOK_NEQ = 281,
    TOK_GT = 282,
    TOK_GE = 283,
    TOK_LT = 284,
    TOK_LE = 285,
    TOK_PLUS = 286,
    TOK_MINUS = 287,
    TOK_MULT = 288,
    TOK_DIV = 289,
    TOK_NULL = 290,
    TOK_JOIN = 291,
    TOK_LEFT = 292,
    TOK_RIGHT = 293,
    TOK_OUTER = 294,
    TOK_ON = 295,
    TOK_STRING_GROUP = 296,
    TOK_STRING_LITERAL = 297,
    TOK_BOOLEAN_LITERAL = 298,
    TOK_INT_LITERAL = 299
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 37 "parser.y"

  char* str_lit;
  bool bool_lit;
  int32_t int_lit;
  ParseNode* node;
  ParseNode** list_node;
  ParseNode*** list_list_node;
  SortDir sort_dir;
  JoinMethod join_method;

#line 125 "/home/rbeatty/Projects/BTDB/src/sql/parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (struct Parser* parser);

#endif /* !YY_YY_HOME_RBEATTY_PROJECTS_BTDB_SRC_SQL_PARSER_H_INCLUDED  */
