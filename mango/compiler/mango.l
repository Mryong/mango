%{
#include "mangoc.h"
#include "y.tab.h"


static void increment_line_number(){

}

int yywrap(){
	return 1;
}
DVM_Boolean st_character_literal_state;
%}

%start C_COMMENT CC_COMMENT STRING_LITERAL_STATE SHIFT_JIS_2ND_CHAR

%%
<INITIAL>"if"			{ return IF; }
<INITIAL>"else"			{ return ELSE; }
<INITIAL>"elsif" 		{ return ELSIF; }
<INITIAL>"switch"		{ return SWITHC; }
<INITIAL>"case"			{ return CASE; }
<INITIAL>"default"		{ return DEFAULT_T; }
<INITIAL>"while"		{ return WHILE; }
<INITIAL>"do"			{ return DO_T; }
<INITIAL>"for"			{ return FOR; }
<INITIAL>"foreach"		{ return FOREACH; }
<INITIAL>"return"		{ return RETURN_T; }
<INITIAL>"break"		{ return BREAK; }
<INITIAL>"continue"		{ return CONTINUE; }
<INITIAL>"null"			{ return NULL_T; }
<INITIAL>"true"			{ return TRUE_T; }
<INITIAL>"false"		{ return FALSE_T; }
<INITIAL>"try"			{ return TRY; }
<INITIAL>"catch"		{ return CATCH; }
<INITIAL>"finally"		{ return FINALLY; }
<INITIAL>"throw"		{ return THROW; }
<INITIAL>"throws"		{ return THROWS; }
<INITIAL>"boolean"		{ return BOOLEAN_T; }
<INITIAL>"void" 		{ return VOID_T; }
<INITIAL>"int" 			{ return INT_T; }
<INITIAL>"double"		{ return DOUBLE_T; }
<INITIAL>"string"		{ return STRING_T; }
<INITIAL>"native_pointer"	{ return NATIVE_POINTER_T; }
<INITIAL>"new"			{ return NEW; }
<INITIAL>"require"		{ return REQUIRE; }
<INITIAL>"rename"		{ return RENAME; }
<INITIAL>"class"		{ return CLASS_T; }
<INITIAL>"interface"	{ return INTERFACE_T; }
<INITIAL>"public"		{ return PUBLIC_T; }
<INITIAL>"private"		{ return PRIVATE_T; }
<INITIAL>"virtual"		{ return VIRTUAL_T; }
<INITIAL>"override"		{ return OVERRIED_T; }
<INITIAL>"abstract"		{ return ABSTRACT_T; }
<INITIAL>"this"			{ return THIS_T; }
<INITIAL>"super"		{ return SUPER_T; }
<INITIAL>"constructor"	{ return CONSTRUCTOR; }
<INITIAL>"instanceof"	{ return INSTANCEOF; }
<INITIAL>"delegate"		{ return DELEGATE; }
<INITIAL>"enum"			{ return ENUM; }
<INITIAL>"final"		{ return FINAL; }
<INITIAL>"const"		{ return CONST; }
<INITIAL>"("			{ return LP; }
<INITIAL>")"			{ return RP; }
<INITIAL>"{"			{ return LC; }
<INITIAL>"}"			{ return RC; }
<INITIAL>"["			{ return LB; }
<INITIAL>"]"			{ return RB; }
<INITIAL>";"			{ return SEMICOLON; }
<INITIAL>":"			{ return COLON; }
<INITIAL>","			{ return COMMA; }
<INITIAL>"&&"			{ return LOGICAL_AND; }
<INITIAL>"||"			{ return LOGICAL_OR; }
<INITIAL>"="			{ return ASSIGN_T; }
<INITIAL>"=="			{ return EQ; }
<INITIAL>"!="			{ return NE; }
<INITIAL>">"			{ return GT; }
<INITIAL>">="			{ return GE; }
<INITIAL>"<"			{ return LT; }
<INITIAL>"<="			{ return LE; }
<INITIAL>"+"			{ return ADD; }
<INITIAL>"-"			{ return SUB; }
<INITIAL>"*"			{ return MUL; }
<INITIAL>"/"			{ return DIV; }
<INITIAL>"%"			{ return MOD; }
<INITIAL>"&"			{ return BIT_AND; }
<INITIAL>"|"			{ return BIT_OR; }
<INITIAL>"^"			{ return BIT_XOR; }
<INITIAL>"~"			{ return BIT_NOT; }
<INITIAL>"+="			{ return ADD_ASSIGN_T; }
<INITIAL>"-="			{ return SUB_ASSIGN_T; }
<INITIAL>"*="			{ return MUL_ASSIGN_T; }
<INITIAL>"/="			{ return DIV_ASSIGN_T; }
<INITIAL>"%="			{ return MOD_ASSIGN_T; }
<INITIAL>"++"			{ return INCREMENT; }
<INITIAL>"--"			{ return DECREMENT; }
<INITIAL>"!"			{ return EXCLAMATION; }
<INITIAL>"."			{ return DOT_T; }
<INITIAL>"::"			{ return DOWN_CAST_BEGIN; }
<INITIAL>":>"			{ return DOWN_CAST_END; }


<INITIAL>[A-Za-z_][A-Za-z_0-9]*	{
	yylval.identifer = mgc_create_identifier(yytext);
	return IDENTIFER;
}

<INITIAL>[1-9][0-9]* {
	Expression *expression = mgc_alloc_expression(INT_EXPRESSION);
	sscanf(yytext, "%d",&expression->u.int_value);
	yylval.expression = expression;
	return INT_LITERAL;
}

<INITIAL>"0"[xX][0-9a-fA-F]+ {
	Expression *expression = mgc_alloc_expression(INT_EXPRESSION);
	sscanf(yytext, "%x",&expression->u.int_value);
	yylval.expression = expression;
	return INT_LITERAL;
}


<INITIAL>"0" {
	Expression *expression = mgc_alloc_expression(INT_EXPRESSION);
	expression->u.int_value = 0;
	yylval.expression = expression;
	return INT_LITERAL;
}


<INITIAL>[0-9]\.[0-9]+ {
	Expression *expression = mgc_alloc_expression(DOUBLE_EXPRESSION);
	sscanf(yytext, "%lf",&expression->u.double_value);
	yylval.expression = expression;
	return INT_LITERAL;
}

<INITIAL>\"	{
	st_character_literal_state = DVM_FALSE;
	BEGIN STRING_LITERAL_STATE;
}

<INITIAL>\'	{
	st_character_literal_state = DVM_TRUE;
	BEGIN STRING_LITERAL_STATE;
}

<INITIAL>[ \t] ;
<INITIAL>[\r\n] { increment_line_number(); }

<INITIAL>"/*" {	BEGIN C_COMMENT; }
<INITIAL>"//" { BEGIN CC_COMMENT; }
<INITIAL>.	{
	mgc_compile_error(0);
}


<C_COMMENT>\n { increment_line_number();}
<C_COMMENT>. ;
<C_COMMENT>"*/"	{ BEGIN INITIAL; }
<C_COMMENT><<EOF>>	{
	mgc_compile_error(0);
}

<CC_COMMENT>.	;
<CC_COMMENT>\n	{
	increment_line_number();
	BEGIN INITIAL;
}
<CC_COMMENT><<EOF>> { BEGIN INITIAL; }

<STRING_LITERAL_STATE>\"	{
	if (st_character_literal_state){
		mgc_add_string_literal('\"');
	}else{
		Expression *expression = mgc_alloc_expression(STRING_EXPRESSION);
		expression->u.string_value = mgc_close_string_literal();
		yylval.expression = expression;
		BEGIN INITIAL;
		return STRING_LITERAL;
	}
}

<STRING_LITERAL_STATE>\'	{
	if(st_character_literal_state){
		Expression *expression = mgc_alloc_expression(INT_EXPRESSION);
		expression->u.int_value = mgc_close_character_literal();
		BEGIN INITIAL;
		return INT_LITERAL;
	}else{
		mgc_add_string_literal('\'');
	}
}

<STRING_LITERAL_STATE>\n	{
	mgc_add_string_literal('\n');
	increment_line_number();
}

<STRING_LITERAL_STATE>\\\" { mgc_add_string_literal('\"'); }
<STRING_LITERAL_STATE>\\n	{ mgc_add_string_literal('\n'); }
<STRING_LITERAL_STATE>\\t	{ mgc_add_string_literal('\t'); }
<STRING_LITERAL_STATE>\\\\	{ mgc_add_string_literal('\\'); }
<STRING_LITERAL_STATE>[\x81-\x9f\xe0-\xef][\x40-\x7e\x80-\xfc] {
  mgc_add_string_literal(yytext[0]);
  mgc_add_string_literal(yytext[1]);
}
<STRING_LITERAL_STATE><<EOF>>	{
	mgc_compile_error(0);
}

<STRING_LITERAL_STATE>.	{
	mgc_add_string_literal(yytext[0]);
}




%%
