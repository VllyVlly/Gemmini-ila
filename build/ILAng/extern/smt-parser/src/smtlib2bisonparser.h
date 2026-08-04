/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_SMTLIB2_PARSER_MNT_D_VIRELLE_PROJECTS_RA_GEMMINI_ILA_BUILD_ILANG_EXTERN_SMT_PARSER_SRC_SMTLIB2BISONPARSER_H_INCLUDED
# define YY_SMTLIB2_PARSER_MNT_D_VIRELLE_PROJECTS_RA_GEMMINI_ILA_BUILD_ILANG_EXTERN_SMT_PARSER_SRC_SMTLIB2BISONPARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int smtlib2_parser_debug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    BINCONSTANT = 258,             /* BINCONSTANT  */
    HEXCONSTANT = 259,             /* HEXCONSTANT  */
    BVCONSTANT = 260,              /* BVCONSTANT  */
    RATCONSTANT = 261,             /* RATCONSTANT  */
    NUMERAL = 262,                 /* NUMERAL  */
    STRING = 263,                  /* STRING  */
    SYMBOL = 264,                  /* SYMBOL  */
    KEYWORD = 265,                 /* KEYWORD  */
    TK_EOF = 266,                  /* TK_EOF  */
    TK_AS = 267,                   /* "as"  */
    TK_UNDERSCORE = 268,           /* "_"  */
    TK_LET = 269,                  /* "let"  */
    TK_BANG = 270,                 /* "!"  */
    TK_FORALL = 271,               /* "forall"  */
    TK_EXISTS = 272,               /* "exists"  */
    TK_SET_LOGIC = 273,            /* "set-logic"  */
    TK_DECLARE_SORT = 274,         /* "declare-sort"  */
    TK_DEFINE_SORT = 275,          /* "define-sort"  */
    TK_DECLARE_FUN = 276,          /* "declare-fun"  */
    TK_DEFINE_FUN = 277,           /* "define-fun"  */
    TK_PUSH = 278,                 /* "push"  */
    TK_POP = 279,                  /* "pop"  */
    TK_ASSERT = 280,               /* "assert"  */
    TK_CHECK_SAT = 281,            /* "check-sat"  */
    TK_GET_ASSERTIONS = 282,       /* "get-assertions"  */
    TK_GET_UNSAT_CORE = 283,       /* "get-unsat-core"  */
    TK_GET_PROOF = 284,            /* "get-proof"  */
    TK_SET_OPTION = 285,           /* "set-option"  */
    TK_GET_INFO = 286,             /* "get-info"  */
    TK_SET_INFO = 287,             /* "set-info"  */
    TK_GET_ASSIGNMENT = 288,       /* "get-assignment"  */
    TK_GET_MODEL = 289,            /* "get-model"  */
    TK_GET_VALUE = 290,            /* "get-value"  */
    TK_EXIT = 291,                 /* "exit"  */
    TK_INTERNAL_PARSE_TERMS = 292  /* ".internal-parse-terms"  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 80 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"

    char *string;
    smtlib2_vector *termlist;
    smtlib2_sort sort;
    smtlib2_vector *sortlist;
    smtlib2_vector *numlist;
    smtlib2_term term;
    void *identifier;
    smtlib2_charbuf *buf;
    char **attribute;
    smtlib2_vector *attributelist;
    smtlib2_vector *stringlist;
    smtlib2_vector *intlist;

#line 116 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int smtlib2_parser_parse (void * scanner, smtlib2_parser_interface *parser);


#endif /* !YY_SMTLIB2_PARSER_MNT_D_VIRELLE_PROJECTS_RA_GEMMINI_ILA_BUILD_ILANG_EXTERN_SMT_PARSER_SRC_SMTLIB2BISONPARSER_H_INCLUDED  */
