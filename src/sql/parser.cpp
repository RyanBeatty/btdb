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
#line 40 "parser.y"

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
      case 33: // target_list
      case 38: // insert_column_list
      case 39: // column_list
      case 40: // insert_values_clause
      case 41: // insert_values_list
      case 42: // insert_value_items
      case 45: // update_assign_expr_list
        value.YY_MOVE_OR_COPY< List* > (YY_MOVE (that.value));
        break;

      case 32: // select_stmt
      case 34: // from_clause
      case 35: // where_clause
      case 36: // expr
      case 37: // insert_stmt
      case 43: // delete_stmt
      case 44: // update_stmt
      case 46: // assign_expr
        value.YY_MOVE_OR_COPY< ParseNode* > (YY_MOVE (that.value));
        break;

      case 28: // STRING_GROUP
      case 29: // STRING_LITERAL
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
      case 33: // target_list
      case 38: // insert_column_list
      case 39: // column_list
      case 40: // insert_values_clause
      case 41: // insert_values_list
      case 42: // insert_value_items
      case 45: // update_assign_expr_list
        value.move< List* > (YY_MOVE (that.value));
        break;

      case 32: // select_stmt
      case 34: // from_clause
      case 35: // where_clause
      case 36: // expr
      case 37: // insert_stmt
      case 43: // delete_stmt
      case 44: // update_stmt
      case 46: // assign_expr
        value.move< ParseNode* > (YY_MOVE (that.value));
        break;

      case 28: // STRING_GROUP
      case 29: // STRING_LITERAL
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
      case 33: // target_list
      case 38: // insert_column_list
      case 39: // column_list
      case 40: // insert_values_clause
      case 41: // insert_values_list
      case 42: // insert_value_items
      case 45: // update_assign_expr_list
        value.move< List* > (that.value);
        break;

      case 32: // select_stmt
      case 34: // from_clause
      case 35: // where_clause
      case 36: // expr
      case 37: // insert_stmt
      case 43: // delete_stmt
      case 44: // update_stmt
      case 46: // assign_expr
        value.move< ParseNode* > (that.value);
        break;

      case 28: // STRING_GROUP
      case 29: // STRING_LITERAL
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
      case 33: // target_list
      case 38: // insert_column_list
      case 39: // column_list
      case 40: // insert_values_clause
      case 41: // insert_values_list
      case 42: // insert_value_items
      case 45: // update_assign_expr_list
        yylhs.value.emplace< List* > ();
        break;

      case 32: // select_stmt
      case 34: // from_clause
      case 35: // where_clause
      case 36: // expr
      case 37: // insert_stmt
      case 43: // delete_stmt
      case 44: // update_stmt
      case 46: // assign_expr
        yylhs.value.emplace< ParseNode* > ();
        break;

      case 28: // STRING_GROUP
      case 29: // STRING_LITERAL
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
#line 94 "parser.y"
    {
    ctx.tree = std::make_unique<ParseTree>(yystack_[0].value.as < ParseNode* > ());
  }
#line 594 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 3:
#line 97 "parser.y"
    {
    ctx.tree = std::make_unique<ParseTree>(yystack_[0].value.as < ParseNode* > ());
  }
#line 602 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 4:
#line 100 "parser.y"
    {
    ctx.tree = std::make_unique<ParseTree>(yystack_[0].value.as < ParseNode* > ());
  }
#line 610 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 5:
#line 103 "parser.y"
    {
    ctx.tree = std::make_unique<ParseTree>(yystack_[0].value.as < ParseNode* > ());
  }
#line 618 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 6:
#line 107 "parser.y"
    {
  NSelectStmt* select = (NSelectStmt*)calloc(1, sizeof(NSelectStmt));
  select->type = btdb::NSELECT_STMT;
  select->target_list = yystack_[3].value.as < List* > ();
  select->table_name = yystack_[2].value.as < ParseNode* > ();
  select->where_clause = yystack_[1].value.as < ParseNode* > ();
  yylhs.value.as < ParseNode* > () = (ParseNode*) select;
}
#line 631 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 7:
#line 117 "parser.y"
    { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

      List* target_list = make_list(btdb::T_PARSENODE);
      target_list->type = btdb::T_PARSENODE;
      push_list(target_list, identifier);
      yylhs.value.as < List* > () = target_list;
    }
#line 649 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 8:
#line 130 "parser.y"
    { 
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[2].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[2].value.as < std::string > ().c_str(), yystack_[2].value.as < std::string > ().length());

      auto* target_list = yystack_[0].value.as < List* > ();
      push_list(target_list, identifier);
      yylhs.value.as < List* > () = target_list;
    }
#line 666 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 9:
#line 144 "parser.y"
    { yylhs.value.as < ParseNode* > () = nullptr; }
#line 672 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 10:
#line 145 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());
      yylhs.value.as < ParseNode* > () = (ParseNode*)identifier;
    }
#line 686 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 11:
#line 171 "parser.y"
    { yylhs.value.as < ParseNode* > () = nullptr; }
#line 692 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 12:
#line 172 "parser.y"
    {
      yylhs.value.as < ParseNode* > () = yystack_[0].value.as < ParseNode* > ();
    }
#line 700 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 13:
#line 181 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());
      yylhs.value.as < ParseNode* > () = (ParseNode*)identifier;
    }
#line 714 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 14:
#line 190 "parser.y"
    {
      // TODO(ryan): Need to remove leading and trailing ' characters. figure out better way.
      assert(yystack_[0].value.as < std::string > ().length() >= 2);
      yystack_[0].value.as < std::string > () = yystack_[0].value.as < std::string > ().substr(1, yystack_[0].value.as < std::string > ().length() - 2);
      NStringLit* str_lit = (NStringLit*)calloc(1, sizeof(NStringLit));
      assert(str_lit != NULL);
      str_lit->type = btdb::NSTRING_LIT;
      str_lit->str_lit = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(str_lit->str_lit != NULL);
      strncpy(str_lit->str_lit, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());
      yylhs.value.as < ParseNode* > () = (ParseNode*)str_lit;
    }
#line 731 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 15:
#line 202 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::EQ;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 745 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 16:
#line 211 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::NEQ;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 759 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 17:
#line 220 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::GT;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 773 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 18:
#line 229 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::GE;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 787 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 19:
#line 238 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::LT;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 801 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 20:
#line 247 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::LE;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 815 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 21:
#line 256 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::PLUS;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 829 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 22:
#line 265 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::MINUS;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 843 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 23:
#line 274 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::MULT;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 857 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 24:
#line 283 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::DIV;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 871 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 25:
#line 292 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::AND;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 885 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 26:
#line 301 "parser.y"
    { 
      NBinExpr* bin_expr = (NBinExpr*)calloc(1, sizeof(NBinExpr));
      assert(bin_expr != NULL);
      bin_expr->type = btdb::NBIN_EXPR;
      bin_expr->op = btdb::OR;
      bin_expr->lhs = yystack_[2].value.as < ParseNode* > ();
      bin_expr->rhs = yystack_[0].value.as < ParseNode* > ();
      yylhs.value.as < ParseNode* > () = (ParseNode*)bin_expr;
    }
#line 899 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 27:
#line 311 "parser.y"
    {
  NInsertStmt* insert = (NInsertStmt*) calloc(1, sizeof(NInsertStmt));
  assert(insert != nullptr);
  insert->type = btdb::NINSERT_STMT;

  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::NIDENTIFIER;
  identifier->identifier = (char*)calloc(yystack_[3].value.as < std::string > ().length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, yystack_[3].value.as < std::string > ().c_str(), yystack_[3].value.as < std::string > ().length());

  insert->table_name = (ParseNode*) identifier;
  insert->column_list = yystack_[2].value.as < List* > ();
  insert->values_list = yystack_[1].value.as < List* > ();

  yylhs.value.as < ParseNode* > () = (ParseNode*) insert;
}
#line 922 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 28:
#line 330 "parser.y"
    { yylhs.value.as < List* > () = yystack_[1].value.as < List* > (); }
#line 928 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 29:
#line 333 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

      List* column_list = make_list(btdb::T_PARSENODE);
      push_list(column_list, identifier);
      yylhs.value.as < List* > () = column_list;
   }
#line 945 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 30:
#line 345 "parser.y"
    {
      NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
      assert(identifier != NULL);
      identifier->type = btdb::NIDENTIFIER;
      identifier->identifier = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
      assert(identifier->identifier != NULL);
      strncpy(identifier->identifier, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

      auto* column_list = yystack_[2].value.as < List* > ();
      push_list(column_list, identifier);
      yylhs.value.as < List* > () = column_list;
  }
#line 962 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 31:
#line 358 "parser.y"
    { yylhs.value.as < List* > () = yystack_[0].value.as < List* > (); }
#line 968 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 32:
#line 360 "parser.y"
    {
      List* values_list = make_list(btdb::T_LIST);
      push_list(values_list, yystack_[1].value.as < List* > ());
      yylhs.value.as < List* > () = values_list;
  }
#line 978 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 33:
#line 365 "parser.y"
    {
    auto* values_list = yystack_[4].value.as < List* > ();
    push_list(values_list, yystack_[1].value.as < List* > ());
    yylhs.value.as < List* > () = values_list;
  }
#line 988 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 34:
#line 372 "parser.y"
    {
      List* value_items = make_list(btdb::T_PARSENODE);
      push_list(value_items, yystack_[0].value.as < ParseNode* > ());
      yylhs.value.as < List* > () = value_items;
  }
#line 998 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 35:
#line 377 "parser.y"
    {
      auto* value_items = yystack_[2].value.as < List* > ();
      push_list(value_items, yystack_[0].value.as < ParseNode* > ());
      yylhs.value.as < List* > () = value_items;
  }
#line 1008 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 36:
#line 383 "parser.y"
    {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::NIDENTIFIER;
  identifier->identifier = (char*)calloc(yystack_[2].value.as < std::string > ().length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, yystack_[2].value.as < std::string > ().c_str(), yystack_[2].value.as < std::string > ().length());

  NDeleteStmt* delete_stmt = (NDeleteStmt*) calloc(1, sizeof(NDeleteStmt));
  delete_stmt->type = btdb::NDELETE_STMT;
  delete_stmt->table_name = (ParseNode*) identifier;
  delete_stmt->where_clause = yystack_[1].value.as < ParseNode* > ();
  yylhs.value.as < ParseNode* > () = (ParseNode*) delete_stmt;
}
#line 1027 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 37:
#line 398 "parser.y"
    {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::NIDENTIFIER;
  identifier->identifier = (char*)calloc(yystack_[4].value.as < std::string > ().length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, yystack_[4].value.as < std::string > ().c_str(), yystack_[4].value.as < std::string > ().length());

  NUpdateStmt* update = (NUpdateStmt*) calloc(1, sizeof(NUpdateStmt));
  update->type = btdb::NUPDATE_STMT;
  update->table_name = (ParseNode*) identifier;
  update->assign_expr_list = yystack_[2].value.as < List* > ();
  update->where_clause = yystack_[1].value.as < ParseNode* > ();
  yylhs.value.as < ParseNode* > () = (ParseNode*) update;
}
#line 1047 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 38:
#line 415 "parser.y"
    {
      List* value_items = make_list(btdb::T_PARSENODE);
      push_list(value_items, yystack_[0].value.as < ParseNode* > ());
      yylhs.value.as < List* > () = value_items;
  }
#line 1057 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 39:
#line 420 "parser.y"
    {
      auto* value_items = yystack_[2].value.as < List* > ();
      push_list(value_items, yystack_[0].value.as < ParseNode* > ());
      yylhs.value.as < List* > () = value_items;
  }
#line 1067 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 40:
#line 426 "parser.y"
    {
  NIdentifier* identifier = (NIdentifier*)calloc(1, sizeof(NIdentifier));
  assert(identifier != NULL);
  identifier->type = btdb::NIDENTIFIER;
  identifier->identifier = (char*)calloc(yystack_[2].value.as < std::string > ().length(), sizeof(char));
  assert(identifier->identifier != NULL);
  strncpy(identifier->identifier, yystack_[2].value.as < std::string > ().c_str(), yystack_[2].value.as < std::string > ().length());

  // TODO(ryan): Need to remove leading and trailing ' characters. figure out better way.
  assert(yystack_[0].value.as < std::string > ().length() >= 2);
  yystack_[0].value.as < std::string > () = yystack_[0].value.as < std::string > ().substr(1, yystack_[0].value.as < std::string > ().length() - 2);
  NStringLit* str_lit = (NStringLit*)calloc(1, sizeof(NStringLit));
  assert(str_lit != NULL);
  str_lit->type = btdb::NSTRING_LIT;
  str_lit->str_lit = (char*)calloc(yystack_[0].value.as < std::string > ().length(), sizeof(char));
  assert(str_lit->str_lit != NULL);
  strncpy(str_lit->str_lit, yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length());

  NAssignExpr* assign_expr = (NAssignExpr*) calloc(1, sizeof(NAssignExpr));
  assign_expr->type = btdb::NASSIGN_EXPR;
  assign_expr->column = (ParseNode*) identifier;
  assign_expr->value = (ParseNode*) str_lit;
  yylhs.value.as < ParseNode* > () = (ParseNode*) assign_expr;
}
#line 1096 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;


#line 1100 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"

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


  const signed char parser::yypact_ninf_ = -25;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
      10,    -9,    53,    49,    11,    62,   -25,   -25,   -25,   -25,
      50,    51,    37,    38,    58,   -25,    -9,    41,    55,    61,
      55,    44,   -25,   -25,   -11,    60,    46,    67,    63,    59,
      40,   -25,   -25,   -25,   -15,   -25,   -25,    26,    68,    66,
     -25,    39,    44,    69,   -11,   -11,   -11,   -11,   -11,   -11,
     -11,   -11,   -11,   -11,   -11,   -11,   -25,    52,   -11,    70,
     -25,   -25,   -25,   -25,    25,    25,     9,     9,     9,     9,
       9,     9,    32,    32,   -25,   -25,   -25,   -15,    27,    71,
     -25,   -11,   -11,   -15,    42,   -25
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     0,     2,     3,     4,     5,
       7,     9,     0,     0,     0,     1,     0,     0,    11,     0,
      11,     0,     8,    10,     0,     0,     0,     0,     0,     0,
      11,    38,    13,    14,    12,     6,    29,     0,     0,     0,
      36,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    28,     0,     0,    31,
      27,    40,    39,    37,    25,    26,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    30,    34,     0,     0,
      32,     0,     0,    35,     0,    33
  };

  const signed char
  parser::yypgoto_[] =
  {
     -25,   -25,   -25,    72,   -25,    12,   -24,   -25,   -25,   -25,
     -25,   -25,     1,   -25,   -25,   -25,    43
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     5,     6,    11,    18,    25,    77,     7,    27,    37,
      39,    59,    78,     8,     9,    30,    31
  };

  const unsigned char
  parser::yytable_[] =
  {
      34,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,     1,     2,     3,     4,    32,    33,    10,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    28,    52,    53,    54,    55,    56,    80,    14,
      57,    81,    43,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    85,    42,    24,    81,    83,    54,    55,
      12,    13,    15,    17,    16,    19,    20,    21,    61,    23,
      24,    26,    29,    35,    36,    38,    40,    41,    58,    60,
      76,    82,    63,    84,    79,    62,     0,     0,    22
  };

  const signed char
  parser::yycheck_[] =
  {
      24,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,     3,     4,     5,     6,    28,    29,    28,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    20,    24,    25,    26,    27,    11,    11,    28,
      14,    14,    30,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    11,    14,    15,    14,    81,    26,    27,
       7,    12,     0,    12,    14,    28,    28,     9,    29,    28,
      15,    10,    28,    13,    28,     8,    13,    18,    10,    13,
      28,    10,    13,    82,    14,    42,    -1,    -1,    16
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     3,     4,     5,     6,    31,    32,    37,    43,    44,
      28,    33,     7,    12,    28,     0,    14,    12,    34,    28,
      28,     9,    33,    28,    15,    35,    10,    38,    35,    28,
      45,    46,    28,    29,    36,    13,    28,    39,     8,    40,
      13,    18,    14,    35,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    11,    14,    10,    41,
      13,    29,    46,    13,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    36,    36,    28,    36,    42,    14,
      11,    14,    10,    36,    42,    11
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    30,    31,    31,    31,    31,    32,    33,    33,    34,
      34,    35,    35,    36,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    36,    36,    36,    37,    38,    39,
      39,    40,    41,    41,    42,    42,    43,    44,    45,    45,
      46
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     5,     1,     3,     0,
       2,     0,     2,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     1,
       3,     2,     3,     5,     1,     3,     5,     6,     1,     3,
       3
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "EOF", "error", "$undefined", "SELECT", "INSERT", "DELETE", "UPDATE",
  "INTO", "VALUES", "SET", "\"(\"", "\")\"", "FROM", "\";\"", "\",\"",
  "WHERE", "AND", "OR", "\"=\"", "\"!=\"", "\">\"", "\">=\"", "\"<\"",
  "\"<=\"", "\"+\"", "\"-\"", "\"*\"", "\"/\"", "STRING_GROUP",
  "STRING_LITERAL", "$accept", "stmt", "select_stmt", "target_list",
  "from_clause", "where_clause", "expr", "insert_stmt",
  "insert_column_list", "column_list", "insert_values_clause",
  "insert_values_list", "insert_value_items", "delete_stmt", "update_stmt",
  "update_assign_expr_list", "assign_expr", YY_NULLPTR
  };


  const unsigned short
  parser::yyrline_[] =
  {
       0,    94,    94,    97,   100,   103,   107,   117,   130,   144,
     145,   171,   172,   181,   190,   202,   211,   220,   229,   238,
     247,   256,   265,   274,   283,   292,   301,   311,   330,   333,
     345,   358,   360,   365,   372,   377,   383,   398,   415,   420,
     426
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
#line 1445 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"

#line 452 "parser.y"


void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
