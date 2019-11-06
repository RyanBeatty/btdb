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
#line 38 "parser.y"

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
      case 31: // target_list
      case 36: // insert_column_list
      case 37: // column_list
      case 38: // insert_values_clause
      case 39: // insert_values_list
      case 40: // insert_value_items
        value.YY_MOVE_OR_COPY< List* > (YY_MOVE (that.value));
        break;

      case 30: // select_stmt
      case 32: // from_clause
      case 33: // where_clause
      case 34: // expr
      case 35: // insert_stmt
      case 41: // delete_stmt
        value.YY_MOVE_OR_COPY< ParseNode* > (YY_MOVE (that.value));
        break;

      case 26: // STRING_GROUP
      case 27: // STRING_LITERAL
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
      case 31: // target_list
      case 36: // insert_column_list
      case 37: // column_list
      case 38: // insert_values_clause
      case 39: // insert_values_list
      case 40: // insert_value_items
        value.move< List* > (YY_MOVE (that.value));
        break;

      case 30: // select_stmt
      case 32: // from_clause
      case 33: // where_clause
      case 34: // expr
      case 35: // insert_stmt
      case 41: // delete_stmt
        value.move< ParseNode* > (YY_MOVE (that.value));
        break;

      case 26: // STRING_GROUP
      case 27: // STRING_LITERAL
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
      case 31: // target_list
      case 36: // insert_column_list
      case 37: // column_list
      case 38: // insert_values_clause
      case 39: // insert_values_list
      case 40: // insert_value_items
        value.move< List* > (that.value);
        break;

      case 30: // select_stmt
      case 32: // from_clause
      case 33: // where_clause
      case 34: // expr
      case 35: // insert_stmt
      case 41: // delete_stmt
        value.move< ParseNode* > (that.value);
        break;

      case 26: // STRING_GROUP
      case 27: // STRING_LITERAL
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
      case 31: // target_list
      case 36: // insert_column_list
      case 37: // column_list
      case 38: // insert_values_clause
      case 39: // insert_values_list
      case 40: // insert_value_items
        yylhs.value.emplace< List* > ();
        break;

      case 30: // select_stmt
      case 32: // from_clause
      case 33: // where_clause
      case 34: // expr
      case 35: // insert_stmt
      case 41: // delete_stmt
        yylhs.value.emplace< ParseNode* > ();
        break;

      case 26: // STRING_GROUP
      case 27: // STRING_LITERAL
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
#line 90 "parser.y"
    {
    ctx.tree = std::make_unique<ParseTree>(yystack_[0].value.as < ParseNode* > ());
  }
#line 582 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 3:
#line 93 "parser.y"
    {
    ctx.tree = std::make_unique<ParseTree>(yystack_[0].value.as < ParseNode* > ());
  }
#line 590 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 4:
#line 96 "parser.y"
    {
    ctx.tree = std::make_unique<ParseTree>(yystack_[0].value.as < ParseNode* > ());
  }
#line 598 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 5:
#line 100 "parser.y"
    {
  NSelectStmt* select = (NSelectStmt*)calloc(1, sizeof(NSelectStmt));
  select->type = btdb::sql::NSELECT_STMT;
  select->target_list = yystack_[3].value.as < List* > ();
  select->table_name = yystack_[2].value.as < ParseNode* > ();
  select->where_clause = yystack_[1].value.as < ParseNode* > ();
  yylhs.value.as < ParseNode* > () = (ParseNode*) select;
}
#line 611 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 6:
#line 110 "parser.y"
    { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

      List* target_list = make_list(btdb::sql::T_PARSENODE);
      push_list(target_list, identifier);
      yylhs.value.as < List* > () = target_list;
    }
#line 628 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 7:
#line 122 "parser.y"
    { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[2].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[2].value.as < std::string > ().c_str(), yystack_[2].value.as < std::string > ().length());

      auto* target_list = yystack_[0].value.as < List* > ();
      push_list(target_list, identifier);
      yylhs.value.as < List* > () = target_list;
    }
#line 645 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 8:
#line 136 "parser.y"
    { yylhs.value.as < ParseNode* > () = nullptr; }
#line 651 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 9:
#line 137 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());
      yylhs.value.as < ParseNode* > () = (ParseNode*)identifier;
    }
#line 665 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 10:
#line 163 "parser.y"
    { yylhs.value.as < ParseNode* > () = nullptr; }
#line 671 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 11:
#line 164 "parser.y"
    {
      yylhs.value.as < ParseNode* > () = yystack_[0].value.as < ParseNode* > ();
    }
#line 679 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 12:
#line 173 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());
      yylhs.value.as < ParseNode* > () = (ParseNode*)identifier;
    }
#line 693 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 13:
#line 182 "parser.y"
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
#line 710 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 14:
#line 194 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::EQ;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 724 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 15:
#line 203 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::NEQ;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 738 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 16:
#line 212 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::GT;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 752 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 17:
#line 221 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::GE;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 766 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 18:
#line 230 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::LT;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 780 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 19:
#line 239 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::LE;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 794 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 20:
#line 248 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::PLUS;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 808 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 21:
#line 257 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::MINUS;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 822 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 22:
#line 266 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::MULT;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 836 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 23:
#line 275 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::DIV;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 850 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 24:
#line 284 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::AND;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 864 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 25:
#line 293 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::sql::NBIN_EXPR;
      bin_expr->op = btdb::sql::OR;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 878 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 26:
#line 303 "parser.y"
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
#line 901 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 27:
#line 322 "parser.y"
    { yylhs.value.as < List* > () = yystack_[1].value.as < List* > (); }
#line 907 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 28:
#line 325 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

      List* column_list = make_list(btdb::sql::T_PARSENODE);
      push_list(column_list, identifier);
      yylhs.value.as < List* > () = column_list;
   }
#line 924 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 29:
#line 337 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::sql::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

      auto* column_list = yystack_[2].value.as < List* > ();
      push_list(column_list, identifier);
      yylhs.value.as < List* > () = column_list;
  }
#line 941 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 30:
#line 350 "parser.y"
    { yylhs.value.as < List* > () = yystack_[0].value.as < List* > (); }
#line 947 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 31:
#line 352 "parser.y"
    {
      List* values_list = make_list(btdb::sql::T_LIST);
      push_list(values_list, yystack_[1].value.as < List* > ());
      yylhs.value.as < List* > () = values_list;
  }
#line 957 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 32:
#line 357 "parser.y"
    {
    auto* values_list = yystack_[4].value.as < List* > ();
    push_list(values_list, yystack_[1].value.as < List* > ());
    yylhs.value.as < List* > () = values_list;
  }
#line 967 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 33:
#line 364 "parser.y"
    {
      List* value_items = make_list(btdb::sql::T_PARSENODE);
      push_list(value_items, yystack_[0].value.as < ParseNode* > ());
      yylhs.value.as < List* > () = value_items;
  }
#line 977 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 34:
#line 369 "parser.y"
    {
      auto* value_items = yystack_[2].value.as < List* > ();
      push_list(value_items, yystack_[0].value.as < ParseNode* > ());
      yylhs.value.as < List* > () = value_items;
  }
#line 987 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 35:
#line 375 "parser.y"
    {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::sql::NIDENTIFIER;
  identifier->identifier = (char*)calloc(yystack_[2].value.as < std::string > ().length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, yystack_[2].value.as < std::string > ().c_str(), yystack_[2].value.as < std::string > ().length());

  NDeleteStmt* delete_stmt = (NDeleteStmt*) calloc(1, sizeof(NDeleteStmt));
  delete_stmt->type = btdb::sql::NDELETE_STMT;
  delete_stmt->table_name = (ParseNode*) identifier;
  delete_stmt->where_clause = yystack_[1].value.as < ParseNode* > ();
  yylhs.value.as < ParseNode* > () = (ParseNode*) delete_stmt;
}
#line 1006 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;


#line 1010 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"

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


  const signed char parser::yypact_ninf_ = -21;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
      46,    29,     7,    44,    56,   -21,   -21,   -21,    45,    48,
      34,    35,   -21,    29,    36,    50,    51,    50,   -21,   -21,
      20,    53,    39,    59,    57,   -21,   -21,   -13,   -21,   -21,
      31,    61,    60,   -21,    20,    20,    20,    20,    20,    20,
      20,    20,    20,    20,    20,    20,   -21,    41,    20,    58,
     -21,    10,    10,    14,    14,    14,    14,    14,    14,    28,
      28,   -21,   -21,   -21,   -13,    32,    64,   -21,    20,    20,
     -13,    33,   -21
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     2,     3,     4,     6,     8,
       0,     0,     1,     0,     0,    10,     0,    10,     7,     9,
       0,     0,     0,     0,     0,    12,    13,    11,     5,    28,
       0,     0,     0,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    27,     0,     0,    30,
      26,    24,    25,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    29,    33,     0,     0,    31,     0,     0,
      34,     0,    32
  };

  const signed char
  parser::yypgoto_[] =
  {
     -21,   -21,   -21,    62,   -21,    63,   -20,   -21,   -21,   -21,
     -21,   -21,     4,   -21
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     4,     5,     9,    15,    21,    64,     6,    23,    30,
      32,    49,    65,     7
  };

  const unsigned char
  parser::yytable_[] =
  {
      27,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    10,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    42,    43,    44,    45,
      46,    67,    72,    47,    68,    68,    25,    26,    70,     1,
       2,     3,    44,    45,    11,     8,    12,    13,    14,    22,
      16,    17,    19,    20,    28,    29,    31,    63,    33,    48,
      66,    50,    69,    71,     0,    18,     0,     0,     0,     0,
      24
  };

  const signed char
  parser::yycheck_[] =
  {
      20,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,     6,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    22,    23,    24,    25,
       9,     9,     9,    12,    12,    12,    26,    27,    68,     3,
       4,     5,    24,    25,    10,    26,     0,    12,    10,     8,
      26,    26,    26,    13,    11,    26,     7,    26,    11,     8,
      12,    11,     8,    69,    -1,    13,    -1,    -1,    -1,    -1,
      17
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     3,     4,     5,    29,    30,    35,    41,    26,    31,
       6,    10,     0,    12,    10,    32,    26,    26,    31,    26,
      13,    33,     8,    36,    33,    26,    27,    34,    11,    26,
      37,     7,    38,    11,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,     9,    12,     8,    39,
      11,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    26,    34,    40,    12,     9,    12,     8,
      34,    40,     9
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    28,    29,    29,    29,    30,    31,    31,    32,    32,
      33,    33,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    35,    36,    37,    37,
      38,    39,    39,    40,    40,    41
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     5,     1,     3,     0,     2,
       0,     2,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     6,     3,     1,     3,
       2,     3,     5,     1,     3,     5
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "EOF", "error", "$undefined", "SELECT", "INSERT", "DELETE", "INTO",
  "VALUES", "\"(\"", "\")\"", "FROM", "\";\"", "\",\"", "WHERE", "AND",
  "OR", "\"=\"", "\"!=\"", "\">\"", "\">=\"", "\"<\"", "\"<=\"", "\"+\"",
  "\"-\"", "\"*\"", "\"/\"", "STRING_GROUP", "STRING_LITERAL", "$accept",
  "stmt", "select_stmt", "target_list", "from_clause", "where_clause",
  "expr", "insert_stmt", "insert_column_list", "column_list",
  "insert_values_clause", "insert_values_list", "insert_value_items",
  "delete_stmt", YY_NULLPTR
  };


  const unsigned short
  parser::yyrline_[] =
  {
       0,    90,    90,    93,    96,   100,   110,   122,   136,   137,
     163,   164,   173,   182,   194,   203,   212,   221,   230,   239,
     248,   257,   266,   275,   284,   293,   303,   322,   325,   337,
     350,   352,   357,   364,   369,   375
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
#line 1348 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"

#line 391 "parser.y"


void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
