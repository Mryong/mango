//
//  create.c
//  mango_preview
//
//  Created by jerry.yong on 2017/5/25.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include "mangoc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "share.h"
#include "DVM_dev.h"




DeclarationList *mgc_chain_declaration(DeclarationList *list,Declaration *decl){
	DeclarationList *add = malloc(sizeof(*add));
	add->next = NULL;
	if (list == NULL) {
		return add;
	}
	
	DeclarationList *last = list;
	while (last->next != NULL) {
		last = last->next;
	}
	last->next = add;
	return list;
}
Declaration	*mgc_alloc_declaration(DVM_Boolean is_final,TypeSpecifier *type, char *identifier){
	Declaration *declaration = malloc(sizeof(*declaration));
	declaration->is_final = is_final;
	declaration->type = type;
	declaration->name = identifier;
	declaration->variable_index = -1;
	return declaration;
	
}
PackageName *mgc_create_package_name(char *identifier){
	PackageName *package_name = malloc(sizeof(*package_name));
	package_name->name = identifier;
	package_name->next = NULL;
	return package_name;

}
PackageName	*mgc_chain_package_name(PackageName *list, char *identifier){
	PackageName *add = mgc_create_package_name(identifier);
	if (list == NULL) {
		return add;
	}
	PackageName *last = list;
	while (last->next != NULL) {
		last = last->next;
	}
	last->next = add;
	return list;
	
	
}

RequireList *mgc_create_require_list(PackageName *package_name){
	RequireList *list = malloc(sizeof(*list));
	MGC_Compiler *current_compiler = mgc_get_current_compiler();
	char *require_page_name_str = mgc_package_name_to_string(package_name);
	char *current_package_name_str = mgc_package_name_to_string(current_compiler->package_name);
	if (dvm_equal_string(require_page_name_str, current_package_name_str)
		&& current_compiler->source_suffix == MGH_SOURCE) {
		mgc_compile_error(current_compiler->current_line_number, REQUIRE_ITSELF_ERR,MESSAGE_ARGUMENT_END);
	}
	list->package_name = package_name;
	list->line_number = current_compiler->current_line_number;
	list->next = NULL;
	return list;
}
RequireList *mgc_chain_require_list(RequireList *list,RequireList *add){
	if (list == NULL) {
		return add;
	}
	RequireList *last = list;
	char buf[LINE_BUF_SIZE];
	while (last->next) {
		if (mgc_equal_package_name(last->package_name, add->package_name)) {
			char *package_name = mgc_package_name_to_string(add->package_name);
			dvm_strncopy(buf, package_name, LINE_BUF_SIZE);
			free(package_name);
			mgc_compile_error(mgc_get_current_compiler()->current_line_number, REQUIRE_DUPLICATE_ERR,
							  STRING_MESSAGE_ARGUMENT,"package_name",buf,MESSAGE_ARGUMENT_END);
		}
		last = last->next;
		
	}
	last->next = add;
	return list;
}

RenameList *mgc_create_rename_list(PackageName *package_name, char *identifer){
	
	PackageName *tail = package_name;
	PackageName *pre_tail = NULL;
	while (tail->next) {
		pre_tail = tail;
		tail = tail->next;
	}
	
	if (pre_tail == NULL) {
		mgc_compile_error(mgc_get_current_compiler()->current_line_number, RENAME_HAS_NO_PACKAGED_NAME_ERR,MESSAGE_ARGUMENT_END);
	}
	
	pre_tail->next = NULL;
	
	
	RenameList *rename = malloc(sizeof(*rename));
	rename->package_name = package_name;
	rename->original_name = tail->name;
	rename->renamed_name = identifer;
	rename->line_number = mgc_get_current_compiler()->current_line_number;
	return rename;
}
RenameList *mgc_chain_rename_list(RenameList *list, RenameList *add){
	if (list == NULL) {
		return add;
	}
	RenameList *pos = list;
	while (pos->next) {
		pos = pos->next;
	}
	pos->next = add;
	return list;
	
}

static RequireList *add_default_package(RequireList *require_list){
	RequireList *pos = require_list;
	DVM_Boolean default_package_has_required = DVM_FALSE;
	while (pos->next) {
		char *temp = mgc_package_name_to_string(pos->package_name);
		if (dvm_equal_string(temp, DVM_NANGO_DEFAULT_PACKAGE)) {
			default_package_has_required = DVM_TRUE;
			free(temp);
			break;
		}
		free(temp);
		pos = pos->next;
	}
	
	if (!default_package_has_required) {
		PackageName *dpn = mgc_create_package_name(DVM_MANGO_DEFAULE_PACKAGE_P1);
		dpn = mgc_chain_package_name(dpn, DVM_NANGO_DEFAULT_PACKAGE_P2);
	    RequireList *new_list = mgc_create_require_list(dpn);
		new_list->next = require_list;
		return new_list;
	}
	
	return require_list;
}

void mgc_set_require_and_rename_list(RequireList *require_list, RenameList *rename_list){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	char *current_package_name_str = mgc_package_name_to_string(compiler->package_name);
	if (!dvm_equal_string(current_package_name_str, DVM_NANGO_DEFAULT_PACKAGE)) {
		require_list = add_default_package(require_list);
	}
	free(current_package_name_str);
	compiler->require_list = require_list;
	compiler->rename_list = rename_list;
}

FunctionDefinition *mgc_create_function_definition(TypeSpecifier *type, char *identifier,
												   ParameterList *parameter_list, ExceptionList *exception_list,
												   Block *block){
	
	MGC_Compiler *compiler = mgc_get_current_compiler();
	
	FunctionDefinition  *function_definition = malloc(sizeof(*function_definition));
	function_definition->type = type;
	function_definition->name = identifier;
	function_definition->parameter_list = parameter_list;
	function_definition->throws = exception_list;
	function_definition->block = block;
	function_definition->class_definition = NULL;
	function_definition->local_variable_count = 0;
	function_definition->local_variable = NULL;
	function_definition->end_line_number = compiler->current_line_number;
	function_definition->next = NULL;
	if (block) {
		block->type = FUNCTION_BLOCK;
		block->parent.function_info.function = function_definition;
	}
	return function_definition;
	
}
void mgc_function_define(TypeSpecifier *type, char *identifier,
						 ParameterList *parameter_list, ExceptionList *exception_list,
						 Block *block){
	
	if (mgc_search_function(identifier) || mgc_search_declaration(identifier, NULL)) {
		mgc_compile_error(mgc_get_current_compiler()->current_line_number, FUNCTION_MULTIPLE_DEFINI_ERR,STRING_MESSAGE_ARGUMENT,"name",identifier,MESSAGE_ARGUMENT_END);
	}
	
	mgc_create_function_definition(type, identifier, parameter_list, exception_list, block);
	
	


}

ParameterList *mgc_create_parameter(TypeSpecifier *type, char *identifier){
	ParameterList *parameter = malloc(sizeof(*parameter));
	parameter->type = type;
	parameter->name = identifier;
	parameter->line_number = mgc_get_current_compiler()->current_line_number;
	parameter->next = NULL;
	return parameter;
}

ParameterList *mgc_chain_parameter(ParameterList *list, TypeSpecifier *type, char *identifier){
	ParameterList *add = mgc_create_parameter(type, identifier);
	ParameterList *pos = list;
	for (; pos->next; pos = pos->next)
		;
	pos->next = add;
	return list;
}

ArgumentList *mgc_create_argument(Expression *expression){
	ArgumentList *argument = malloc(sizeof(*argument));
	argument->expression = expression;
	argument->next = NULL;
	return argument;

}

ArgumentList *mgc_chain_argument(ArgumentList *list,Expression *expression){
	ArgumentList *pos = list;
	for (; pos->next; pos = pos->next)
		;
	pos->next = mgc_create_argument(expression);
	return list;

}

ExpressionList *mgc_create_expression_list(Expression *expression){
	ExpressionList *expression_list = malloc(sizeof(*expression_list));
	expression_list->expression = expression;
	expression_list->next = NULL;
	return expression_list;
}


ExpressionList *mgc_chain_expression_list(ExpressionList *list, Expression *expression){
	ExpressionList *pos = list;
	for (; pos->next; pos = pos->next)
		;
	pos->next = mgc_create_expression_list(expression);
	return list;
}


StatementList *mgc_create_statement_list(Statement *statement){
	StatementList *statement_list = malloc(sizeof(*statement_list));
	statement_list->statement = statement;
	statement_list->next = NULL;
	return statement_list;
}

StatementList *mgc_chain_statement_list(StatementList *statement_list, Statement *statement){
	StatementList *add = mgc_create_statement_list(statement);
	if (statement_list == NULL) {
		return add;
	}
	
	StatementList *pos = statement_list;
	for (; pos->next; pos = pos->next)
		;
	
	pos->next = add;
	return statement_list;
	
}



TypeSpecifier *mgc_create_type_specifier(DVM_BaseType base_type){
	TypeSpecifier *ts = mgc_alloc_type_specifier(base_type);
	ts->line_number = mgc_get_current_compiler()->current_line_number;
	return ts;
}

TypeSpecifier *mgc_create_identifier_type_specifier(char *identifier){
	TypeSpecifier *ts = mgc_alloc_type_specifier(DVM_UNSPECIFIED_IDENTIFIER_TYPE);
	ts->identifier = identifier;
	ts->line_number = mgc_get_current_compiler()->current_line_number;
	return ts;

}
TypeSpecifier *mgc_create_array_type_specifier(TypeSpecifier *base){
	TypeDerive *type_derive = mgc_alloc_type_derive(ARRAY_DERIVE);
	if (base->derive == NULL) {
		base->derive = type_derive;
	}else{
		TypeDerive *pos;
		for (pos = base->derive; pos->next; pos = pos->next)
			;
		pos->next = type_derive;
	
	}
	
	return base;
	
}


Expression *mgc_alloc_expression(ExpressionKind kind){
	Expression *exp = malloc(sizeof(*exp));
	exp->kind = kind;
	exp->type = NULL;
	exp->line_number = mgc_get_current_compiler()->current_line_number;
	return exp;

}


Expression *mgc_create_comma_expression(Expression *left, Expression *right){
	Expression *exp = mgc_alloc_expression(COMMA_EXPRESSION);
	exp->u.comma_expression.left = left;
	exp->u.comma_expression.right = right;
	return exp;
}
Expression *mgc_create_assign_expression(Expression *left, AssignmentOperator operator, Expression *operand){
	Expression *exp = mgc_alloc_expression(ASSIGN_EXPRESSION);
	exp->u.assignment_expression.operator = operator;
	exp->u.assignment_expression.left = left;
	exp->u.assignment_expression.operand = operand;
	return exp;
}
Expression *mgc_create_binary_expression(ExpressionKind operator, Expression *left, Expression *right){
	Expression *exp = mgc_alloc_expression(operator);
	exp->u.binary_expression.left = left;
	exp->u.binary_expression.right = right;
	return exp;
}

Expression *mgc_create_minus_expression(Expression *operand){
	Expression *exp = mgc_alloc_expression(MINUS_EXPRESSION);
	exp->u.minus_expression = operand;
	return exp;
}


Expression *mgc_create_logic_not_expression(Expression *operand){
	Expression *exp = mgc_alloc_expression(LOGICAL_NOT_EXPRESSION);
	exp->u.logic_not = operand;
	return exp;
	

}
Expression *mgc_create_bit_not_expression(Expression *operand){
	Expression *exp = mgc_alloc_expression(BIT_NOT_EXPRESSION);
	exp->u.bit_not = exp;
	return exp;

}
Expression *mgc_create_index_expression(Expression *array,Expression *index){
	Expression *exp = mgc_alloc_expression(INDEX_EXPRESSION);
	exp->u.index_expression.array = array;
	exp->u.index_expression.index = index;
	return exp;
}
Expression *mgc_create_incdec_expression(Expression *operand,ExpressionKind inc_or_dec){
	Expression *exp = mgc_alloc_expression(inc_or_dec);
	exp->u.inc_dec.operand = operand;
	return exp;

}
Expression *mgc_create_instanceof_expression(Expression *operand, TypeSpecifier *type){
	Expression *exp = mgc_alloc_expression(INSTANCEOF_EXPRESSION);
	exp->u.instanceof.expression = operand;
	exp->u.instanceof.type = type;
	return exp;
}

Expression *mgc_create_identifier_expression(char *identifier){
	Expression *exp = mgc_alloc_expression(IDENTIFIER_EXPRESSION);
	exp->u.identifer_express.name = identifier;
	return exp;
}

Expression *mgc_create_function_call_expression(Expression *function, ArgumentList *argument){
	Expression *exp = mgc_alloc_expression(FUNCTION_CALL_EXPRESSION);
	exp->u.function_call_expression.function = function;
	exp->u.function_call_expression.argument = argument;
	return exp;
}


Expression *mgc_create_down_cast_expression(Expression *operand,TypeSpecifier *type){
	Expression *exp = mgc_alloc_expression(DOWN_CAST_EXPRESSION);
	exp->u.down_cast.operand = operand;
	exp->u.down_cast.type = type;
	return exp;
}
Expression *mgc_create_member_expression(Expression *expression, char *member_name){
	Expression *exp = mgc_alloc_expression(MEMBER_EXPRESSION);
	exp->u.member_expression.expression = expression;
	exp->u.member_expression.member_name = member_name;
	return exp;
}
Expression *mgc_create_boolean_expression(DVM_Boolean value){
	Expression *exp = mgc_alloc_expression(BOOLEAN_EXPRESSION);
	exp->u.boolean_value = value;
	return exp;
}
Expression *mgc_create_null_expression(void){
	Expression *exp = mgc_alloc_expression(NULL_EXPRESSION);
	return exp;


}

Expression *mgc_create_new_expression(char *class_name, char *method_name, ArgumentList *argument){
	Expression *exp = mgc_alloc_expression(NEW_EXPRESSION);
	exp->u.new_e.argument = argument;
	exp->u.new_e.class_name = class_name;
	exp->u.new_e.method_name = method_name;
	return exp;

}
Expression *mgc_create_array_literal_expression(ExpressionList *list){
	Expression *exp = mgc_alloc_expression(ARRAY_LITERAL_EXPRESSION);
	exp->u.array_literal = list;
	return exp;

}
Expression *mgc_create_base_array_creation(DVM_BaseType base_type,
										   ArrayDimension *dim_expr_list,
										   ArrayDimension *dim_list);


Expression *mgc_create_class_array_creation(TypeSpecifier *type,
											ArrayDimension *dim_expr_list,
											ArrayDimension *dim_list);

Expression *mgc_create_this_expression(void);
Expression *mgc_create_super_expression(void);

ArrayDimension *mgc_create_array_dimension(Expression *expression);
ArrayDimension *mgc_chain_array_dimension(ArrayDimension *list,
										  ArrayDimension *dim);

Statement *mgc_alloc_statement(StatementType type);
Statement *mgc_create_if_statement(Expression *condition,
								   Block *then_block, Elsif *elsif_list,
								   Block *else_block);

Elsif *mgc_create_elsif_statement(Expression *expr, Block *block);
Elsif *mgc_chain_elsif_statement(Elsif *list, Elsif *add);

Statement *mgc_create_switch_statement(Expression *expr,
									   CaseList *case_list,
									   Block *default_block);

CaseList *mgc_create_one_case(ExpressionList *expression_list, Block *block);
CaseList *mgc_chain_case(CaseList *list, CaseList *add);

Statement *mgc_create_while_statement(char *label,
									  Expression *condition,
									  Block *block);

Statement *mgc_create_foreach_statement(char *label,
										char *variable,
										Expression *collection,
										Block *block);

Statement *mgc_create_for_statement(char *label,
									Expression *init,
									Expression *condition,
									Expression *post,
									Block *block);

Statement *mgc_create_do_while_statement(char *label,
										 Block *block,
										 Expression *condition);


Block *mgc_alloc_block(void);
Block *mgc_open_block(void);
Block *mgc_close_block(Block *block, StatementList *statement_list);

Statement *mgc_create_expression_statement(Expression *expression);
Statement *mgc_create_return_statement(Expression *expression);
Statement *mgc_create_break_statement(char *label);
Statement *mgc_create_continue_statement(char *label);
Statement *mgc_create_try_statement(Block *try_block,
									CatchClause *catch_clause,
									Block *finally_block);

CatchClause *mgc_create_catch_clause(TypeSpecifier *type,
									 char *variable_name,
									 Block *block);
CatchClause *mgc_start_catch_clause(void);
CatchClause *mgc_end_catch_clause(CatchClause *catch_clause,
								  TypeSpecifier *type,
								  char *variable_name,
								  Block *block);
CatchClause *mgc_chain_catch_clause(CatchClause *list, CatchClause *add);


Statement *mgc_create_throw_statement(Expression *expression);
Statement *mgc_create_declaration_statement(DVM_Boolean is_final,
											TypeSpecifier *type,
											char *identifier,
											Expression *initializer);

void mgc_start_class_definition(ClassOrMemberModifierList *modifier,
								DVM_ClassOrInterface class_or_interface,
								char *identifier,
								ExtendsList *extends);

void mgc_class_define(MemberDeclaration *member_list);
ExtendsList *mgc_create_extends_list(char *identifier);
ExtendsList *mgc_chain_extends_list(ExtendsList *list, char *add);

ClassOrMemberModifierList mgc_create_class_or_member_modifier(ClassOrMemberModifierKind kind);
ClassOrMemberModifierList mgc_chain_class_or_member_modifier(ClassOrMemberModifierList list,
															 ClassOrMemberModifierList add);

MemberDeclaration *mgc_chain_member_declaration_list(MemberDeclaration *list,
													 MemberDeclaration *add);

MemberDeclaration *mgc_create_method_member(ClassOrMemberModifierList *modifier,
											FunctionDefinition *function_definition,
											DVM_Boolean is_final);
FunctionDefinition *mgc_method_function_definition(TypeSpecifier *type,
												   char *identifier,
												   ParameterList *parameter,
												   ExceptionList *thorws,
												   Block *block);
FunctionDefinition *mgc_constructor_function_definition(char *identifier,
														ParameterList *parameter,
														ExceptionList *throws,
														Block *block);

MemberDeclaration *mgc_create_field_member(ClassOrMemberModifierList *modifier,
										   DVM_Boolean *is_final,
										   TypeSpecifier *type,
										   char *name,
										   Expression *initializer);

ExceptionList *mgc_create_thorws(char *identifer);
ExceptionList *mgc_chain_exception_list(ExceptionList *list, char *identifier);

void mgc_create_delegate_definition(TypeSpecifier *type, char *identifier,
									ParameterList *parameter, ExceptionList *thorws);


void mgc_create_enum_definition(char *identifier, Enumerator *enumerator);
Enumerator *mgc_create_enumerator(char *identifier);
Enumerator *mgc_chain_enumerator(Enumerator *enumerator, char *identifier);

void *mgc_create_const_definition(TypeSpecifier *type,
								  char *identifier,
								  Expression *initializer);





int yyerror(char const *str){
	printf("%s\n",str);
	return 0;
}

void mgc_compile_error(int line_number,CompileError err,...){
	printf("%s\n","mgc_compile_error");
	exit(1);
}


void t(){
	Expression *expression = mgc_alloc_expression(INT_EXPRESSION);
	sscanf("", "%d",&expression->u.int_value);
	
}
