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
#include <assert.h>




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
	while (pos) {
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
										   ArrayDimension *dim_list){
	TypeSpecifier *type = mgc_alloc_type_specifier(base_type);
	Expression *exp = mgc_create_class_array_creation(type, dim_expr_list, dim_list);
	return exp;


}


Expression *mgc_create_class_array_creation(TypeSpecifier *type,
											ArrayDimension *dim_expr_list,
											ArrayDimension *dim_list){
	
	Expression *exp = mgc_alloc_expression(ARRAY_CREATION_EXPRESSION);
	exp->u.array_creation.type = type;
	exp->u.array_creation.dimension = mgc_chain_array_dimension(dim_expr_list, dim_list);
	return exp;
}

Expression *mgc_create_this_expression(void){
	Expression *exp = mgc_alloc_expression(THIS_EXPRESSION);
	return exp;
}


Expression *mgc_create_super_expression(void){
	Expression *exp = mgc_alloc_expression(SUPER_EXPRESSION);
	return exp;
}

ArrayDimension *mgc_create_array_dimension(Expression *expression){
	ArrayDimension *dim = malloc(sizeof(*dim));
	dim->expression = expression;
	dim->next = NULL;
	return dim;

}

ArrayDimension *mgc_chain_array_dimension(ArrayDimension *list,
										  ArrayDimension *dim){
	ArrayDimension *pos = list;
	for (; pos->next; pos = pos->next)
		;
	pos->next = dim;
	return list;
}

Statement *mgc_alloc_statement(StatementType type){
	Statement *statement = malloc(sizeof(*statement));
	statement->type = type;
	statement->line_number = mgc_get_current_compiler()->current_line_number;
	return statement;

}


Statement *mgc_create_if_statement(Expression *condition,
								   Block *then_block, Elsif *elsif_list,
								   Block *else_block){
	Statement *if_statement = mgc_alloc_statement(IF_STATEMENT);
	if_statement->u.if_s.condition = condition;
	if_statement->u.if_s.then_block = then_block;
	if_statement->u.if_s.elsif_list = elsif_list;
	if_statement->u.if_s.else_block = else_block;
	return if_statement;
}


Elsif *mgc_create_elsif_statement(Expression *expr, Block *block){
	Elsif *elsif = malloc(sizeof(*elsif));
	elsif->condition = expr;
	elsif->then_block = block;
	elsif->next = NULL;
	return  elsif;

}
Elsif *mgc_chain_elsif_statement(Elsif *list, Elsif *add){
	
	Elsif *pos;
	for (pos = list; pos->next; pos = pos->next)
		;
	pos->next = add;
	return list;
}

Statement *mgc_create_switch_statement(Expression *expr,
									   CaseList *case_list,
									   Block *default_block){
	Statement *statement = mgc_alloc_statement(SWITCH_STATEMENT);
	statement->u.switch_s.expression = expr;
	statement->u.switch_s.case_list = case_list;
	statement->u.switch_s.default_block = default_block;
	return statement;
}

CaseList *mgc_create_one_case(ExpressionList *expression_list, Block *block){
	CaseList *one_case = malloc(sizeof(*one_case));
	one_case->expression_list= expression_list;
	one_case->block = block;
	one_case->next = NULL;
	return one_case;
}
CaseList *mgc_chain_case(CaseList *list, CaseList *add){
	CaseList *pos;
	for (pos = list; pos->next; pos = pos->next)
		;
	pos->next = add;
	return pos;
}

Statement *mgc_create_while_statement(char *label,
									  Expression *condition,
									  Block *block){
	Statement *while_s = mgc_alloc_statement(WHILE_STATEMENT);
	while_s->u.while_s.label = label;
	while_s->u.while_s.condition = condition;
	while_s->u.while_s.block = block;
	block->type = WHILE_STATEMENT_BLOCK;
	block->parent.statement_info.statement = while_s;
	return while_s;
}

Statement *mgc_create_foreach_statement(char *label,
										char *variable,
										Expression *collection,
										Block *block){
	Statement *foreach_s = mgc_alloc_statement(FOREACH_STATEMENT);
	foreach_s->u.foreach_s.label = label;
	foreach_s->u.foreach_s.variable = variable;
	foreach_s->u.foreach_s.collection = collection;
	foreach_s->u.foreach_s.block = block;
	return foreach_s;
}


Statement *mgc_create_for_statement(char *label,
									Expression *init,
									Expression *condition,
									Expression *post,
									Block *block){
	Statement *for_s = mgc_alloc_statement(FOR_STATEMENT);
	for_s->u.for_s.label = label;
	for_s->u.for_s.init = init;
	for_s->u.for_s.condition = condition;
	for_s->u.for_s.post = post;
	for_s->u.for_s.block = block;
	block->type = FOR_STATEMENT_BLOCK;
	block->parent.statement_info.statement = for_s;
	return for_s;
}

Statement *mgc_create_do_while_statement(char *label,
										 Block *block,
										 Expression *condition){
	Statement *do_while_s  = mgc_alloc_statement(DO_WHILE_STATEMENT);
	do_while_s->u.do_while_s.label = label;
	do_while_s->u.do_while_s.block = block;
	do_while_s->u.do_while_s.condition = condition;
	block->type = DO_WHILE_STATEMENT_BLOCK;
	block->parent.statement_info.statement = do_while_s;
	return do_while_s;
}


Block *mgc_alloc_block(void){
	Block *block = malloc(sizeof(*block));
	block->type = UNDEFINED_BLOCK;
	block->declaration_list = NULL;
	block->out_block = NULL;
	block->statement_list = NULL;
	return block;
}


Block *mgc_open_block(void){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	Block *new_block = mgc_alloc_block();
	new_block->out_block = compiler->current_block;
	compiler->current_block = new_block;
	return new_block;
	
}
Block *mgc_close_block(Block *block, StatementList *statement_list){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	assert(compiler->current_block == block);
	block->statement_list = statement_list;
	compiler->current_block = block->out_block;
	return block;
}

Statement *mgc_create_expression_statement(Expression *expression){
	Statement *expression_s = mgc_alloc_statement(EXPRESSION_STATEMENT);
	expression_s->u.expression_s = expression;
	return expression_s;

}
Statement *mgc_create_return_statement(Expression *expression){
	Statement *return_s = mgc_alloc_statement(RETURN_STATEMENT);
	return_s->u.return_s.return_value = expression;
	return return_s;
}
Statement *mgc_create_break_statement(char *label){
	Statement *break_s = mgc_alloc_statement(BREAK_STATEMENT);
	break_s->u.break_s.label = label;
	return break_s;

}
Statement *mgc_create_continue_statement(char *label){
	Statement *continue_s = mgc_alloc_statement(CONTINUE_STATEMENT);
	continue_s->u.continue_s.label = label;
	return continue_s;
}

Statement *mgc_create_try_statement(Block *try_block,
									CatchClause *catch_clause,
									Block *finally_block){
	Statement *try_s = mgc_alloc_statement(TRY_STATEMENT);
	try_s->u.try_s.try_block = try_block;
	try_block->type = TRY_CLAUSE_BLOCK;
	try_s->u.try_s.catch_clause = catch_clause;
	if (finally_block) {
		finally_block->type = FINALLY_CLAUSE_BLOCK;
	}
	try_s->u.try_s.finally_block = finally_block;
	return try_s;
}

CatchClause *mgc_create_catch_clause(TypeSpecifier *type,
									 char *variable_name,
									 Block *block){
	CatchClause *catch_clause = alloca(sizeof(*catch_clause));
	catch_clause->type = type;
	catch_clause->variable_name = variable_name;
	catch_clause->block = block;
	block->type = CATCH_CLAUSE_BLOCK;
	return catch_clause;
	
}
CatchClause *mgc_start_catch_clause(void){
	CatchClause *cc = malloc(sizeof(*cc));
	cc->line_number = mgc_get_current_compiler()->current_line_number;
	cc->next = NULL;
	return cc;
}
CatchClause *mgc_end_catch_clause(CatchClause *catch_clause,
								  TypeSpecifier *type,
								  char *variable_name,
								  Block *block){
	catch_clause->type = type;
	catch_clause->variable_name = variable_name;
	catch_clause->block = block;
	return catch_clause;
}

CatchClause *mgc_chain_catch_clause(CatchClause *list, CatchClause *add){
	CatchClause *pos;
	for (pos = list; pos->next; pos = pos->next)
		;
	pos->next = add;
	return pos;

}


Statement *mgc_create_throw_statement(Expression *expression){
	Statement *throw_statement = mgc_alloc_statement(THROW_STATEMENT);
	throw_statement->u.throw_s.expression = expression;
	return throw_statement;
	
}

Statement *mgc_create_declaration_statement(DVM_Boolean is_final,
											TypeSpecifier *type,
											char *identifier,
											Expression *initializer){
	if (is_final && initializer == NULL) {
		mgc_compile_error(mgc_get_current_compiler()->current_line_number, FINAL_VARIABLE_WITHOUT_INITIALIZER_ERR,
						  STRING_MESSAGE_ARGUMENT,"name",identifier,MESSAGE_ARGUMENT_END);
	}
	Statement *ds = mgc_alloc_statement(DECLARATION_STATEMENT);
	Declaration *declaration = mgc_alloc_declaration(is_final, type, identifier);
	declaration->initializer = initializer;
	ds->u.declaration_s = declaration;
	return ds;
}


DVM_AccessModifier conv_access_modifier(ClassOrMemberModifierKind src){
	if (src == PUBLIC_MODIFIER) {
		return DVM_PUBLIC_MODIFIER;
	}
	
	if (src == PRIVATE_MODIFIER) {
		return DVM_PRIVATE_MODIFIER;
	}
	
	assert(src == NOT_SPECIFIED_MODIFIER);
	
	return DVM_FILE_MODIFIER;

}

void mgc_start_class_definition(ClassOrMemberModifierList *modifier,
								DVM_ClassOrInterface class_or_interface,
								char *identifier,
								ExtendsList *extends){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	ClassDefinition *class_definition = malloc(sizeof(*class_definition));
	class_definition->is_abstract = class_or_interface == DVM_INTERFACE_DEFINITION;
	class_definition->access_modifier = DVM_FILE_MODIFIER;
	
	if (modifier) {
		if (modifier->id_abstract == ABSTRACT_MODIFIER) {
			class_definition->is_abstract = DVM_TRUE;
		}
		
		class_definition->access_modifier = conv_access_modifier(modifier->access_modifier);
	}
	
	class_definition->class_or_interface = class_or_interface;
	class_definition->package_name = compiler->package_name;
	class_definition->name = identifier;
	class_definition->extends = extends;
	class_definition->super_class = NULL;
	class_definition->interface_list = NULL;
	class_definition->member = NULL;
	class_definition->next = NULL;
	class_definition->line_number = compiler->current_line_number;
	
	assert(compiler->current_catch_clause == NULL);
	
	compiler->current_class_definition = class_definition;

 }

void mgc_class_define(MemberDeclaration *member_list){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	ClassDefinition *current_class = compiler->current_class_definition;
	assert(current_class != NULL);
	if (compiler->class_definition_list == NULL) {
		compiler->class_definition_list = current_class;
	}else{
		ClassDefinition *pos;
		for (pos = compiler->class_definition_list; pos->next; pos = pos->next)
			;
		pos->next = current_class;
	}
	current_class->member = member_list;
	compiler->current_class_definition = NULL;
}


ExtendsList *mgc_create_extends_list(char *identifier){
	ExtendsList *extends = malloc(sizeof(*extends));
	extends->identifer = identifier;
	extends->class_definition = NULL;
	extends->next = NULL;
	return extends;
}

ExtendsList *mgc_chain_extends_list(ExtendsList *list, char *add){
	ExtendsList *pos;
	for (pos = list; pos->next; pos = pos->next)
		;
	
	pos->next = mgc_create_extends_list(add);
	return list;
	

}

ClassOrMemberModifierList mgc_create_class_or_member_modifier(ClassOrMemberModifierKind kind){
	ClassOrMemberModifierList ret;
	ret.access_modifier = NOT_SPECIFIED_MODIFIER;
	ret.id_abstract = NOT_SPECIFIED_MODIFIER;
	ret.is_virtual = NOT_SPECIFIED_MODIFIER;
	ret.is_override = NOT_SPECIFIED_MODIFIER;
	
	switch (kind) {
		case ABSTRACT_MODIFIER:
			ret.id_abstract = ABSTRACT_MODIFIER;
			break;
			
		case PUBLIC_MODIFIER:
			ret.access_modifier = PUBLIC_MODIFIER;
			break;
		
		case PRIVATE_MODIFIER:
			ret.access_modifier = PRIVATE_MODIFIER;
			break;
			
		case VIRTUAL_MODIFIER:
			ret.is_virtual = VIRTUAL_MODIFIER;
			break;
			
		case OVERRIDE_MODIFIER:
			ret.is_override = OVERRIDE_MODIFIER;
			break;
				
	  default:
			assert(0);
			break;
	}
	
	return ret;
	
}


ClassOrMemberModifierList mgc_chain_class_or_member_modifier(ClassOrMemberModifierList list,
															 ClassOrMemberModifierList add){
	if (add.id_abstract != NOT_SPECIFIED_MODIFIER) {
		assert(add.id_abstract == ABSTRACT_MODIFIER);
		if (list.id_abstract != NOT_SPECIFIED_MODIFIER) {
			mgc_compile_error(mgc_get_current_compiler()->current_line_number, ABSTRACT_MULTIPLE_SPECIFIED_ERR,MESSAGE_ARGUMENT_END);
		}
		list.id_abstract = ABSTRACT_MODIFIER;
		
	}else if(add.access_modifier != NOT_SPECIFIED_MODIFIER){
		assert(add.access_modifier == PUBLIC_MODIFIER || add.access_modifier == PRIVATE_MODIFIER);
		if (list.access_modifier != NOT_SPECIFIED_MODIFIER) {
			mgc_compile_error(mgc_get_current_compiler()->current_line_number, ACCESS_MODIFIER_MULTIPLE_SPECIFIED_ERR,MESSAGE_ARGUMENT_END);
		}
		list.access_modifier = add.access_modifier;
		
	}else if (add.is_override != NOT_SPECIFIED_MODIFIER){
		assert(add.is_override == OVERRIDE_MODIFIER);
		if (list.is_override != NOT_SPECIFIED_MODIFIER) {
			mgc_compile_error(mgc_get_current_compiler()->current_line_number, OVERRIDE_MODIFIER_MULTIPLE_SPECIFIED_ERR,MESSAGE_ARGUMENT_END);
		}
		list.is_override = OVERRIDE_MODIFIER;
	
	}else if (add.is_virtual != NOT_SPECIFIED_MODIFIER){
		assert(add.is_virtual = VIRTUAL_MODIFIER);
		if (list.is_virtual != NOT_SPECIFIED_MODIFIER) {
			mgc_compile_error(mgc_get_current_compiler()->current_line_number, VIRTUAL_MODIFIER_MULTIPLE_SPECIFIED_ERR,MESSAGE_ARGUMENT_END);
		}
		list.is_virtual = VIRTUAL_MODIFIER;
	}
	
	return list;
}


MemberDeclaration *mgc_chain_member_declaration_list(MemberDeclaration *list,
													 MemberDeclaration *add){
	MemberDeclaration *pos;
	for (pos = list; pos->next; pos = pos->next)
		;
	pos->next = add;
	return list;
}

static MemberDeclaration *alloc_member_declaration(MemberKind kind, ClassOrMemberModifierList *modifier){
	MemberDeclaration *member = malloc(sizeof(*member));
	member->kind = kind;
	if (modifier) {
		member->access_modifier = conv_access_modifier(modifier->access_modifier);
	}else{
		member->access_modifier = DVM_FILE_MODIFIER;
	}
	member->line_number = mgc_get_current_compiler()->current_line_number;
	member->next = NULL;
	return member;
}

MemberDeclaration *mgc_create_method_member(ClassOrMemberModifierList *modifier,
											FunctionDefinition *function_definition,
											DVM_Boolean is_constructor){
	MemberDeclaration *member = alloc_member_declaration(METHOD_MEMBER, modifier);
	member->u.method.is_constructor = is_constructor;
	member->u.method.is_abstract = DVM_FALSE;
	member->u.method.is_virtual = DVM_FALSE;
	member->u.method.is_override = DVM_FALSE;
	
	if (modifier) {
		if (modifier->id_abstract == ABSTRACT_MODIFIER) {
			member->u.method.is_abstract = DVM_TRUE;
		}
		
		if (modifier->is_virtual == VIRTUAL_MODIFIER) {
			member->u.method.is_virtual = DVM_TRUE;
		}
		
		if (modifier->is_override == OVERRIDE_MODIFIER) {
			member->u.method.is_override = DVM_TRUE;
		}
	}
	
	MGC_Compiler *compiler = mgc_get_current_compiler();
	if (compiler->current_class_definition->class_or_interface == DVM_INTERFACE_DEFINITION) {
		member->u.method.is_virtual = DVM_TRUE;
		member->u.method.is_abstract = DVM_TRUE;
	}
	
	member->u.method.function_definition = function_definition;
	if (member->u.method.is_abstract) {
		if (function_definition->block) {
			mgc_compile_error(compiler->current_line_number, ABSTRACT_METHOD_HAS_BODY_ERR,MESSAGE_ARGUMENT_END);
		}
	}else{
		if (function_definition->block == NULL) {
			mgc_compile_error(compiler->current_line_number, CONCRETE_METHOD_HAS_NO_BODY_ERR);
		}
	
	}
	
	function_definition->class_definition = compiler->current_class_definition;
	return member;
	
}

FunctionDefinition *mgc_method_function_definition(TypeSpecifier *type,
												   char *identifier,
												   ParameterList *parameter,
												   ExceptionList *thorws,
												   Block *block){
	
	FunctionDefinition *fd = mgc_create_function_definition(type, identifier, parameter, thorws, block);
	return fd;
}


FunctionDefinition *mgc_constructor_function_definition(char *identifier,
														ParameterList *parameter,
														ExceptionList *throws,
														Block *block){
	TypeSpecifier *type = mgc_alloc_type_specifier(DVM_VOID_TYPE);
	FunctionDefinition *fd =mgc_create_function_definition(type, identifier, parameter, throws, block);
	return fd;

}

MemberDeclaration *mgc_create_field_member(ClassOrMemberModifierList *modifier,
										   DVM_Boolean *is_final,
										   TypeSpecifier *type,
										   char *name,
										   Expression *initializer){
	MemberDeclaration *member = alloc_member_declaration(FIELD_MEMBER, modifier);
	member->u.field.is_final = is_final;
	member->u.field.type = type;
	member->u.field.name = name;
	member->u.field.initializer = initializer;
	return member;
}

ExceptionList *mgc_create_thorws(char *identifer){
	ExceptionList *list = malloc(sizeof(*list));
	list->exception = malloc(sizeof(ExceptionRef));
	list->exception->identifer = identifer;
	list->exception->class_definition  = NULL;
	list->exception->line_number = mgc_get_current_compiler()->current_line_number;
	list->next = NULL;
	return list;
}


ExceptionList *mgc_chain_exception_list(ExceptionList *list, char *identifier){
	ExceptionList *add = mgc_create_thorws(identifier);
	if (!list) {
		return add;
	}
	ExceptionList *pos;
	for (pos = list; pos ->next; pos = pos->next)
		;
	pos->next = add;
	return list;

}

void mgc_create_delegate_definition(TypeSpecifier *type, char *identifier,
									ParameterList *parameter, ExceptionList *thorws){
	DelegateDefinition *delegate = malloc(sizeof(*delegate));
	delegate->type = type;
	delegate->name = identifier;
	delegate->parameter_list = parameter;
	delegate->throws = thorws;
	delegate->next = NULL;
	
	MGC_Compiler *compiler = mgc_get_current_compiler();
	if (compiler->delegate_definition_list) {
		DelegateDefinition *pos;
		for (pos = compiler->delegate_definition_list; pos->next; pos = pos->next)
			;
		pos->next = delegate;
	}else{
		compiler->delegate_definition_list = delegate;
	}
}


void mgc_create_enum_definition(char *identifier, Enumerator *enumerator){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	EnumDefinition *enum_definition = malloc(sizeof(*enum_definition));
	enum_definition->package_name = compiler->package_name;
	enum_definition->name = identifier;
	enum_definition->next = NULL;
	enum_definition->enumerator = enumerator;
	
	int value = 0;
	for (Enumerator *enumerator_pos = enumerator; enumerator_pos->next; enumerator_pos = enumerator_pos->next) {
		enumerator_pos->value = value;
		value++;
	}
	
	
	if (compiler->enum_definition_list) {
		EnumDefinition *pos;
		for (pos = compiler->enum_definition_list; pos->next; pos = pos->next)
			;
		pos->next = enum_definition;
	}else{
		compiler->enum_definition_list = enum_definition;
	}
	
}

Enumerator *mgc_create_enumerator(char *identifier){
	Enumerator *enumerator = malloc(sizeof(*enumerator));
	enumerator->name = identifier;
	enumerator->value = UNDEFINED_ENUMERATOR;
	enumerator->next = NULL;
	return enumerator;
}

Enumerator *mgc_chain_enumerator(Enumerator *enumerator, char *identifier){
	Enumerator *add = mgc_create_enumerator(identifier);
	if (!enumerator) {
		return add;
	}
	Enumerator *pos;
	for (pos = enumerator; pos->next; pos = pos->next)
		;
	pos->next = add;
	return enumerator;
}

void mgc_create_const_definition(TypeSpecifier *type,
								  char *identifier,
								  Expression *initializer){


}





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
