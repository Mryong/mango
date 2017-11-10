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
     IDENTIFIER = 258,
     STRING_LITERAL = 259,
     COLON = 260,
     SEMICOLON = 261,
     COMMA = 262,
     LA = 263,
     RA = 264,
     LP = 265,
     RP = 266,
     LB = 267,
     RB = 268,
     LC = 269,
     RC = 270,
     CLASS = 271,
     STRUCT = 272,
     DECLARE = 273,
     PROPERTY = 274,
     WEAK = 275,
     STRONG = 276,
     COPY = 277,
     ASSIGN = 278,
     NONATOMIC = 279,
     ATOMIC = 280,
     MINUS = 281,
     PLUS = 282,
     ADDRESS = 283,
     ASTERISK = 284,
     AND = 285,
     NS_INTEGER = 286,
     NS_U_INTEGER = 287,
     CG_FLOAT = 288,
     DOUBLE = 289,
     NS_STRING = 290,
     NS_NUMBER = 291,
     NS_ARRAY = 292,
     NS_MUTABLE_ARRAY = 293,
     NS_DICTIONARY = 294,
     NS_MUTABLE_DICTIONARY = 295,
     ID = 296,
     CG_RECT = 297,
     CG_SIZE = 298,
     CG_POINT = 299,
     CG_AFFINE_TRANSFORM = 300,
     NS_RANGE = 301
   };
#endif
/* Tokens.  */
#define IDENTIFIER 258
#define STRING_LITERAL 259
#define COLON 260
#define SEMICOLON 261
#define COMMA 262
#define LA 263
#define RA 264
#define LP 265
#define RP 266
#define LB 267
#define RB 268
#define LC 269
#define RC 270
#define CLASS 271
#define STRUCT 272
#define DECLARE 273
#define PROPERTY 274
#define WEAK 275
#define STRONG 276
#define COPY 277
#define ASSIGN 278
#define NONATOMIC 279
#define ATOMIC 280
#define MINUS 281
#define PLUS 282
#define ADDRESS 283
#define ASTERISK 284
#define AND 285
#define NS_INTEGER 286
#define NS_U_INTEGER 287
#define CG_FLOAT 288
#define DOUBLE 289
#define NS_STRING 290
#define NS_NUMBER 291
#define NS_ARRAY 292
#define NS_MUTABLE_ARRAY 293
#define NS_DICTIONARY 294
#define NS_MUTABLE_DICTIONARY 295
#define ID 296
#define CG_RECT 297
#define CG_SIZE 298
#define CG_POINT 299
#define CG_AFFINE_TRANSFORM 300
#define NS_RANGE 301




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 9 "/Users/yongpengliang/Documents/macalline/ananasExample/ananas/compiler/ananas.y"
{
	char	*identifier;
}
/* Line 1529 of yacc.c.  */
#line 145 "y.tab.m"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

