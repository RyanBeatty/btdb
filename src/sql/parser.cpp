// A Bison parser, made by GNU Bison 3.4.1.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2019 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.





#include "parser.hpp"


// Unqualified %code blocks.
#line 35 "parser.y"

  #include <cassert>
  #include <stdlib.h>
  #include <iostream>
  #include <memory>
  #include <string.h>
  #include "sql/context.hpp"

#line 54 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 129 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"


  /// Build a parser object.
  parser::parser (btdb::sql::ParserContext& ctx_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      ctx (ctx_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_number_type
  parser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state))
  {
    switch (that.type_get ())
    {
      case 30: // target_list
      case 35: // insert_column_list
      case 36: // column_list
      case 37: // insert_values_list
      case 38: // values_list
        value.YY_MOVE_OR_COPY< List* > (YY_MOVE (that.value));
        break;

      case 29: // select_stmt
      case 31: // from_clause
      case 32: // where_clause
      case 33: // expr
      case 34: // insert_stmt
        value.YY_MOVE_OR_COPY< ParseNode* > (YY_MOVE (that.value));
        break;

      case 25: // STRING_GROUP
      case 26: // STRING_LITERAL
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s)
  {
    switch (that.type_get ())
    {
      case 30: // target_list
      case 35: // insert_column_list
      case 36: // column_list
      case 37: // insert_values_list
      case 38: // values_list
        value.move< List* > (YY_MOVE (that.value));
        break;

      case 29: // select_stmt
      case 31: // from_clause
      case 32: // where_clause
      case 33: // expr
      case 34: // insert_stmt
        value.move< ParseNode* > (YY_MOVE (that.value));
        break;

      case 25: // STRING_GROUP
      case 26: // STRING_LITERAL
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 30: // target_list
      case 35: // insert_column_list
      case 36: // column_list
      case 37: // insert_values_list
      case 38: // values_list
        value.move< List* > (that.value);
        break;

      case 29: // select_stmt
      case 31: // from_clause
      case 32: // where_clause
      case 33: // expr
      case 34: // insert_stmt
        value.move< ParseNode* > (that.value);
        break;

      case 25: // STRING_GROUP
      case 26: // STRING_LITERAL
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " (";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (ctx));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case 30: // target_list
      case 35: // insert_column_list
      case 36: // column_list
      case 37: // insert_values_list
      case 38: // values_list
        yylhs.value.emplace< List* > ();
        break;

      case 29: // select_stmt
      case 31: // from_clause
      case 32: // where_clause
      case 33: // expr
      case 34: // insert_stmt
        yylhs.value.emplace< ParseNode* > ();
        break;

      case 25: // STRING_GROUP
      case 26: // STRING_LITERAL
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2:
#line 86 "parser.y"
    {
    ctx.tree = std::make_unique<ParseTree>(yystack_[0].value.as < ParseNode* > ());
  }
#line 574 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 3:
#line 89 "parser.y"
    {
    ctx.tree = std::make_unique<ParseTree>(yystack_[0].value.as < ParseNode* > ());
  }
#line 582 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 4:
#line 93 "parser.y"
    {
  NSelectStmt* select = (NSelectStmt*)calloc(1, sizeof(NSelectStmt));
  select->type = btdb::sql::NSELECT_STMT;
  select->target_list = yystack_[3].value.as < List* > ();
  select->table_name = yystack_[2].value.as < ParseNode* > ();
  select->where_clause = yystack_[1].value.as < ParseNode* > ();
  yylhs.value.as < ParseNode* > () = (ParseNode*) select;
}
#line 595 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 5:
#line 103 "parser.y"
    { 
      List* target_list = (List*)calloc(1, sizeof(List));
      // TODO: Don't hardcode the size of this;
      target_list->items = (ParseNode**)calloc(10, sizeof(ParseNode*));
      target_list->capacity = 10;

      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

      target_list->items[0] = (ParseNode*) identifier;
      target_list->length = 1;
      yylhs.value.as < List* > () = target_list;
    }
#line 617 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 6:
#line 120 "parser.y"
    { 
      auto* target_list = yystack_[0].value.as < List* > ();
      assert(target_list->length < target_list->capacity);

      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[2].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[2].value.as < std::string > ().c_str(), yystack_[2].value.as < std::string > ().length());

      target_list->items[target_list->length] = (ParseNode*) identifier;
      target_list->length++;
      // Don't actually think this is neccessary, but it is clear.
      yylhs.value.as < List* > () = target_list;
    }
#line 638 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 7:
#line 138 "parser.y"
    { yylhs.value.as < ParseNode* > () = nullptr; }
#line 644 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 8:
#line 139 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());
      yylhs.value.as < ParseNode* > () = (ParseNode*)identifier;
    }
#line 658 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 9:
#line 165 "parser.y"
    { yylhs.value.as < ParseNode* > () = nullptr; }
#line 664 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 10:
#line 166 "parser.y"
    {
      yylhs.value.as < ParseNode* > () = yystack_[0].value.as < ParseNode* > ();
    }
#line 672 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 11:
#line 175 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());
      yylhs.value.as < ParseNode* > () = (ParseNode*)identifier;
    }
#line 686 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 12:
#line 184 "parser.y"
    {
      // TODO(ryan): Need to remove leading and trailing ' characters. figure out better way.
      assert(yystack_[0].value.as < std::string > ().length() >= 2);
      yystack_[0].value.as < std::string > () = yystack_[0].value.as < std::string > ().substr(1, yystack_[0].value.as < std::string > ().length() - 2);
      NStringLit* str_lit = (NStringLit*)calloc(1, sizeof(NStringLit));
      assert(str_lit != NULL);
      str_lit->type = btdb::sql::NSTRING_LIT;
      str_lit->str_lit = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(str_lit->str_lit != NULL);
      strncpy(str_lit->str_lit, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());
      yylhs.value.as < ParseNode* > () = (ParseNode*)str_lit;
    }
#line 703 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 13:
#line 196 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::EQ;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 717 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 14:
#line 205 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::NEQ;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 731 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 15:
#line 214 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::GT;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 745 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 16:
#line 223 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::GE;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 759 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 17:
#line 232 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::LT;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 773 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 18:
#line 241 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::LE;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 787 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 19:
#line 250 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::PLUS;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 801 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 20:
#line 259 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::MINUS;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 815 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 21:
#line 268 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::MULT;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 829 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 22:
#line 277 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::DIV;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 843 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 23:
#line 286 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::AND;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 857 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 24:
#line 295 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::OR;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 871 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 25:
#line 305 "parser.y"
    {
  NInsertStmt* insert = (NInsertStmt*) calloc(1, sizeof(NInsertStmt));
  assert(insert != nullptr);
  insert->type = btdb::sql::NINSERT_STMT;

  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::sql::NIDENTIFIER;
  identifier->identifier = (char*)calloc(yystack_[3].value.as < std::string > ().length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, yystack_[3].value.as < std::string > ().c_str(), yystack_[3].value.as < std::string > ().length());

  insert->table_name = (ParseNode*) identifier;
  insert->column_list = yystack_[2].value.as < List* > ();
  insert->values_list = yystack_[1].value.as < List* > ();

  yylhs.value.as < ParseNode* > () = (ParseNode*) insert;
}
#line 894 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 26:
#line 324 "parser.y"
    { yylhs.value.as < List* > () = yystack_[1].value.as < List* > (); }
#line 900 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 27:
#line 327 "parser.y"
    {
      List* target_list = (List*)calloc(1, sizeof(List));
      // TODO: Don't hardcode the size of this;
      target_list->items = (ParseNode**)calloc(10, sizeof(ParseNode*));
      target_list->capacity = 10;

      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

      target_list->items[0] = (ParseNode*) identifier;
      target_list->length = 1;
      yylhs.value.as < List* > () = target_list;
   }
#line 922 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 28:
#line 344 "parser.y"
    {
      auto* column_list = yystack_[2].value.as < List* > ();
      assert(column_list->length < column_list->capacity);

      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

      column_list->items[column_list->length] = (ParseNode*) identifier;
      column_list->length++;
      // Don't actually think this is neccessary, but it is clear.
      yylhs.value.as < List* > () = column_list;
  }
#line 943 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 29:
#line 361 "parser.y"
    { yylhs.value.as < List* > () = yystack_[1].value.as < List* > (); }
#line 949 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 30:
#line 364 "parser.y"
    {
      List* target_list = (List*)calloc(1, sizeof(List));
      // TODO: Don't hardcode the size of this;
      target_list->items = (ParseNode**)calloc(10, sizeof(ParseNode*));
      target_list->capacity = 10;

      target_list->items[0] = yystack_[0].value.as < ParseNode* > ();
      target_list->length = 1;
      yylhs.value.as < List* > () = target_list;
  }
#line 964 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 31:
#line 374 "parser.y"
    {
      auto* values_list = yystack_[2].value.as < List* > ();
      assert(values_list->length < values_list->capacity);

      values_list->items[values_list->length] = yystack_[0].value.as < ParseNode* > ();
      values_list->length++;
      // Don't actually think this is neccessary, but it is clear.
      yylhs.value.as < List* > () = values_list;
  }
#line 978 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;


#line 982 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yysyntax_error_ (yystack_[0].state, yyla));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }


      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
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


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char parser::yypact_ninf_ = -29;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
       9,    20,    26,    52,   -29,   -29,    42,    45,    30,   -29,
      20,    31,    46,    50,   -29,   -29,    23,    49,    37,    54,
     -29,   -29,     2,   -29,   -29,    35,    56,    51,    23,    23,
      23,    23,    23,    23,    23,    23,    23,    23,    23,    23,
     -29,    39,    23,   -29,    18,    18,     6,     6,     6,     6,
       6,     6,    27,    27,   -29,   -29,   -29,     2,    36,   -29,
      23,     2
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     2,     3,     5,     7,     0,     1,
       0,     0,     9,     0,     6,     8,     0,     0,     0,     0,
      11,    12,    10,     4,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      26,     0,     0,    25,    23,    24,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    28,    30,     0,    29,
       0,    31
  };

  const signed char
  parser::yypgoto_[] =
  {
     -29,   -29,   -29,    55,   -29,   -29,   -28,   -29,   -29,   -29,
     -29,   -29
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     3,     4,     7,    12,    17,    22,     5,    19,    25,
      27,    58
  };

  const unsigned char
  parser::yytable_[] =
  {
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,     1,     2,    57,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    36,    37,    38,
      39,     8,    61,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    59,     6,    41,    60,    20,    21,
      38,    39,     9,    10,    11,    13,    15,    18,    16,    23,
      26,    43,    24,    42,    56,    14
  };

  const unsigned char
  parser::yycheck_[] =
  {
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,     3,     4,    42,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    21,    22,    23,
      24,     5,    60,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,     8,     8,    25,    11,    11,    25,    26,
      23,    24,     0,    11,     9,    25,    25,     7,    12,    10,
       6,    10,    25,     7,    25,    10
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     3,     4,    28,    29,    34,    25,    30,     5,     0,
      11,     9,    31,    25,    30,    25,    12,    32,     7,    35,
      25,    26,    33,    10,    25,    36,     6,    37,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
       8,    11,     7,    10,    33,    33,    33,    33,    33,    33,
      33,    33,    33,    33,    33,    33,    25,    33,    38,     8,
      11,    33
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    27,    28,    28,    29,    30,    30,    31,    31,    32,
      32,    33,    33,    33,    33,    33,    33,    33,    33,    33,
      33,    33,    33,    33,    33,    34,    35,    36,    36,    37,
      38,    38
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,     1,     1,     5,     1,     3,     0,     2,     0,
       2,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     6,     3,     1,     3,     4,
       1,     3
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "EOF", "error", "$undefined", "SELECT", "INSERT", "INTO", "VALUES",
  "\"(\"", "\")\"", "FROM", "\";\"", "\",\"", "WHERE", "AND", "OR",
  "\"=\"", "\"!=\"", "\">\"", "\">=\"", "\"<\"", "\"<=\"", "\"+\"",
  "\"-\"", "\"*\"", "\"/\"", "STRING_GROUP", "STRING_LITERAL", "$accept",
  "stmt", "select_stmt", "target_list", "from_clause", "where_clause",
  "expr", "insert_stmt", "insert_column_list", "column_list",
  "insert_values_list", "values_list", YY_NULLPTR
  };


  const unsigned short
  parser::yyrline_[] =
  {
       0,    86,    86,    89,    93,   103,   120,   138,   139,   165,
     166,   175,   184,   196,   205,   214,   223,   232,   241,   250,
     259,   268,   277,   286,   295,   305,   324,   327,   344,   361,
     364,   374
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    unsigned yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 1312 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"

#line 385 "parser.y"


void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
