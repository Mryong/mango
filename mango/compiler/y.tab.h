/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT_LITERAL = 258,
     DOUBLE_LITERAL = 259,
     STRING_LITERAL = 260,
     REGEXP_LITERAL = 261,
     IDENTIFER = 262,
     IF = 263,
     ELSE = 264,
     ELSIF = 265,
     SWITHC = 266,
     CASE = 267,
     DEFAULT_T = 268,
     WHILE = 269,
     DO_T = 270,
     FOR = 271,
     FOREACH = 272,
     RETURN_T = 273,
     BREAK = 274,
     CONTINUE = 275,
     NULL_T = 276,
     LP = 277,
     RP = 278,
     LC = 279,
     RC = 280,
     LB = 281,
     RB = 282,
     SEMICOLON = 283,
     COLON = 284,
     COMMA = 285,
     ASSIGN_T = 286,
     LOGICAL_AND = 287,
     LOGICAL_OR = 288,
     EQ = 289,
     NE = 290,
     GT = 291,
     GE = 292,
     LT = 293,
     LE = 294,
     ADD = 295,
     SUB = 296,
     MUL = 297,
     DIV = 298,
     MOD = 299,
     BIT_AND = 300,
     BIT_OR = 301,
     BIT_XOR = 302,
     BIT_NOT = 303,
     TRUE_T = 304,
     FALSE_T = 305,
     EXCLAMATION = 306,
     DOT_T = 307,
     ADD_ASSIGN_T = 308,
     SUB_ASSIGN_T = 309,
     MUL_ASSIGN_T = 310,
     DIV_ASSIGN_T = 311,
     MOD_ASSIGN_T = 312,
     INCREMENT = 313,
     DECREMENT = 314,
     TRY = 315,
     CATCH = 316,
     FINALLY = 317,
     THROW = 318,
     THROWS = 319,
     VOID_T = 320,
     BOOLEAN_T = 321,
     INT_T = 322,
     DOUBLE_T = 323,
     STRING_T = 324,
     NATIVE_POINTER_T = 325,
     NEW = 326,
     REQUIRE = 327,
     RENAME = 328,
     CLASS_T = 329,
     INTERFACE_T = 330,
     PUBLIC_T = 331,
     PRIVATE_T = 332,
     VIRTUAL_T = 333,
     OVERRIED_T = 334,
     ABSTRACT_T = 335,
     THIS_T = 336,
     SUPER_T = 337,
     CONSTRUCTOR = 338,
     INSTANCEOF = 339,
     DOWN_CAST_BEGIN = 340,
     DOWN_CAST_END = 341,
     DELEGATE = 342,
     FINAL = 343,
     ENUM = 344,
     CONST = 345
   };
#endif
/* Tokens.  */
#define INT_LITERAL 258
#define DOUBLE_LITERAL 259
#define STRING_LITERAL 260
#define REGEXP_LITERAL 261
#define IDENTIFER 262
#define IF 263
#define ELSE 264
#define ELSIF 265
#define SWITHC 266
#define CASE 267
#define DEFAULT_T 268
#define WHILE 269
#define DO_T 270
#define FOR 271
#define FOREACH 272
#define RETURN_T 273
#define BREAK 274
#define CONTINUE 275
#define NULL_T 276
#define LP 277
#define RP 278
#define LC 279
#define RC 280
#define LB 281
#define RB 282
#define SEMICOLON 283
#define COLON 284
#define COMMA 285
#define ASSIGN_T 286
#define LOGICAL_AND 287
#define LOGICAL_OR 288
#define EQ 289
#define NE 290
#define GT 291
#define GE 292
#define LT 293
#define LE 294
#define ADD 295
#define SUB 296
#define MUL 297
#define DIV 298
#define MOD 299
#define BIT_AND 300
#define BIT_OR 301
#define BIT_XOR 302
#define BIT_NOT 303
#define TRUE_T 304
#define FALSE_T 305
#define EXCLAMATION 306
#define DOT_T 307
#define ADD_ASSIGN_T 308
#define SUB_ASSIGN_T 309
#define MUL_ASSIGN_T 310
#define DIV_ASSIGN_T 311
#define MOD_ASSIGN_T 312
#define INCREMENT 313
#define DECREMENT 314
#define TRY 315
#define CATCH 316
#define FINALLY 317
#define THROW 318
#define THROWS 319
#define VOID_T 320
#define BOOLEAN_T 321
#define INT_T 322
#define DOUBLE_T 323
#define STRING_T 324
#define NATIVE_POINTER_T 325
#define NEW 326
#define REQUIRE 327
#define RENAME 328
#define CLASS_T 329
#define INTERFACE_T 330
#define PUBLIC_T 331
#define PRIVATE_T 332
#define VIRTUAL_T 333
#define OVERRIED_T 334
#define ABSTRACT_T 335
#define THIS_T 336
#define SUPER_T 337
#define CONSTRUCTOR 338
#define INSTANCEOF 339
#define DOWN_CAST_BEGIN 340
#define DOWN_CAST_END 341
#define DELEGATE 342
#define FINAL 343
#define ENUM 344
#define CONST 345




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 9 "mango.y"
{
    char     *identifer;
    Expression  *expression;
}
/* Line 1529 of yacc.c.  */
#line 234 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

