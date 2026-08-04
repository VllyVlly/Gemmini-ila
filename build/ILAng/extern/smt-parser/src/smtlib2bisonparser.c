/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         smtlib2_parser_parse
#define yylex           smtlib2_parser_lex
#define yyerror         smtlib2_parser_error
#define yydebug         smtlib2_parser_debug
#define yynerrs         smtlib2_parser_nerrs

/* First part of user prologue.  */
#line 28 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"

#include "smtparser/smtlib2parserinterface.h"
#include "smtlib2bisonparser.h"

/* This is a flex bug.
"%option nounistd only affects the generated c code
not the header*/
#define YY_NO_UNISTD_H
#include "smtlib2flexlexer.h"
#undef YY_NO_UNISTD_H

#include <limits.h>
#include <assert.h>

#define YYMAXDEPTH LONG_MAX
#define YYLTYPE_IS_TRIVIAL 1

void smtlib2_parser_error(YYLTYPE *yylloc, yyscan_t scanner,
                          smtlib2_parser_interface *parser,
                          const char *s);

/*
 * Stores information about an identifier. Used to handle type annotations and
 * indexed identifiers, without supporting such things in the core solver
 */
typedef struct smtlib2_indexed_identifier {
    char *name;
    smtlib2_vector *idx;
    smtlib2_sort tp;
} smtlib2_indexed_identifier;

smtlib2_indexed_identifier *smtlib2_indexed_identifier_new(
    const char *n, smtlib2_vector *i, smtlib2_sort t);
void smtlib2_indexed_identifier_delete(smtlib2_indexed_identifier *i);

smtlib2_term smtlib2_make_term_from_identifier(
    smtlib2_parser_interface *parser,
    smtlib2_indexed_identifier *ident, smtlib2_vector *args);


#line 117 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
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

#include "smtlib2bisonparser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_BINCONSTANT = 3,                /* BINCONSTANT  */
  YYSYMBOL_HEXCONSTANT = 4,                /* HEXCONSTANT  */
  YYSYMBOL_BVCONSTANT = 5,                 /* BVCONSTANT  */
  YYSYMBOL_RATCONSTANT = 6,                /* RATCONSTANT  */
  YYSYMBOL_NUMERAL = 7,                    /* NUMERAL  */
  YYSYMBOL_STRING = 8,                     /* STRING  */
  YYSYMBOL_SYMBOL = 9,                     /* SYMBOL  */
  YYSYMBOL_KEYWORD = 10,                   /* KEYWORD  */
  YYSYMBOL_TK_EOF = 11,                    /* TK_EOF  */
  YYSYMBOL_TK_AS = 12,                     /* "as"  */
  YYSYMBOL_TK_UNDERSCORE = 13,             /* "_"  */
  YYSYMBOL_TK_LET = 14,                    /* "let"  */
  YYSYMBOL_TK_BANG = 15,                   /* "!"  */
  YYSYMBOL_TK_FORALL = 16,                 /* "forall"  */
  YYSYMBOL_TK_EXISTS = 17,                 /* "exists"  */
  YYSYMBOL_TK_SET_LOGIC = 18,              /* "set-logic"  */
  YYSYMBOL_TK_DECLARE_SORT = 19,           /* "declare-sort"  */
  YYSYMBOL_TK_DEFINE_SORT = 20,            /* "define-sort"  */
  YYSYMBOL_TK_DECLARE_FUN = 21,            /* "declare-fun"  */
  YYSYMBOL_TK_DEFINE_FUN = 22,             /* "define-fun"  */
  YYSYMBOL_TK_PUSH = 23,                   /* "push"  */
  YYSYMBOL_TK_POP = 24,                    /* "pop"  */
  YYSYMBOL_TK_ASSERT = 25,                 /* "assert"  */
  YYSYMBOL_TK_CHECK_SAT = 26,              /* "check-sat"  */
  YYSYMBOL_TK_GET_ASSERTIONS = 27,         /* "get-assertions"  */
  YYSYMBOL_TK_GET_UNSAT_CORE = 28,         /* "get-unsat-core"  */
  YYSYMBOL_TK_GET_PROOF = 29,              /* "get-proof"  */
  YYSYMBOL_TK_SET_OPTION = 30,             /* "set-option"  */
  YYSYMBOL_TK_GET_INFO = 31,               /* "get-info"  */
  YYSYMBOL_TK_SET_INFO = 32,               /* "set-info"  */
  YYSYMBOL_TK_GET_ASSIGNMENT = 33,         /* "get-assignment"  */
  YYSYMBOL_TK_GET_MODEL = 34,              /* "get-model"  */
  YYSYMBOL_TK_GET_VALUE = 35,              /* "get-value"  */
  YYSYMBOL_TK_EXIT = 36,                   /* "exit"  */
  YYSYMBOL_TK_INTERNAL_PARSE_TERMS = 37,   /* ".internal-parse-terms"  */
  YYSYMBOL_38_ = 38,                       /* '('  */
  YYSYMBOL_39_ = 39,                       /* ')'  */
  YYSYMBOL_40_ = 40,                       /* '['  */
  YYSYMBOL_41_ = 41,                       /* ']'  */
  YYSYMBOL_YYACCEPT = 42,                  /* $accept  */
  YYSYMBOL_single_command = 43,            /* single_command  */
  YYSYMBOL_command = 44,                   /* command  */
  YYSYMBOL_cmd_error = 45,                 /* cmd_error  */
  YYSYMBOL_cmd_set_logic = 46,             /* cmd_set_logic  */
  YYSYMBOL_cmd_declare_sort = 47,          /* cmd_declare_sort  */
  YYSYMBOL_cmd_define_sort = 48,           /* cmd_define_sort  */
  YYSYMBOL_cmd_declare_fun = 49,           /* cmd_declare_fun  */
  YYSYMBOL_cmd_define_fun = 50,            /* cmd_define_fun  */
  YYSYMBOL_cmd_push = 51,                  /* cmd_push  */
  YYSYMBOL_cmd_pop = 52,                   /* cmd_pop  */
  YYSYMBOL_cmd_assert = 53,                /* cmd_assert  */
  YYSYMBOL_cmd_check_sat = 54,             /* cmd_check_sat  */
  YYSYMBOL_cmd_get_assertions = 55,        /* cmd_get_assertions  */
  YYSYMBOL_cmd_get_unsat_core = 56,        /* cmd_get_unsat_core  */
  YYSYMBOL_cmd_get_proof = 57,             /* cmd_get_proof  */
  YYSYMBOL_cmd_set_option = 58,            /* cmd_set_option  */
  YYSYMBOL_cmd_get_info = 59,              /* cmd_get_info  */
  YYSYMBOL_cmd_set_info = 60,              /* cmd_set_info  */
  YYSYMBOL_info_argument = 61,             /* info_argument  */
  YYSYMBOL_cmd_get_assignment = 62,        /* cmd_get_assignment  */
  YYSYMBOL_cmd_get_value = 63,             /* cmd_get_value  */
  YYSYMBOL_cmd_exit = 64,                  /* cmd_exit  */
  YYSYMBOL_cmd_internal_parse_terms = 65,  /* cmd_internal_parse_terms  */
  YYSYMBOL_a_term = 66,                    /* a_term  */
  YYSYMBOL_annotated_term = 67,            /* annotated_term  */
  YYSYMBOL_plain_term = 68,                /* plain_term  */
  YYSYMBOL_term_symbol = 69,               /* term_symbol  */
  YYSYMBOL_term_unqualified_symbol = 70,   /* term_unqualified_symbol  */
  YYSYMBOL_term_num_constant = 71,         /* term_num_constant  */
  YYSYMBOL_term_attribute_list = 72,       /* term_attribute_list  */
  YYSYMBOL_term_attribute = 73,            /* term_attribute  */
  YYSYMBOL_attribute_value = 74,           /* attribute_value  */
  YYSYMBOL_attribute_value_list = 75,      /* attribute_value_list  */
  YYSYMBOL_num_list = 76,                  /* num_list  */
  YYSYMBOL_int_list = 77,                  /* int_list  */
  YYSYMBOL_term_list = 78,                 /* term_list  */
  YYSYMBOL_quant_var_list = 79,            /* quant_var_list  */
  YYSYMBOL_begin_let_scope = 80,           /* begin_let_scope  */
  YYSYMBOL_let_bindings = 81,              /* let_bindings  */
  YYSYMBOL_let_binding = 82,               /* let_binding  */
  YYSYMBOL_logic_name = 83,                /* logic_name  */
  YYSYMBOL_sort_list = 84,                 /* sort_list  */
  YYSYMBOL_a_sort = 85,                    /* a_sort  */
  YYSYMBOL_sort_param_list = 86,           /* sort_param_list  */
  YYSYMBOL_a_sort_param = 87,              /* a_sort_param  */
  YYSYMBOL_verbatim_term_list = 88,        /* verbatim_term_list  */
  YYSYMBOL_verbatim_term = 89              /* verbatim_term  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  43
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   234

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  105
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  221

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   292


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      38,    39,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    40,     2,    41,     2,     2,     2,     2,     2,     2,
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
      35,    36,    37
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   178,   178,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   208,   214,   222,   233,   238,   249,   255,
     268,   273,   283,   292,   302,   309,   316,   323,   330,   338,
     345,   352,   368,   377,   386,   396,   400,   404,   408,   415,
     422,   435,   442,   452,   456,   464,   481,   488,   498,   508,
     512,   518,   529,   531,   540,   545,   555,   560,   565,   571,
     577,   588,   593,   602,   612,   616,   620,   624,   659,   664,
     673,   680,   691,   699,   710,   715,   724,   734,   746,   754,
     755,   759,   768,   770,   781,   786,   795,   800,   806,   816,
     822,   830,   840,   845,   854,   858
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "BINCONSTANT",
  "HEXCONSTANT", "BVCONSTANT", "RATCONSTANT", "NUMERAL", "STRING",
  "SYMBOL", "KEYWORD", "TK_EOF", "\"as\"", "\"_\"", "\"let\"", "\"!\"",
  "\"forall\"", "\"exists\"", "\"set-logic\"", "\"declare-sort\"",
  "\"define-sort\"", "\"declare-fun\"", "\"define-fun\"", "\"push\"",
  "\"pop\"", "\"assert\"", "\"check-sat\"", "\"get-assertions\"",
  "\"get-unsat-core\"", "\"get-proof\"", "\"set-option\"", "\"get-info\"",
  "\"set-info\"", "\"get-assignment\"", "\"get-model\"", "\"get-value\"",
  "\"exit\"", "\".internal-parse-terms\"", "'('", "')'", "'['", "']'",
  "$accept", "single_command", "command", "cmd_error", "cmd_set_logic",
  "cmd_declare_sort", "cmd_define_sort", "cmd_declare_fun",
  "cmd_define_fun", "cmd_push", "cmd_pop", "cmd_assert", "cmd_check_sat",
  "cmd_get_assertions", "cmd_get_unsat_core", "cmd_get_proof",
  "cmd_set_option", "cmd_get_info", "cmd_set_info", "info_argument",
  "cmd_get_assignment", "cmd_get_value", "cmd_exit",
  "cmd_internal_parse_terms", "a_term", "annotated_term", "plain_term",
  "term_symbol", "term_unqualified_symbol", "term_num_constant",
  "term_attribute_list", "term_attribute", "attribute_value",
  "attribute_value_list", "num_list", "int_list", "term_list",
  "quant_var_list", "begin_let_scope", "let_bindings", "let_binding",
  "logic_name", "sort_list", "a_sort", "sort_param_list", "a_sort_param",
  "verbatim_term_list", "verbatim_term", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-115)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -30,   150,    23,  -115,  -115,  -115,  -115,  -115,  -115,  -115,
    -115,  -115,  -115,  -115,  -115,  -115,  -115,  -115,  -115,  -115,
    -115,  -115,  -115,  -115,     2,    17,    49,    55,    70,    74,
     108,    25,    62,    86,   100,   101,   131,   132,   133,   105,
     109,   110,   112,  -115,   106,   113,   141,   115,   117,   119,
     120,   121,  -115,  -115,  -115,  -115,  -115,   107,   122,  -115,
    -115,  -115,  -115,  -115,  -115,  -115,  -115,  -115,    83,   123,
     102,  -115,    88,  -115,    25,   144,  -115,   124,     3,     0,
      75,  -115,  -115,    -5,    56,  -115,    25,   146,   151,   116,
      25,   152,  -115,   149,   153,   154,   155,  -115,  -115,  -115,
    -115,  -115,   156,  -115,  -115,    45,  -115,  -115,    61,  -115,
    -115,    11,   157,  -115,  -115,     7,    21,  -115,  -115,    69,
       7,    35,  -115,   145,     7,    92,   178,     7,   189,   190,
     148,   161,   161,   191,    18,   163,  -115,  -115,  -115,  -115,
    -115,  -115,  -115,    79,   164,  -115,   165,  -115,  -115,   166,
       7,  -115,     7,   193,   167,     7,  -115,     7,    25,   198,
       7,   169,   170,  -115,    -4,    98,     9,  -115,    94,    96,
    -115,   201,   172,   163,  -115,  -115,  -115,  -115,  -115,   173,
      38,   206,  -115,   175,   176,   177,     7,    25,  -115,  -115,
    -115,  -115,  -115,  -115,  -115,    25,    25,    25,    25,  -115,
    -115,  -115,  -115,    -2,  -115,  -115,  -115,   179,   180,   181,
     182,   183,   184,  -115,  -115,  -115,  -115,  -115,  -115,  -115,
    -115
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      23,     0,     0,     2,    22,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     1,    92,     0,     0,     0,     0,     0,
       0,     0,    68,    69,    67,    66,    64,     0,     0,    53,
      54,    60,    62,    59,    35,    36,    37,    38,     0,     0,
       0,    49,     0,    51,     0,     0,    24,     0,     0,     0,
       0,    32,    33,     0,     0,    88,     0,     0,     0,     0,
       0,     0,    34,     0,     0,     0,     0,    43,    46,    45,
      48,    47,     0,   105,    75,     0,    74,   104,     0,   102,
      84,     0,     0,    25,   101,     0,     0,    99,    96,     0,
       0,     0,    94,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,    39,    42,    41,
      44,    76,    78,     0,     0,   103,     0,    85,    93,     0,
       0,   100,     0,     0,     0,     0,    95,     0,     0,     0,
       0,     0,     0,    80,     0,     0,     0,    71,     0,     0,
      61,     0,     0,    89,    77,    79,    50,    52,    26,     0,
       0,     0,    28,     0,     0,     0,     0,     0,    63,    70,
      81,    65,    73,    55,    72,     0,     0,     0,     0,    90,
      27,    98,    82,     0,    29,    86,    30,     0,     0,     0,
       0,     0,     0,    83,    97,    87,    31,    57,    58,    91,
      56
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -115,  -115,  -115,  -115,  -115,  -115,  -115,  -115,  -115,  -115,
    -115,  -115,  -115,  -115,  -115,  -115,  -115,  -115,  -115,   147,
    -115,  -115,  -115,  -115,   -31,  -115,  -115,   168,   143,  -115,
    -115,    58,  -103,  -115,  -115,  -115,   137,     6,  -115,    57,
    -115,  -115,    76,  -114,  -115,   118,  -115,   125
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,   106,
      20,    21,    22,    23,   110,    59,    60,    61,    62,    63,
     166,   167,   107,   143,   164,   203,   111,   125,    91,   172,
     173,    45,   121,   122,   116,   117,   108,   109
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      58,   149,   142,   190,    56,   213,   154,   156,     1,   118,
     158,    44,   114,   161,    52,    53,   118,    54,    55,   165,
      56,    52,    53,    43,    54,    55,    46,    56,    52,    53,
     114,    54,    55,   126,    56,   191,   179,   214,   119,   120,
     175,   183,   115,   184,   118,   119,   187,   118,   193,    57,
     146,    98,    99,   100,   101,   130,    57,   170,    47,   104,
     150,   128,   192,    57,    48,   129,   156,    98,    99,   100,
     101,   103,   207,   119,   155,   104,   119,   201,   152,    49,
     147,    50,   153,   105,   141,    98,    99,   100,   101,    93,
      94,    95,    96,   104,    98,    99,   100,   101,   103,   105,
     144,    64,   104,   147,    98,    99,   100,   101,    98,    99,
     100,   101,   104,   123,   124,    51,    56,   105,   174,    83,
      84,    85,    86,    87,    88,    65,   105,   185,    83,   133,
     159,   160,   159,   195,   159,   196,   105,   168,   169,    66,
      67,    68,    69,    70,    71,    89,    75,    72,    77,    73,
      74,   112,    76,    78,   157,    79,   208,    80,   165,    81,
      82,    92,    97,   113,   209,   210,   211,   212,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,   131,    40,    41,    42,   136,   132,
     135,   133,   137,   138,   139,   140,   162,   163,   148,   123,
     129,   171,   181,   176,   177,   178,   182,   186,   188,   189,
     197,   198,   200,   202,   204,   205,   206,   102,   215,   216,
     217,   218,   219,   220,   194,    90,   127,   134,   180,     0,
     199,     0,     0,   145,   151
};

static const yytype_int16 yycheck[] =
{
      31,   115,   105,     7,     9,     7,   120,   121,    38,     9,
     124,     9,     9,   127,     3,     4,     9,     6,     7,    10,
       9,     3,     4,     0,     6,     7,     9,     9,     3,     4,
       9,     6,     7,    38,     9,    39,   150,    39,    38,    39,
     143,   155,    39,   157,     9,    38,   160,     9,    39,    38,
      39,     6,     7,     8,     9,    86,    38,    39,     9,    14,
      39,     5,   165,    38,     9,     9,   180,     6,     7,     8,
       9,    10,   186,    38,    39,    14,    38,    39,     9,     9,
     111,     7,    13,    38,    39,     6,     7,     8,     9,     6,
       7,     8,     9,    14,     6,     7,     8,     9,    10,    38,
      39,    39,    14,   134,     6,     7,     8,     9,     6,     7,
       8,     9,    14,    38,    39,     7,     9,    38,    39,    12,
      13,    14,    15,    16,    17,    39,    38,   158,    12,    13,
      38,    39,    38,    39,    38,    39,    38,   131,   132,    39,
      39,    10,    10,    10,    39,    38,    40,    38,     7,    39,
      38,     7,    39,    38,     9,    38,   187,    38,    10,    39,
      39,    39,    39,    39,   195,   196,   197,   198,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    38,    35,    36,    37,    39,    38,
      38,    13,    39,    39,    39,    39,     7,     7,    41,    38,
       9,    38,     9,    39,    39,    39,    39,     9,    39,    39,
       9,    39,    39,     7,    39,    39,    39,    70,    39,    39,
      39,    39,    39,    39,   166,    57,    83,    90,   152,    -1,
     173,    -1,    -1,   108,   116
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    38,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      62,    63,    64,    65,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      35,    36,    37,     0,     9,    83,     9,     9,     9,     9,
       7,     7,     3,     4,     6,     7,     9,    38,    66,    67,
      68,    69,    70,    71,    39,    39,    39,    39,    10,    10,
      10,    39,    38,    39,    38,    40,    39,     7,    38,    38,
      38,    39,    39,    12,    13,    14,    15,    16,    17,    38,
      69,    80,    39,     6,     7,     8,     9,    39,     6,     7,
       8,     9,    61,    10,    14,    38,    61,    74,    88,    89,
      66,    78,     7,    39,     9,    39,    86,    87,     9,    38,
      39,    84,    85,    38,    39,    79,    38,    70,     5,     9,
      66,    38,    38,    13,    78,    38,    39,    39,    39,    39,
      39,    39,    74,    75,    39,    89,    39,    66,    41,    85,
      39,    87,     9,    13,    85,    39,    85,     9,    85,    38,
      39,    85,     7,     7,    76,    10,    72,    73,    79,    79,
      39,    38,    81,    82,    39,    74,    39,    39,    39,    85,
      84,     9,    39,    85,    85,    66,     9,    85,    39,    39,
       7,    39,    74,    39,    73,    39,    39,     9,    39,    81,
      39,    39,     7,    77,    39,    39,    39,    85,    66,    66,
      66,    66,    66,     7,    39,    39,    39,    39,    39,    39,
      39
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    42,    43,    44,    44,    44,    44,    44,    44,    44,
      44,    44,    44,    44,    44,    44,    44,    44,    44,    44,
      44,    44,    44,    45,    46,    47,    48,    48,    49,    49,
      50,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      58,    58,    58,    59,    60,    61,    61,    61,    61,    62,
      63,    64,    65,    66,    66,    67,    68,    68,    68,    68,
      68,    68,    69,    69,    70,    70,    71,    71,    71,    71,
      71,    72,    72,    73,    74,    74,    74,    74,    75,    75,
      76,    76,    77,    77,    78,    78,    79,    79,    80,    81,
      81,    82,    83,    83,    84,    84,    85,    85,    85,    86,
      86,    87,    88,    88,    89,    89
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     4,     5,     7,     8,     7,     8,
       8,     9,     4,     4,     4,     3,     3,     3,     3,     5,
       5,     5,     5,     4,     5,     1,     1,     1,     1,     3,
       6,     3,     6,     1,     1,     5,     7,     7,     7,     1,
       1,     4,     1,     5,     1,     5,     1,     1,     1,     1,
       5,     1,     2,     2,     1,     1,     2,     3,     1,     2,
       1,     2,     1,     2,     1,     2,     4,     5,     1,     1,
       2,     4,     1,     4,     1,     2,     1,     5,     4,     1,
       2,     1,     1,     2,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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
        yyerror (&yylloc, scanner, parser, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


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


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, scanner, parser); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void * scanner, smtlib2_parser_interface *parser)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (scanner);
  YY_USE (parser);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void * scanner, smtlib2_parser_interface *parser)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, scanner, parser);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, void * scanner, smtlib2_parser_interface *parser)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), scanner, parser);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, scanner, parser); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void * scanner, smtlib2_parser_interface *parser)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (scanner);
  YY_USE (parser);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_BINCONSTANT: /* BINCONSTANT  */
#line 162 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { free(((*yyvaluep).string)); }
#line 1188 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_HEXCONSTANT: /* HEXCONSTANT  */
#line 162 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { free(((*yyvaluep).string)); }
#line 1194 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_RATCONSTANT: /* RATCONSTANT  */
#line 162 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { free(((*yyvaluep).string)); }
#line 1200 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_NUMERAL: /* NUMERAL  */
#line 162 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { free(((*yyvaluep).string)); }
#line 1206 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 162 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { free(((*yyvaluep).string)); }
#line 1212 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_SYMBOL: /* SYMBOL  */
#line 162 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { free(((*yyvaluep).string)); }
#line 1218 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_KEYWORD: /* KEYWORD  */
#line 162 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { free(((*yyvaluep).string)); }
#line 1224 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_term_symbol: /* term_symbol  */
#line 169 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { smtlib2_indexed_identifier_delete((smtlib2_indexed_identifier *)((*yyvaluep).identifier)); }
#line 1230 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_term_unqualified_symbol: /* term_unqualified_symbol  */
#line 170 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { smtlib2_indexed_identifier_delete((smtlib2_indexed_identifier *)((*yyvaluep).identifier)); }
#line 1236 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_term_attribute_list: /* term_attribute_list  */
#line 171 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { smtlib2_vector_delete(((*yyvaluep).attributelist)); }
#line 1242 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_term_attribute: /* term_attribute  */
#line 172 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { free(((*yyvaluep).attribute)[0]); free(((*yyvaluep).attribute)[1]); free(((*yyvaluep).attribute)); }
#line 1248 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_num_list: /* num_list  */
#line 168 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { smtlib2_vector_delete(((*yyvaluep).numlist)); }
#line 1254 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_int_list: /* int_list  */
#line 168 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { smtlib2_vector_delete(((*yyvaluep).intlist)); }
#line 1260 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_term_list: /* term_list  */
#line 164 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { smtlib2_vector_delete(((*yyvaluep).termlist)); }
#line 1266 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_quant_var_list: /* quant_var_list  */
#line 167 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { smtlib2_vector_delete(((*yyvaluep).termlist)); }
#line 1272 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_logic_name: /* logic_name  */
#line 162 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { free(((*yyvaluep).string)); }
#line 1278 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_sort_list: /* sort_list  */
#line 165 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { smtlib2_vector_delete(((*yyvaluep).sortlist)); }
#line 1284 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

    case YYSYMBOL_sort_param_list: /* sort_param_list  */
#line 166 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
            { smtlib2_vector_delete(((*yyvaluep).sortlist)); }
#line 1290 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void * scanner, smtlib2_parser_interface *parser)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* single_command: command  */
#line 179 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      YYACCEPT;
  }
#line 1595 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 23: /* cmd_error: %empty  */
#line 208 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      YYERROR;
  }
#line 1603 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 24: /* cmd_set_logic: '(' "set-logic" logic_name ')'  */
#line 215 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->set_logic(parser, (yyvsp[-1].string));
      free((yyvsp[-1].string));
  }
#line 1612 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 25: /* cmd_declare_sort: '(' "declare-sort" SYMBOL NUMERAL ')'  */
#line 223 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      int n = atoi((yyvsp[-1].string));
      parser->declare_sort(parser, (yyvsp[-2].string), n);
      free((yyvsp[-1].string));
      free((yyvsp[-2].string));
  }
#line 1623 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 26: /* cmd_define_sort: '(' "define-sort" SYMBOL '(' ')' a_sort ')'  */
#line 234 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->define_sort(parser, (yyvsp[-4].string), NULL, (yyvsp[-1].sort));
      free((yyvsp[-4].string));
  }
#line 1632 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 27: /* cmd_define_sort: '(' "define-sort" SYMBOL '(' sort_param_list ')' a_sort ')'  */
#line 239 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->define_sort(parser, (yyvsp[-5].string), (yyvsp[-3].sortlist), (yyvsp[-1].sort));
      parser->pop_sort_param_scope(parser);
      smtlib2_vector_delete((yyvsp[-3].sortlist));
      free((yyvsp[-5].string));
  }
#line 1643 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 28: /* cmd_declare_fun: '(' "declare-fun" SYMBOL '(' ')' a_sort ')'  */
#line 250 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      smtlib2_sort tp = (yyvsp[-1].sort);
      parser->declare_function(parser, (yyvsp[-4].string), tp);
      free((yyvsp[-4].string));
  }
#line 1653 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 29: /* cmd_declare_fun: '(' "declare-fun" SYMBOL '(' sort_list ')' a_sort ')'  */
#line 256 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      smtlib2_sort tp = (yyvsp[-1].sort);
      assert(smtlib2_vector_size((yyvsp[-3].sortlist)) > 0);
      smtlib2_vector_push((yyvsp[-3].sortlist), (intptr_t)tp);
      tp = parser->make_function_sort(parser, (yyvsp[-3].sortlist));
      parser->declare_function(parser, (yyvsp[-5].string), tp);
      free((yyvsp[-5].string));
      smtlib2_vector_delete((yyvsp[-3].sortlist));
  }
#line 1667 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 30: /* cmd_define_fun: '(' "define-fun" SYMBOL '(' ')' a_sort a_term ')'  */
#line 269 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->define_function(parser, (yyvsp[-5].string), NULL, (yyvsp[-2].sort), (yyvsp[-1].term));
      free((yyvsp[-5].string));
  }
#line 1676 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 31: /* cmd_define_fun: '(' "define-fun" SYMBOL '(' quant_var_list ')' a_sort a_term ')'  */
#line 274 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->define_function(parser, (yyvsp[-6].string), (yyvsp[-4].termlist), (yyvsp[-2].sort), (yyvsp[-1].term));
      parser->pop_quantifier_scope(parser);
      free((yyvsp[-6].string));
      smtlib2_vector_delete((yyvsp[-4].termlist));
  }
#line 1687 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 32: /* cmd_push: '(' "push" NUMERAL ')'  */
#line 284 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      int n = atoi((yyvsp[-1].string));
      free((yyvsp[-1].string));
      parser->push(parser, n);
  }
#line 1697 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 33: /* cmd_pop: '(' "pop" NUMERAL ')'  */
#line 293 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      int n = atoi((yyvsp[-1].string));
      free((yyvsp[-1].string));
      parser->pop(parser, n);
  }
#line 1707 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 34: /* cmd_assert: '(' "assert" a_term ')'  */
#line 303 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->assert_formula(parser, (yyvsp[-1].term));
  }
#line 1715 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 35: /* cmd_check_sat: '(' "check-sat" ')'  */
#line 310 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->check_sat(parser);
  }
#line 1723 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 36: /* cmd_get_assertions: '(' "get-assertions" ')'  */
#line 317 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->get_assertions(parser);
  }
#line 1731 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 37: /* cmd_get_unsat_core: '(' "get-unsat-core" ')'  */
#line 324 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->get_unsat_core(parser);
  }
#line 1739 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 38: /* cmd_get_proof: '(' "get-proof" ')'  */
#line 331 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->get_proof(parser);
  }
#line 1747 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 39: /* cmd_set_option: '(' "set-option" KEYWORD NUMERAL ')'  */
#line 339 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      int n = atoi((yyvsp[-1].string));
      parser->set_int_option(parser, (yyvsp[-2].string), n);
      free((yyvsp[-1].string));
      free((yyvsp[-2].string));
  }
#line 1758 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 40: /* cmd_set_option: '(' "set-option" KEYWORD RATCONSTANT ')'  */
#line 346 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      double n = atof((yyvsp[-1].string));
      parser->set_rat_option(parser, (yyvsp[-2].string), n);
      free((yyvsp[-1].string));
      free((yyvsp[-2].string));
  }
#line 1769 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 41: /* cmd_set_option: '(' "set-option" KEYWORD SYMBOL ')'  */
#line 353 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      if (strcmp((yyvsp[-1].string), "true") == 0) {
          parser->set_int_option(parser, (yyvsp[-2].string), 1);
      } else if (strcmp((yyvsp[-1].string), "false") == 0) {
          parser->set_int_option(parser, (yyvsp[-2].string), 0);
      } else if (strcmp((yyvsp[-1].string), "none") == 0) {
          parser->set_rat_option(parser, (yyvsp[-2].string), 0);
      } else {
          free((yyvsp[-1].string));
          free((yyvsp[-2].string));
          YYERROR;
      }
      free((yyvsp[-1].string));
      free((yyvsp[-2].string));
  }
#line 1789 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 42: /* cmd_set_option: '(' "set-option" KEYWORD STRING ')'  */
#line 369 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->set_str_option(parser, (yyvsp[-2].string), (yyvsp[-1].string));
      free((yyvsp[-1].string));
      free((yyvsp[-2].string));
  }
#line 1799 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 43: /* cmd_get_info: '(' "get-info" KEYWORD ')'  */
#line 378 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->get_info(parser, (yyvsp[-1].string));
      free((yyvsp[-1].string));
  }
#line 1808 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 44: /* cmd_set_info: '(' "set-info" KEYWORD info_argument ')'  */
#line 387 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->set_info(parser, (yyvsp[-2].string), (yyvsp[-1].string));
      free((yyvsp[-1].string));
      free((yyvsp[-2].string));
  }
#line 1818 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 45: /* info_argument: NUMERAL  */
#line 397 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = (yyvsp[0].string);
  }
#line 1826 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 46: /* info_argument: RATCONSTANT  */
#line 401 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = (yyvsp[0].string);
  }
#line 1834 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 47: /* info_argument: SYMBOL  */
#line 405 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = (yyvsp[0].string);
  }
#line 1842 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 48: /* info_argument: STRING  */
#line 409 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = (yyvsp[0].string);
  }
#line 1850 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 49: /* cmd_get_assignment: '(' "get-assignment" ')'  */
#line 416 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->get_assignment(parser);
  }
#line 1858 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 50: /* cmd_get_value: '(' "get-value" '(' verbatim_term_list ')' ')'  */
#line 423 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      size_t i;
      parser->get_value(parser, (yyvsp[-2].stringlist));
      for (i = 0; i < smtlib2_vector_size((yyvsp[-2].stringlist)); ++i) {
          char *s = (char *)smtlib2_vector_at((yyvsp[-2].stringlist), i);
          free(s);
      }
      smtlib2_vector_delete((yyvsp[-2].stringlist));
  }
#line 1872 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 51: /* cmd_exit: '(' "exit" ')'  */
#line 436 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->exit(parser);
  }
#line 1880 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 52: /* cmd_internal_parse_terms: '(' ".internal-parse-terms" '(' term_list ')' ')'  */
#line 443 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->set_internal_parsed_terms(parser, (yyvsp[-2].termlist));
      smtlib2_vector_delete((yyvsp[-2].termlist));
  }
#line 1889 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 53: /* a_term: annotated_term  */
#line 453 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.term) = (yyvsp[0].term);
  }
#line 1897 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 54: /* a_term: plain_term  */
#line 457 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.term) = (yyvsp[0].term);
  }
#line 1905 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 55: /* annotated_term: '(' "!" a_term term_attribute_list ')'  */
#line 465 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      size_t i;
      (yyval.term) = (yyvsp[-2].term);
      parser->annotate_term(parser, (yyval.term), (yyvsp[-1].attributelist));
      for (i = 0; i < smtlib2_vector_size((yyvsp[-1].attributelist)); ++i) {
          char **pair = (char **)smtlib2_vector_at((yyvsp[-1].attributelist), i);
          free(pair[0]);
          free(pair[1]);
          free(pair);
      }
      smtlib2_vector_delete((yyvsp[-1].attributelist));
  }
#line 1922 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 56: /* plain_term: '(' begin_let_scope '(' let_bindings ')' a_term ')'  */
#line 482 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.term) = parser->pop_let_scope(parser);
      if (! (yyval.term)) {
          (yyval.term) = (yyvsp[-1].term);
      }
  }
#line 1933 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 57: /* plain_term: '(' "forall" '(' quant_var_list ')' a_term ')'  */
#line 489 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      smtlib2_term tmp;
      (yyval.term) = parser->make_forall_term(parser, (yyvsp[-1].term));
      tmp = parser->pop_quantifier_scope(parser);
      if (tmp) {
          (yyval.term) = tmp;
      }
      smtlib2_vector_delete((yyvsp[-3].termlist));
  }
#line 1947 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 58: /* plain_term: '(' "exists" '(' quant_var_list ')' a_term ')'  */
#line 499 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      smtlib2_term tmp;
      (yyval.term) = parser->make_exists_term(parser, (yyvsp[-1].term));
      tmp = parser->pop_quantifier_scope(parser);
      if (tmp) {
          (yyval.term) = tmp;
      }
      smtlib2_vector_delete((yyvsp[-3].termlist));
  }
#line 1961 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 59: /* plain_term: term_num_constant  */
#line 509 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.term) = (yyvsp[0].term);
  }
#line 1969 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 60: /* plain_term: term_symbol  */
#line 513 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      smtlib2_indexed_identifier *id = (smtlib2_indexed_identifier *)(yyvsp[0].identifier);
      (yyval.term) = smtlib2_make_term_from_identifier(parser, id, NULL);
      smtlib2_indexed_identifier_delete(id);
  }
#line 1979 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 61: /* plain_term: '(' term_symbol term_list ')'  */
#line 519 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      smtlib2_indexed_identifier *id = (smtlib2_indexed_identifier *)(yyvsp[-2].identifier);
      (yyval.term) = smtlib2_make_term_from_identifier(parser, id, (yyvsp[-1].termlist));
      smtlib2_indexed_identifier_delete(id);
      smtlib2_vector_delete((yyvsp[-1].termlist));
  }
#line 1990 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 62: /* term_symbol: term_unqualified_symbol  */
#line 530 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  { (yyval.identifier) = (yyvsp[0].identifier); }
#line 1996 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 63: /* term_symbol: '(' "as" term_unqualified_symbol a_sort ')'  */
#line 532 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.identifier) = (yyvsp[-2].identifier);
      ((smtlib2_indexed_identifier *)(yyval.identifier))->tp = (yyvsp[-1].sort);
  }
#line 2005 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 64: /* term_unqualified_symbol: SYMBOL  */
#line 541 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.identifier) = smtlib2_indexed_identifier_new((yyvsp[0].string), NULL, NULL);
      free((yyvsp[0].string));
  }
#line 2014 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 65: /* term_unqualified_symbol: '(' "_" SYMBOL num_list ')'  */
#line 546 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.identifier) = smtlib2_indexed_identifier_new((yyvsp[-2].string), (yyvsp[-1].numlist), NULL);
      free((yyvsp[-2].string));
      /* $$ takes ownership of $4, so we don't delete it here */
  }
#line 2024 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 66: /* term_num_constant: NUMERAL  */
#line 556 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.term) = parser->make_number_term(parser, (yyvsp[0].string), 0, 10);
      free((yyvsp[0].string));
  }
#line 2033 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 67: /* term_num_constant: RATCONSTANT  */
#line 561 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.term) = parser->make_number_term(parser, (yyvsp[0].string), 0, 10);
      free((yyvsp[0].string));
  }
#line 2042 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 68: /* term_num_constant: BINCONSTANT  */
#line 566 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      const char *s = (yyvsp[0].string) + 2; /* skip the "#b" prefix */
      (yyval.term) = parser->make_number_term(parser, s, strlen(s), 2);
      free((yyvsp[0].string));
  }
#line 2052 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 69: /* term_num_constant: HEXCONSTANT  */
#line 572 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      const char *s = (yyvsp[0].string) + 2; /* skip the "#x" prefix */
      (yyval.term) = parser->make_number_term(parser, s, 4 * strlen(s), 16);
      free((yyvsp[0].string));
  }
#line 2062 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 70: /* term_num_constant: '(' "_" BVCONSTANT NUMERAL ')'  */
#line 578 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      const char *s = (yyvsp[-2].string) + 2; /* skip the "bv" prefix */
      (yyval.term) = parser->make_number_term(parser, s, atoi((yyvsp[-1].string)), 10);
      free((yyvsp[-1].string));
      free((yyvsp[-2].string));
  }
#line 2073 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 71: /* term_attribute_list: term_attribute  */
#line 589 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.attributelist) = smtlib2_vector_new();
      smtlib2_vector_push((yyval.attributelist), (intptr_t)(yyvsp[0].attribute));
  }
#line 2082 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 72: /* term_attribute_list: term_attribute_list term_attribute  */
#line 594 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.attributelist) = (yyvsp[-1].attributelist);
      smtlib2_vector_push((yyval.attributelist), (intptr_t)(yyvsp[0].attribute));
  }
#line 2091 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 73: /* term_attribute: KEYWORD attribute_value  */
#line 603 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.attribute) = (char **)malloc(sizeof(char *) * 2);
      (yyval.attribute)[0] = (yyvsp[-1].string);
      (yyval.attribute)[1] = (yyvsp[0].string);
  }
#line 2101 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 74: /* attribute_value: info_argument  */
#line 613 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = (yyvsp[0].string);
  }
#line 2109 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 75: /* attribute_value: "let"  */
#line 617 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = smtlib2_strdup("let");
  }
#line 2117 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 76: /* attribute_value: '(' ')'  */
#line 621 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = smtlib2_strdup("()");
  }
#line 2125 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 77: /* attribute_value: '(' attribute_value_list ')'  */
#line 625 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      size_t howmany = 0;
      size_t i;
      char *s;
      
      for (i = 0; i < smtlib2_vector_size((yyvsp[-1].stringlist)); ++i) {
          howmany += strlen((char *)smtlib2_vector_at((yyvsp[-1].stringlist), i));
      }
      howmany += 2 /* '(' and ')' */ +
          (smtlib2_vector_size((yyvsp[-1].stringlist))-1) /* ' 's */ + 1; /* '\0' */
      (yyval.string) = (char *)malloc(sizeof(char) * howmany);

      /* concatenate everything together */
      s = (yyval.string);
      s[0] = '(';
      ++s;
      for (i = 0; i < smtlib2_vector_size((yyvsp[-1].stringlist)); ++i) {
          char *s2 = (char *)smtlib2_vector_at((yyvsp[-1].stringlist), i);
          char *s3 = s2;
          while (*s2) {
              *s++ = *s2++;
          }
          *s++ = ' ';
          free(s3);
      }
      *(s-1) = ')';
      *s = '\0';
      
      smtlib2_vector_delete((yyvsp[-1].stringlist));
  }
#line 2160 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 78: /* attribute_value_list: attribute_value  */
#line 660 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.stringlist) = smtlib2_vector_new();
      smtlib2_vector_push((yyval.stringlist), (intptr_t)(yyvsp[0].string));
  }
#line 2169 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 79: /* attribute_value_list: attribute_value_list attribute_value  */
#line 665 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.stringlist) = (yyvsp[-1].stringlist);
      smtlib2_vector_push((yyval.stringlist), (intptr_t)(yyvsp[0].string));
  }
#line 2178 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 80: /* num_list: NUMERAL  */
#line 674 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.numlist) = smtlib2_vector_new();
      int n = atoi((yyvsp[0].string));
      smtlib2_vector_push((yyval.numlist), n);
      free((yyvsp[0].string));
  }
#line 2189 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 81: /* num_list: num_list NUMERAL  */
#line 681 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      int n = atoi((yyvsp[0].string));
      smtlib2_vector_push((yyvsp[-1].numlist), n);
      (yyval.numlist) = (yyvsp[-1].numlist);
      free((yyvsp[0].string));
  }
#line 2200 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 82: /* int_list: NUMERAL  */
#line 692 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      int n;
      (yyval.intlist) = smtlib2_vector_new();
      n = atoi((yyvsp[0].string));
      smtlib2_vector_push((yyval.intlist), n);
      free((yyvsp[0].string));
  }
#line 2212 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 83: /* int_list: int_list NUMERAL  */
#line 700 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      int n = atoi((yyvsp[0].string));
      smtlib2_vector_push((yyvsp[-1].intlist), n);
      (yyval.intlist) = (yyvsp[-1].intlist);
      free((yyvsp[0].string));
  }
#line 2223 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 84: /* term_list: a_term  */
#line 711 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.termlist) = smtlib2_vector_new();
      smtlib2_vector_push((yyval.termlist), (intptr_t)(yyvsp[0].term));
  }
#line 2232 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 85: /* term_list: term_list a_term  */
#line 716 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      smtlib2_vector_push((yyvsp[-1].termlist), (intptr_t)(yyvsp[0].term));
      (yyval.termlist) = (yyvsp[-1].termlist);
  }
#line 2241 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 86: /* quant_var_list: '(' SYMBOL a_sort ')'  */
#line 725 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      intptr_t t;
      parser->push_quantifier_scope(parser);
      (yyval.termlist) = smtlib2_vector_new();
      parser->declare_variable(parser, (yyvsp[-2].string), (yyvsp[-1].sort));
      t = (intptr_t)parser->make_term(parser, (yyvsp[-2].string), (yyvsp[-1].sort), NULL, NULL);
      smtlib2_vector_push((yyval.termlist), t);
      free((yyvsp[-2].string));
  }
#line 2255 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 87: /* quant_var_list: quant_var_list '(' SYMBOL a_sort ')'  */
#line 735 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      intptr_t t;
      parser->declare_variable(parser, (yyvsp[-2].string), (yyvsp[-1].sort));
      t = (intptr_t)parser->make_term(parser, (yyvsp[-2].string), (yyvsp[-1].sort), NULL, NULL);
      smtlib2_vector_push((yyvsp[-4].termlist), t);
      free((yyvsp[-2].string));
      (yyval.termlist) = (yyvsp[-4].termlist);
  }
#line 2268 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 88: /* begin_let_scope: "let"  */
#line 747 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->push_let_scope(parser);
  }
#line 2276 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 89: /* let_bindings: let_binding  */
#line 754 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
              {}
#line 2282 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 90: /* let_bindings: let_binding let_bindings  */
#line 755 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
                           {}
#line 2288 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 91: /* let_binding: '(' SYMBOL a_term ')'  */
#line 760 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->define_let_binding(parser, (yyvsp[-2].string), (yyvsp[-1].term));
      free((yyvsp[-2].string));
  }
#line 2297 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 92: /* logic_name: SYMBOL  */
#line 769 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  { (yyval.string) = (yyvsp[0].string); }
#line 2303 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 93: /* logic_name: SYMBOL '[' NUMERAL ']'  */
#line 771 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = (char *)(malloc(strlen((yyvsp[-3].string)) + strlen((yyvsp[-1].string)) + 2 + 1));
      sprintf((yyval.string), "%s[%s]", (yyvsp[-3].string), (yyvsp[-1].string));
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
  }
#line 2314 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 94: /* sort_list: a_sort  */
#line 782 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.sortlist) = smtlib2_vector_new();
      smtlib2_vector_push((yyval.sortlist), (intptr_t)(yyvsp[0].sort));
  }
#line 2323 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 95: /* sort_list: sort_list a_sort  */
#line 787 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.sortlist) = (yyvsp[-1].sortlist);
      smtlib2_vector_push((yyval.sortlist), (intptr_t)(yyvsp[0].sort));
  }
#line 2332 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 96: /* a_sort: SYMBOL  */
#line 796 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.sort) = parser->make_sort(parser, (yyvsp[0].string), NULL);
      free((yyvsp[0].string));
  }
#line 2341 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 97: /* a_sort: '(' "_" SYMBOL int_list ')'  */
#line 801 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.sort) = parser->make_sort(parser, (yyvsp[-2].string), (yyvsp[-1].intlist));
      smtlib2_vector_delete((yyvsp[-1].intlist));
      free((yyvsp[-2].string));
  }
#line 2351 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 98: /* a_sort: '(' SYMBOL sort_list ')'  */
#line 807 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.sort) = parser->make_parametric_sort(parser, (yyvsp[-2].string), (yyvsp[-1].sortlist));
      smtlib2_vector_delete((yyvsp[-1].sortlist));
      free((yyvsp[-2].string));
  }
#line 2361 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 99: /* sort_param_list: a_sort_param  */
#line 817 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->push_sort_param_scope(parser);
      (yyval.sortlist) = smtlib2_vector_new();
      smtlib2_vector_push((yyval.sortlist), (intptr_t)(yyvsp[0].sort));
  }
#line 2371 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 100: /* sort_param_list: sort_param_list a_sort_param  */
#line 823 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.sortlist) = (yyvsp[-1].sortlist);
      smtlib2_vector_push((yyval.sortlist), (intptr_t)(yyvsp[0].sort));
  }
#line 2380 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 101: /* a_sort_param: SYMBOL  */
#line 831 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      parser->declare_sort(parser, (yyvsp[0].string), 0);
      (yyval.sort) = parser->make_sort(parser, (yyvsp[0].string), NULL);
      free((yyvsp[0].string));
  }
#line 2390 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 102: /* verbatim_term_list: verbatim_term  */
#line 841 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.stringlist) = smtlib2_vector_new();
      smtlib2_vector_push((yyval.stringlist), (intptr_t)(yyvsp[0].string));
  }
#line 2399 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 103: /* verbatim_term_list: verbatim_term_list verbatim_term  */
#line 846 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.stringlist) = (yyvsp[-1].stringlist);
      smtlib2_vector_push((yyval.stringlist), (intptr_t)(yyvsp[0].string));
  }
#line 2408 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 104: /* verbatim_term: attribute_value  */
#line 855 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = (yyvsp[0].string);
  }
#line 2416 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;

  case 105: /* verbatim_term: KEYWORD  */
#line 859 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"
  {
      (yyval.string) = (yyvsp[0].string);
  }
#line 2424 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"
    break;


#line 2428 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/build/ILAng/extern/smt-parser/src/smtlib2bisonparser.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (&yylloc, scanner, parser, YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
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
                      yytoken, &yylval, &yylloc, scanner, parser);
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
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, scanner, parser);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, scanner, parser, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, scanner, parser);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, scanner, parser);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 865 "/mnt/d/Virelle/Projects/RA/Gemmini-ila/ILAng/extern/smt-parser/src/smtlib2bisonparser.y"



smtlib2_indexed_identifier *smtlib2_indexed_identifier_new(
    const char *n, smtlib2_vector *i, smtlib2_sort t)
{
    smtlib2_indexed_identifier *ret = (smtlib2_indexed_identifier *)malloc(
        sizeof(smtlib2_indexed_identifier));
    ret->name = smtlib2_strdup(n);
    ret->idx = i;
    ret->tp = t;

    return ret;
}


void smtlib2_indexed_identifier_delete(smtlib2_indexed_identifier *i)
{
    free(i->name);
    free(i);
}


smtlib2_term smtlib2_make_term_from_identifier(
    smtlib2_parser_interface *parser,
    smtlib2_indexed_identifier *ident, smtlib2_vector *args)
{
    return parser->make_term(parser, ident->name, ident->tp, ident->idx, args);
}


void smtlib2_parser_error(YYLTYPE *yylloc, yyscan_t scanner,
                          smtlib2_parser_interface *parser,
                          const char *s)
{
    parser->handle_error(parser, s);
}
