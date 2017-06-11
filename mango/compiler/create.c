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

Expression *mgc_alloc_expression(ExpressionKind kind){
	Expression *exp = malloc(sizeof(*exp));
	exp->kind = kind;
	exp->line_number = 1;
	return exp;
}



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
	
	
	
}

void mgc_set_require_and_rename_list(RequireList *require_list, RenameList *rename_list){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	char *current_package_name_str = mgc_package_name_to_string(compiler->package_name);
	if (!dvm_equal_string(current_package_name_str, DVM_NANGO_DEFAULT_PACKAGE)) {
		
	}
	free(current_package_name_str);
	compiler->require_list = require_list;
	compiler->rename_list = rename_list;

	


}

FunctionDefinition *mgc_create_function_definition(TypeSpecifier *type, char *identifier,
												   ParameterList *parameter_list, ExceptionList *exception_list,
												   Block *block);
void mgc_function_define(TypeSpecifier *type, char *identifier,
						 ParameterList *parameter_list, ExceptionList *exception_list,
						 Block *block);

ParameterList *mgc_create_parameter(TypeSpecifier *type, char *identifier);
ParameterList *mgc_chain_parameter(ParameterList *list, TypeSpecifier *type, char *identifier);

ArgumentList *mgc_create_argument(Expression *expression);
ArgumentList *mgc_chain_argument(ArgumentList *list,Expression *expression);

ExpressionList *mgc_create_expression_list(Expression *expression);
ExpressionList *mgc_chain_expression_list(ExpressionList *list, Expression *expression);


StatementList *mgc_create_statement_list(Statement *statement);
StatementList *mgc_chain_statement_list(StatementList *statement_list, Statement *statement);



TypeSpecifier *mgc_create_type_specifier(DVM_BaseType base_type);
TypeSpecifier *mgc_create_identifier_type_specifier(char *identifier);
TypeSpecifier *mgc_create_array_type_specifier(TypeSpecifier *base);


Expression *mgc_alloc_expression(ExpressionKind kind);
Expression *mgc_create_comma_expression(Expression *left, Expression *right);
Expression *mgc_create_assign_expression(Expression *left, AssignmentOperator operator, Expression *operand);
Expression *mgc_create_binary_expression(ExpressionKind operator, Expression *left, Expression *right);
Expression *mgc_create_minus_expression(Expression *operand);
Expression *mgc_create_logic_not_expression(Expression *operand);
Expression *mgc_create_bit_not_expression(Expression *operand);
Expression *mgc_create_index_expression(Expression *array,Expression *index);
Expression *mgc_create_incdec_expression(Expression *operand,ExpressionKind inc_or_dec);
Expression *mgc_create_instanceof_expression(Expression *operand, TypeSpecifier *type);
Expression *mgc_create_identifier_expression(char *identifier);
Expression *mgc_create_function_call_expression(Expression *function, ArgumentList *argument);
Expression *mgc_create_down_cast_expression(Expression *operand,TypeSpecifier *type);
Expression *mgc_create_member_expression(Expression *expression, char *member_name);
Expression *mgc_create_boolean_expression(DVM_Boolean value);
Expression *mgc_create_null_expression(void);
Expression *mgc_create_new_expression(char *class_name, char *method_name, ArgumentList *argument);
Expression *mgc_create_array_literal_expression(ExpressionList *list);
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
