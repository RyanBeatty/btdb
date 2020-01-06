/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1





# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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

#line 111 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"

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
    TOK_ORDER = 264,
    TOK_BY = 265,
    TOK_ASC = 266,
    TOK_DESC = 267,
    TOK_SET = 268,
    TOK_CREATE = 269,
    TOK_TABLE = 270,
    TOK_LPARENS = 271,
    TOK_RPARENS = 272,
    TOK_FROM = 273,
    TOK_SEMICOLON = 274,
    TOK_COMMA = 275,
    TOK_WHERE = 276,
    TOK_AND = 277,
    TOK_OR = 278,
    TOK_EQ = 279,
    TOK_NEQ = 280,
    TOK_GT = 281,
    TOK_GE = 282,
    TOK_LT = 283,
    TOK_LE = 284,
    TOK_PLUS = 285,
    TOK_MINUS = 286,
    TOK_MULT = 287,
    TOK_DIV = 288,
    TOK_STRING_GROUP = 289,
    TOK_STRING_LITERAL = 290,
    TOK_BOOLEAN_LITERAL = 291
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 37 "parser.y"

  char* str_lit;
  bool bool_lit;
  ParseNode* node;
  ParseNode** list_node;
  ParseNode*** list_list_node;
  SortDir sort_dir;

#line 169 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (struct Parser* parser);

#endif /* !YY_YY_HOME_RBEATTY_PROJECTS_BTDB_SRC_SQL_PARSER_H_INCLUDED  */


/* Unqualified %code blocks.  */
#line 16 "parser.y"

  #include <assert.h>
  #include <stdlib.h>
  #include <stdbool.h>
  #include <string.h>
  #include <stdio.h>

  #include "stb_ds.h"
  
  #include "sql/driver.h"

  extern int yylex(void);
  // Because we use %parse-param, the signature of yyerror changes.
  void yyerror(Parser*, const char*);

#line 202 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  18
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   117

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  37
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  52
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  110

#define YYUNDEFTOK  2
#define YYMAXUTOK   291

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    94,    94,    97,   100,   103,   106,   110,   121,   131,
     143,   144,   149,   159,   171,   172,   177,   178,   187,   188,
     189,   196,   203,   215,   222,   231,   240,   249,   258,   267,
     276,   285,   294,   303,   312,   321,   331,   348,   351,   361,
     372,   375,   380,   387,   392,   398,   411,   426,   431,   438,
     452,   466,   486
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "EOF", "error", "$undefined", "SELECT", "INSERT", "DELETE", "UPDATE",
  "INTO", "VALUES", "ORDER", "BY", "ASC", "DESC", "SET", "CREATE", "TABLE",
  "\"(\"", "\")\"", "FROM", "\";\"", "\",\"", "WHERE", "AND", "OR",
  "\"=\"", "\"!=\"", "\">\"", "\">=\"", "\"<\"", "\"<=\"", "\"+\"",
  "\"-\"", "\"*\"", "\"/\"", "STRING_GROUP", "STRING_LITERAL",
  "BOOLEAN_LITERAL", "$accept", "stmt", "select_stmt", "target_list",
  "from_clause", "from_list", "where_clause", "sort_clause",
  "sort_direction", "expr", "insert_stmt", "insert_column_list",
  "column_list", "insert_values_clause", "insert_values_list",
  "insert_value_items", "delete_stmt", "update_stmt",
  "update_assign_expr_list", "assign_expr", "create_table_stmt",
  "table_expr", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291
};
# endif

#define YYPACT_NINF -33

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-33)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       0,   -32,     4,    30,    15,    32,    50,   -33,   -33,   -33,
     -33,   -33,   -33,     6,    53,    54,    73,    55,   -33,    56,
      57,    71,    77,    71,    60,    79,   -33,    76,   -33,     8,
      88,    64,    91,    81,    78,    -3,   -33,    67,    69,   -33,
     -33,   -33,    41,    94,    86,   -33,    -5,    90,    89,   -33,
       8,    60,    92,    75,    -1,   -33,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     8,   -33,
     -33,    80,     8,    87,   -33,    41,   -33,   -33,   -33,    93,
      82,    52,    52,   -23,   -23,   -23,   -23,   -23,   -23,    13,
      13,   -33,   -33,    29,   -33,    41,     3,    97,   -33,    83,
     -33,   -33,   -33,   -33,     8,     8,   -33,    41,     5,   -33
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     2,     3,     4,
       5,     6,     8,    10,     0,     0,     0,     0,     1,     0,
       0,    14,     0,    14,     0,     0,    12,    11,     9,     0,
      16,     0,     0,     0,     0,    14,    47,     0,     0,    21,
      22,    23,    15,     0,     0,    38,     0,     0,     0,    45,
       0,     0,     0,     0,     0,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     7,
      37,     0,     0,    40,    36,    49,    48,    46,    51,     0,
       0,    34,    35,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    18,    39,    43,     0,     0,    50,     0,
      19,    20,    17,    41,     0,     0,    52,    44,     0,    42
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -33,   -33,   -33,   -33,   -33,   -33,   -22,   -33,   -33,   -29,
     -33,   -33,   -33,   -33,   -33,    10,   -33,   -33,   -33,    59,
     -33,   -33
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     6,     7,    13,    21,    27,    30,    44,   102,    95,
       8,    32,    46,    48,    73,    96,     9,    10,    35,    36,
      11,    54
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      42,    33,    12,     1,     2,     3,     4,    64,    65,    66,
      67,    14,    70,    52,     5,    71,    79,    51,    29,    80,
     103,    75,   109,   104,    19,   104,    20,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
     100,   101,    39,    40,    41,    66,    67,    17,    15,    16,
      18,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,   107,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    24,    22,    23,    25,
      26,    28,    29,    31,    34,    37,    38,    43,    45,    47,
      49,    53,    50,    55,    68,    69,    72,    97,    74,    78,
      76,    77,    98,   105,    94,   108,    99,   106
};

static const yytype_uint8 yycheck[] =
{
      29,    23,    34,     3,     4,     5,     6,    30,    31,    32,
      33,     7,    17,    35,    14,    20,    17,    20,    21,    20,
      17,    50,    17,    20,    18,    20,    20,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      11,    12,    34,    35,    36,    32,    33,    15,    18,    34,
       0,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,   104,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    13,    34,    34,    34,
      34,    34,    21,    16,    34,    16,    20,     9,    34,     8,
      19,    34,    24,    34,    10,    19,    16,    20,    19,    34,
      51,    19,    19,    16,    34,   105,    34,    34
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,    14,    38,    39,    47,    53,
      54,    57,    34,    40,     7,    18,    34,    15,     0,    18,
      20,    41,    34,    34,    13,    34,    34,    42,    34,    21,
      43,    16,    48,    43,    34,    55,    56,    16,    20,    34,
      35,    36,    46,     9,    44,    34,    49,     8,    50,    19,
      24,    20,    43,    34,    58,    34,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    10,    19,
      17,    20,    16,    51,    19,    46,    56,    19,    34,    17,
      20,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    46,    46,    46,    34,    46,    52,    20,    19,    34,
      11,    12,    45,    17,    20,    16,    34,    46,    52,    17
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    37,    38,    38,    38,    38,    38,    39,    40,    40,
      41,    41,    42,    42,    43,    43,    44,    44,    45,    45,
      45,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    47,    48,    49,    49,
      50,    51,    51,    52,    52,    53,    54,    55,    55,    56,
      57,    58,    58
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     6,     1,     3,
       0,     2,     1,     3,     0,     2,     0,     4,     0,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     6,     3,     1,     3,
       2,     3,     5,     1,     3,     5,     6,     1,     3,     3,
       7,     2,     4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (parser, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, parser); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, struct Parser* parser)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (parser);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, struct Parser* parser)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, parser);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, struct Parser* parser)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              , parser);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, parser); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, struct Parser* parser)
{
  YYUSE (yyvaluep);
  YYUSE (parser);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (struct Parser* parser)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 94 "parser.y"
    {
    parser->tree = (yyvsp[0].node);
  }
#line 1360 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 3:
#line 97 "parser.y"
    {
    parser->tree = (yyvsp[0].node);
  }
#line 1368 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 4:
#line 100 "parser.y"
    {
    parser->tree = (yyvsp[0].node);
  }
#line 1376 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 5:
#line 103 "parser.y"
    {
    parser->tree = (yyvsp[0].node);
  }
#line 1384 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 6:
#line 106 "parser.y"
    {
    parser->tree = (yyvsp[0].node);
  }
#line 1392 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 7:
#line 110 "parser.y"
    {
  NSelectStmt* select = (NSelectStmt*)calloc(1, sizeof(NSelectStmt));
  select->type = NSELECT_STMT;
  select->target_list = (yyvsp[-4].list_node);
  select->from_clause = (yyvsp[-3].list_node);
  select->where_clause = (yyvsp[-2].node);
  select->sort_clause = (yyvsp[-1].node);
  (yyval.node) = (ParseNode*) select;
}
#line 1406 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 8:
#line 121 "parser.y"
    { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = (yyvsp[0].str_lit);

      ParseNode** target_list = NULL;
      arrpush(target_list, (ParseNode*)identifier);
      (yyval.list_node) = target_list;
    }
#line 1421 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 9:
#line 131 "parser.y"
    { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = (yyvsp[0].str_lit);

      ParseNode** target_list = (yyvsp[-2].list_node);
      arrpush(target_list, (ParseNode*)identifier);
      (yyval.list_node) = target_list;
    }
#line 1436 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 10:
#line 143 "parser.y"
    { (yyval.list_node) = NULL; }
#line 1442 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 11:
#line 144 "parser.y"
    {
      (yyval.list_node) = (yyvsp[0].list_node);
    }
#line 1450 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 12:
#line 149 "parser.y"
    { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = (yyvsp[0].str_lit);

      ParseNode** from_list = NULL;
      arrpush(from_list, (ParseNode*)identifier);
      (yyval.list_node) = from_list;
    }
#line 1465 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 13:
#line 159 "parser.y"
    { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = (yyvsp[0].str_lit);

      ParseNode** from_list = (yyvsp[-2].list_node);
      arrpush(from_list, (ParseNode*)identifier);
      (yyval.list_node) = from_list;
    }
#line 1480 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 14:
#line 171 "parser.y"
    { (yyval.node) = NULL; }
#line 1486 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 15:
#line 172 "parser.y"
    {
      (yyval.node) = (yyvsp[0].node);
    }
#line 1494 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 16:
#line 177 "parser.y"
    { (yyval.node) = NULL; }
#line 1500 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 17:
#line 178 "parser.y"
    {
    NSortBy* sort_by = calloc(1, sizeof(NSortBy));
    sort_by->type = NSORTBY;
    sort_by->dir = (yyvsp[0].sort_dir);
    sort_by->sort_expr = (yyvsp[-1].node);
    (yyval.node) = (ParseNode*)sort_by; 
  }
#line 1512 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 18:
#line 187 "parser.y"
    { (yyval.sort_dir) = SORT_ASC; }
#line 1518 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 19:
#line 188 "parser.y"
    { (yyval.sort_dir) = SORT_ASC; }
#line 1524 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 20:
#line 189 "parser.y"
    { (yyval.sort_dir) = SORT_DESC; }
#line 1530 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 21:
#line 196 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = (yyvsp[0].str_lit);
      (yyval.node) = (ParseNode*)identifier;
    }
#line 1542 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 22:
#line 203 "parser.y"
    {
      // TODO(ryan): Need to remove leading and trailing ' characters. figure out better way.
      size_t len = strlen((yyvsp[0].str_lit));
      assert(len >= 2);
      NStringLit* str_lit = (NStringLit*)calloc(1, sizeof(NStringLit));
      assert(str_lit != NULL);
      str_lit->type = NSTRING_LIT;
      str_lit->str_lit = (char*)calloc(len - 2 + 1, sizeof(char));
      assert(str_lit->str_lit != NULL);
      strncpy(str_lit->str_lit, (yyvsp[0].str_lit) + 1, len - 2);
      (yyval.node) = (ParseNode*)str_lit;
    }
#line 1559 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 23:
#line 215 "parser.y"
    {
    NBoolLit* bool_lit = (NBoolLit*)calloc(1, sizeof(NBoolLit));
    assert(bool_lit != NULL);
    bool_lit->type = NBOOL_LIT;
    bool_lit->bool_lit = (yyvsp[0].bool_lit);
    (yyval.node) = (ParseNode*)bool_lit;
  }
#line 1571 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 24:
#line 222 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = EQ;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1585 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 25:
#line 231 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = NEQ;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1599 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 26:
#line 240 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = GT;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1613 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 27:
#line 249 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = GE;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1627 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 28:
#line 258 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = LT;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1641 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 29:
#line 267 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = LE;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1655 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 30:
#line 276 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = PLUS;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1669 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 31:
#line 285 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = MINUS;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1683 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 32:
#line 294 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = MULT;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1697 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 33:
#line 303 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = DIV;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1711 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 34:
#line 312 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = AND;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1725 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 35:
#line 321 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = NBIN_EXPR;
      bin_expr->op = OR;
      bin_expr->lhs = (yyvsp[-2].node);
      bin_expr->rhs = (yyvsp[0].node);
      (yyval.node) = (ParseNode*)bin_expr;
    }
#line 1739 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 36:
#line 331 "parser.y"
    {
  NInsertStmt* insert = (NInsertStmt*) calloc(1, sizeof(NInsertStmt));
  assert(insert != NULL);
  insert->type = NINSERT_STMT;

  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = NIDENTIFIER;
  identifier->identifier = (yyvsp[-3].str_lit);

  insert->table_name = (ParseNode*) identifier;
  insert->column_list = (yyvsp[-2].list_node);
  insert->values_list = (yyvsp[-1].list_list_node);

  (yyval.node) = (ParseNode*) insert;
}
#line 1760 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 37:
#line 348 "parser.y"
    { (yyval.list_node) = (yyvsp[-1].list_node); }
#line 1766 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 38:
#line 351 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = (yyvsp[0].str_lit);

      ParseNode** column_list = NULL;
      arrpush(column_list, (ParseNode*)identifier);
      (yyval.list_node) = column_list;
   }
#line 1781 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 39:
#line 361 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = NIDENTIFIER;
      identifier->identifier = (yyvsp[0].str_lit);

      ParseNode** column_list = (yyvsp[-2].list_node);
      arrpush(column_list, (ParseNode*)identifier);
      (yyval.list_node) = column_list;
  }
#line 1796 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 40:
#line 372 "parser.y"
    { (yyval.list_list_node) = (yyvsp[0].list_list_node); }
#line 1802 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 41:
#line 375 "parser.y"
    {
      ParseNode*** values_list = NULL;
      arrpush(values_list, (yyvsp[-1].list_node));
      (yyval.list_list_node) = values_list;
  }
#line 1812 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 42:
#line 380 "parser.y"
    {
    ParseNode*** values_list = (yyvsp[-4].list_list_node);
    arrpush(values_list, (yyvsp[-1].list_node));
    (yyval.list_list_node) = values_list;
  }
#line 1822 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 43:
#line 387 "parser.y"
    {
      ParseNode** value_items = NULL;
      arrpush(value_items, (yyvsp[0].node));
      (yyval.list_node) = value_items;
  }
#line 1832 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 44:
#line 392 "parser.y"
    {
      ParseNode** value_items = (yyvsp[-2].list_node);
      arrpush(value_items, (yyvsp[0].node));
      (yyval.list_node) = value_items;
  }
#line 1842 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 45:
#line 398 "parser.y"
    {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = NIDENTIFIER;
  identifier->identifier = (yyvsp[-2].str_lit);

  NDeleteStmt* delete_stmt = (NDeleteStmt*) calloc(1, sizeof(NDeleteStmt));
  delete_stmt->type = NDELETE_STMT;
  delete_stmt->table_name = (ParseNode*) identifier;
  delete_stmt->where_clause = (yyvsp[-1].node);
  (yyval.node) = (ParseNode*) delete_stmt;
}
#line 1859 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 46:
#line 411 "parser.y"
    {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = NIDENTIFIER;
  identifier->identifier = (yyvsp[-4].str_lit);

  NUpdateStmt* update = (NUpdateStmt*) calloc(1, sizeof(NUpdateStmt));
  update->type = NUPDATE_STMT;
  update->table_name = (ParseNode*) identifier;
  update->assign_expr_list = (yyvsp[-2].list_node);
  update->where_clause = (yyvsp[-1].node);
  (yyval.node) = (ParseNode*) update;
}
#line 1877 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 47:
#line 426 "parser.y"
    {
      ParseNode** value_items = NULL;
      arrpush(value_items, (yyvsp[0].node));
      (yyval.list_node) = value_items;
  }
#line 1887 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 48:
#line 431 "parser.y"
    {
      ParseNode** value_items = (yyvsp[-2].list_node);
      arrpush(value_items, (yyvsp[0].node));
      (yyval.list_node) = value_items;
  }
#line 1897 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 49:
#line 438 "parser.y"
    {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = NIDENTIFIER;
  identifier->identifier = (yyvsp[-2].str_lit);

  NAssignExpr* assign_expr = (NAssignExpr*) calloc(1, sizeof(NAssignExpr));
  assign_expr->type = NASSIGN_EXPR;
  assign_expr->column = (ParseNode*) identifier;
  assign_expr->value_expr = (yyvsp[0].node);
  (yyval.node) = (ParseNode*) assign_expr;
}
#line 1914 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 50:
#line 452 "parser.y"
    {
    NIdentifier* table_name = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(table_name != NULL);
    table_name->type = NIDENTIFIER;
    table_name->identifier = (yyvsp[-4].str_lit);

    NCreateTable* create_table = calloc(1, sizeof(NCreateTable));
    create_table->type = NCREATE_TABLE;
    create_table->table_name = (ParseNode*)table_name;
    create_table->column_defs = (yyvsp[-2].list_node);
    (yyval.node) = (ParseNode*)create_table;
  }
#line 1931 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 51:
#line 466 "parser.y"
    {
    NIdentifier* col_name = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(col_name != NULL);
    col_name->type = NIDENTIFIER;
    col_name->identifier = (yyvsp[-1].str_lit);

    NIdentifier* col_type = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(col_type != NULL);
    col_type->type = NIDENTIFIER;
    col_type->identifier = (yyvsp[0].str_lit);

    NColumnDef* column_def = calloc(1, sizeof(NColumnDef));
    column_def->type = NCOLUMN_DEF;
    column_def->col_name = (ParseNode*)col_name;
    column_def->col_type = (ParseNode*)col_type;

    ParseNode** column_defs = NULL;
    arrpush(column_defs, (ParseNode*)column_def);
    (yyval.list_node) = column_defs;
  }
#line 1956 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;

  case 52:
#line 486 "parser.y"
    {
    NIdentifier* col_name = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(col_name != NULL);
    col_name->type = NIDENTIFIER;
    col_name->identifier = (yyvsp[-1].str_lit);

    NIdentifier* col_type = (NIdentifier*)calloc(1, sizeof(NIdentifier));
    assert(col_type != NULL);
    col_type->type = NIDENTIFIER;
    col_type->identifier = (yyvsp[0].str_lit);

    NColumnDef* column_def = calloc(1, sizeof(NColumnDef));
    column_def->type = NCOLUMN_DEF;
    column_def->col_name = (ParseNode*)col_name;
    column_def->col_type = (ParseNode*)col_type;

    ParseNode** column_defs = (yyvsp[-3].list_node);
    arrpush(column_defs, (ParseNode*)column_def);
    (yyval.list_node) = column_defs;
  }
#line 1981 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"
    break;


#line 1985 "/home/rbeatty/Projects/BTDB/src/sql/parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (parser, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (parser, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, parser);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, parser);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (parser, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, parser);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, parser);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 508 "parser.y"


void yyerror(Parser* parser, const char* msg) {
  fprintf(stderr, "%s\n", msg);
}
