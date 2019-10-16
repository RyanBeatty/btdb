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
#line 27 "parser.y"

  #include <stdio.h>
  #include <iostream>
  #include <memory>
  #include "context.hpp"

#line 52 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"


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
#line 127 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"


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
      case 20: // STRING_GROUP
      case 21: // STRING_LITERAL
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case 26: // where_clause
        value.YY_MOVE_OR_COPY< std::unique_ptr<btdb::sql::WhereClause> > (YY_MOVE (that.value));
        break;

      case 25: // column_exp
        value.YY_MOVE_OR_COPY< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      case 27: // bool_expr
      case 28: // expr
      case 29: // factor
        value.YY_MOVE_OR_COPY< void* > (YY_MOVE (that.value));
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
      case 20: // STRING_GROUP
      case 21: // STRING_LITERAL
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case 26: // where_clause
        value.move< std::unique_ptr<btdb::sql::WhereClause> > (YY_MOVE (that.value));
        break;

      case 25: // column_exp
        value.move< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      case 27: // bool_expr
      case 28: // expr
      case 29: // factor
        value.move< void* > (YY_MOVE (that.value));
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
      case 20: // STRING_GROUP
      case 21: // STRING_LITERAL
        value.move< std::string > (that.value);
        break;

      case 26: // where_clause
        value.move< std::unique_ptr<btdb::sql::WhereClause> > (that.value);
        break;

      case 25: // column_exp
        value.move< std::vector<std::string> > (that.value);
        break;

      case 27: // bool_expr
      case 28: // expr
      case 29: // factor
        value.move< void* > (that.value);
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
      case 20: // STRING_GROUP
      case 21: // STRING_LITERAL
        yylhs.value.emplace< std::string > ();
        break;

      case 26: // where_clause
        yylhs.value.emplace< std::unique_ptr<btdb::sql::WhereClause> > ();
        break;

      case 25: // column_exp
        yylhs.value.emplace< std::vector<std::string> > ();
        break;

      case 27: // bool_expr
      case 28: // expr
      case 29: // factor
        yylhs.value.emplace< void* > ();
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
#line 73 "parser.y"
    { 
  btdb::sql::SelectStmt sel(yystack_[4].value.as < std::vector<std::string> > (), yystack_[2].value.as < std::string > (), std::move(yystack_[1].value.as < std::unique_ptr<btdb::sql::WhereClause> > ()));
  ctx.stmt = std::move(sel);
}
#line 565 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 3:
#line 79 "parser.y"
    { yylhs.value.as < std::vector<std::string> > () = std::vector<std::string>{yystack_[0].value.as < std::string > ()}; }
#line 571 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 4:
#line 80 "parser.y"
    { yylhs.value.as < std::vector<std::string> > () = yystack_[0].value.as < std::vector<std::string> > (); yylhs.value.as < std::vector<std::string> > ().push_back(yystack_[2].value.as < std::string > ()); }
#line 577 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 5:
#line 84 "parser.y"
    { yylhs.value.as < std::unique_ptr<btdb::sql::WhereClause> > () = nullptr; }
#line 583 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 6:
#line 85 "parser.y"
    { 
    //$$ = std::make_unique<btdb::sql::WhereClause>(btdb::sql::WhereClause{ $2, $4});
    yylhs.value.as < std::unique_ptr<btdb::sql::WhereClause> > () = nullptr;
  }
#line 592 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 7:
#line 91 "parser.y"
    {yylhs.value.as < void* > () = yystack_[0].value.as < void* > ();}
#line 598 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 8:
#line 92 "parser.y"
    { yylhs.value.as < void* > () = nullptr;}
#line 604 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 9:
#line 93 "parser.y"
    { yylhs.value.as < void* > () = yystack_[2].value.as < void* > (); }
#line 610 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 10:
#line 96 "parser.y"
    { yylhs.value.as < void* > () = yystack_[0].value.as < void* > (); }
#line 616 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 11:
#line 97 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 622 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 12:
#line 98 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 628 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 13:
#line 99 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 634 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 14:
#line 100 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 640 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 15:
#line 101 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 646 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 16:
#line 102 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 652 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 17:
#line 103 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 658 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 18:
#line 104 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 664 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 19:
#line 105 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 670 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 20:
#line 106 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 676 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 21:
#line 107 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 682 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;

  case 22:
#line 121 "parser.y"
    { yylhs.value.as < void* > () = nullptr; }
#line 688 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"
    break;


#line 692 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"

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


  const signed char parser::yypact_ninf_ = -9;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
      22,     6,    39,    34,    37,    -9,     6,    23,    -9,    35,
      24,    41,    -9,    -9,    -8,    -9,    -9,    24,    24,    24,
      24,    24,    24,    24,    24,    24,    24,    24,    24,    24,
      -9,    -9,     5,     5,     5,     5,     5,     5,     5,     5,
       5,     5,     5
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     0,     0,     3,     0,     1,     0,     0,     4,     5,
       0,     0,    22,     6,     7,    10,     2,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       8,     9,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21
  };

  const signed char
  parser::yypgoto_[] =
  {
      -9,    -9,    38,    -9,    -5,     9,    -9
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     2,     4,    11,    13,    14,    15
  };

  const unsigned char
  parser::yytable_[] =
  {
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    30,    31,    29,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,     1,     3,    29,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     5,
       6,     7,    10,     9,     8,    12,    16
  };

  const unsigned char
  parser::yycheck_[] =
  {
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    17,    18,    22,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,     3,    20,    22,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,     0,
       6,     4,     7,    20,     6,    21,     5
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     3,    24,    20,    25,     0,     6,     4,    25,    20,
       7,    26,    21,    27,    28,    29,     5,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    22,
      27,    27,    28,    28,    28,    28,    28,    28,    28,    28,
      28,    28,    28
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    23,    24,    25,    25,    26,    26,    27,    27,    27,
      28,    28,    28,    28,    28,    28,    28,    28,    28,    28,
      28,    28,    29
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,     6,     1,     3,     0,     2,     1,     3,     3,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "EOF", "error", "$undefined", "SELECT", "FROM", "\";\"", "\",\"",
  "WHERE", "AND", "OR", "\"=\"", "\"!=\"", "\">\"", "\">=\"", "\"<\"",
  "\"<=\"", "\"+\"", "\"-\"", "\"*\"", "\"/\"", "STRING_GROUP",
  "STRING_LITERAL", "\"\"", "$accept", "select_stmt", "column_exp",
  "where_clause", "bool_expr", "expr", "factor", YY_NULLPTR
  };


  const unsigned char
  parser::yyrline_[] =
  {
       0,    72,    72,    79,    80,    84,    85,    91,    92,    93,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   121
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
#line 1005 "/home/rbeatty/Projects/BTDB/src/sql/parser.cpp"

#line 125 "parser.y"


void yy::parser::error(const std::string& m) {
  std::cerr << m << std::endl;
}
