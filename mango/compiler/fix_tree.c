//
//  fix_tree.c
//  mango
//
//  Created by jerry.yong on 2017/6/26.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mangoc.h"
#include "share.h"
#include "MEM.h"
#include "DBG.h"

static size_t reserve_function_index(MGC_Compiler *compiler, FunctionDefinition *src){
	if (src->class_definition && src->block == NULL) {
		return ABSTRACT_METHOD_INDEX;
	}
	char *package_name = mgc_package_name_to_string(src->package_name);
	for (size_t i = 0; i < compiler->dvm_function_count; i++) {
		if (dvm_equal_package_name(package_name, compiler->dvm_function[i].package_name)
			&& !strcmp(src->name, compiler->dvm_function[i].name)) {
			MEM_free(package_name);
			return i;
		}
	}
	
	compiler->dvm_function = MEM_realloc(compiler->dvm_function, sizeof(DVM_Function) * (compiler->dvm_function_count + 1));
	DVM_Function *dest = &compiler->dvm_function[compiler->dvm_function_count];
	compiler->dvm_function_count++;
	dest->package_name = package_name;
	if (src->class_definition) {
		dest->name = dvm_create_method_function_name(src->class_definition->name, src->name);
	}else{
		dest->name = MEM_strdup(src->name);
	}
	
	return compiler->dvm_function_count - 1;
}

static size_t reserve_enum_index(MGC_Compiler *compiler, EnumDefinition *src, DVM_Boolean is_defined){
	DVM_Enum *dest;
	size_t i;
	char *package_name = mgc_package_name_to_string(src->package_name);
	for (i = 0; i < compiler->dvm_enum_count; i++) {
		if (dvm_equal_package_name(package_name, compiler->dvm_enum[i].package_name)
			&& !strcmp(src->name, compiler->dvm_enum[i].name)) {
			MEM_free(package_name);
			dest = &compiler->dvm_enum[i];
			if (!is_defined || dest->is_defined) {
				return i;
			}else{
				break;
			}
		}
	}
	
	size_t dest_idx;
	if (i == compiler->dvm_enum_count) {
		compiler->dvm_enum = MEM_realloc(compiler->dvm_enum, sizeof(DVM_Enum) * (compiler->dvm_enum_count + 1));
		dest_idx = compiler->dvm_enum_count;
		dest = &compiler->dvm_enum[compiler->dvm_enum_count];
		compiler->dvm_enum_count++;
		dest->package_name = package_name;
		dest->name = MEM_strdup(src->name);
		
	}else{
		dest = &compiler->dvm_enum[i];
		dest_idx = i;
	}
	
	dest->is_defined = is_defined;
	if (is_defined) {
		size_t enumerator_count = 0;
		for (Enumerator *pos = src->enumerator; pos; pos = pos->next) {
			enumerator_count++;
		}
		dest->enumerator_count = enumerator_count;
		dest->enumerator = MEM_malloc(sizeof(char *) * enumerator_count);
		
		i = 0;
		for (Enumerator *pos = src->enumerator; pos; pos = pos->next,i++) {
			dest->enumerator[i] = MEM_strdup(pos->name);
		}
	}
	
	return dest_idx;
}


static size_t reserve_constant_idnex(MGC_Compiler *compiler, ConstantDefinition *src, DVM_Boolean is_define){
	char *package_name = mgc_package_name_to_string(src->package_name);
	for (size_t i = 0; i < compiler->dvm_constant_count; i++) {
		if (dvm_equal_package_name(package_name, compiler->dvm_constant[i].package_name)
			&& !strcmp(src->name, compiler->dvm_constant[i].name)) {
			MEM_free(package_name);
			return i;
		}
		
	}
	
	compiler->dvm_constant = MEM_realloc(compiler->dvm_constant, sizeof(DVM_Constant) * (compiler->dvm_constant_count + 1));
	
	DVM_Constant *dest = &compiler->dvm_constant[compiler->dvm_constant_count];
	compiler->dvm_constant_count++;
	dest->package_name = package_name;
	dest->name = MEM_strdup(src->name);
	dest->is_defined = is_define;
	
	return compiler->dvm_constant_count - 1;

}



static ClassDefinition *search_class_and_add(int line_number, char *name, size_t *index);


static size_t add_class(ClassDefinition *src){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	char *package_name = mgc_package_name_to_string(src->package_name);
	for (size_t i = 0; i < compiler->dvm_class_count; i++) {
		if (dvm_equal_package_name(package_name, compiler->dvm_class[i].package_name)
			&& !strcmp(src->name, compiler->dvm_class[i].name)) {
			MEM_free(package_name);
			return i;
		}
	}
	compiler->dvm_class = MEM_realloc(compiler->dvm_class, sizeof(DVM_Class) * (compiler->dvm_class_count + 1));
	DVM_Class *dest = &compiler->dvm_class[compiler->dvm_class_count];
	compiler->dvm_class_count++;
	dest->package_name = package_name;
	dest->name = MEM_strdup(src->name);
	dest->is_implemented = DVM_FALSE;
	
	for (ExtendsList *pos = src->extends; pos; pos = pos->next) {
		search_class_and_add(src->line_number, pos->identifer, NULL);
	}
	return compiler->dvm_class_count - 1;

}

static DVM_Boolean check_type_compatibility(TypeSpecifier *super_type, TypeSpecifier *sub_type);

static DVM_Boolean is_super_interface(ClassDefinition *child, ClassDefinition *parent, size_t *index){
	size_t i =0;
	for (ExtendsList *pos = child->interface_list; pos; pos = pos->next) {
		if (pos->class_definition == parent) {
			if (index) {
				*index = i;
			}
			return DVM_TRUE;
		}
		i++;
	}
	
	return DVM_FALSE;

}


static DVM_Boolean is_super_class(ClassDefinition *child, ClassDefinition *parent, DVM_Boolean *is_interface, size_t *interface_index){
	for (ClassDefinition *pos = child->super_class; pos; pos = pos->next) {
		if (pos == parent) {
			if (is_interface) {
				*is_interface = DVM_FALSE;
			}
			
			return DVM_TRUE;
		}
	}
	
	DVM_Boolean b = is_super_interface(child, parent, interface_index);
	if (is_interface) {
		*is_interface = b;
	}
	return b;
}

static DVM_Boolean is_exception_class(ClassDefinition *cd){
	
	ClassDefinition *excepion_class = mgc_search_class(EXCEPTION_CLASS_NAME);
	
	if (excepion_class == cd || is_super_class(cd, excepion_class, NULL, NULL)) {
		return DVM_TRUE;
	}
	return DVM_FALSE;

}

static void fix_type_specifier(TypeSpecifier *type);

static void fix_parameter_list(ParameterList *parameter_list){
	for (ParameterList *pos = parameter_list; pos; pos = pos->next) {
		fix_type_specifier(pos->type);
	}
}

static void fix_thorws(ExceptionList *throws){
	for (ExceptionList *pos = throws; pos; pos = pos->next) {
		pos->exception->class_definition = mgc_search_class(pos->exception->identifer);
		if (pos->exception->class_definition == NULL) {
			mgc_compile_error(pos->exception->line_number, THROWS_TYPE_NOT_FOUND_ERR,STRING_MESSAGE_ARGUMENT,"name",pos->exception->identifer,MESSAGE_ARGUMENT_END);
		}
		
		if (!is_exception_class(pos->exception->class_definition)) {
			mgc_compile_error(pos->exception->line_number,THROW_TYPE_IS_NOT_EXCEPTION_ERR, STRING_MESSAGE_ARGUMENT,"name",pos->exception->identifer,MESSAGE_ARGUMENT_END);
		}
		
	}

}


static void fix_type_specifier(TypeSpecifier *type){
	for (TypeDerive *pos = type->derive; pos; pos = pos->next) {
		if (pos->tag == FUNCTION_DERIVE) {
			fix_parameter_list(pos->u.function_d.parameter_list);
			fix_thorws(pos->u.function_d.throws);
		}
	}
	
	MGC_Compiler *compiler = mgc_get_current_compiler();
	if (type->base_type == DVM_UNSPECIFIED_IDENTIFIER_TYPE) {
		ClassDefinition *cd = mgc_search_class(type->identifier);
		if (cd) {
			if (!mgc_equal_package_name(cd->package_name, compiler->package_name)
				&& cd->access_modifier != DVM_PUBLIC_MODIFIER) {
				mgc_compile_error(type->line_number, PACKAGE_CLASS_ACCESS_ERR,STRING_MESSAGE_ARGUMENT,"class_name",cd->name,MESSAGE_ARGUMENT_END);
			}
			type->base_type = DVM_CLASS_TYPE;
			type->u.class_ref.class_definition = cd;
			type->u.class_ref.class_index = add_class(cd);
			return;
		}
		
		DelegateDefinition *dd = mgc_search_delegate(type->identifier);
		if (dd) {
			type->base_type = DVM_DELEGAET_TYPE;
			type->u.delegate_ref.delegate_definition = dd;
			return;
		}
		
		EnumDefinition *ed = mgc_search_enum(type->identifier);
		if (ed) {
			type->base_type = DVM_ENUM_TYPE;
			type->u.enum_ref.enum_definition = ed;
			type->u.enum_ref.enum_index = reserve_enum_index(compiler, ed, DVM_FALSE);
			return;
		}
		
		mgc_compile_error(type->line_number, TYPE_NAME_NOT_FOUND_ERR,STRING_MESSAGE_ARGUMENT,"name",type->identifier,MESSAGE_ARGUMENT_END);
	}

}

static TypeSpecifier *create_function_derive_type(FunctionDefinition *fd){
	TypeSpecifier *ret = mgc_alloc_type_specifier(fd->type->base_type);
	*ret = *fd->type;
	ret->derive= mgc_alloc_type_derive(FUNCTION_DERIVE);
	ret->derive->u.function_d.parameter_list = fd->parameter_list;
	ret->derive->u.function_d.throws = fd->throws;
	ret->derive->next = fd->type->derive;
	return ret;
}

static Expression *fix_identifier_expression(Block *current_block, Expression *expr){
	
	MGC_Compiler *compiler = mgc_get_current_compiler();
	
	Declaration *decl = mgc_search_declaration(expr->u.identifer_express.name, current_block);
	if (decl) {
		expr->type = decl->type;
		expr->u.identifer_express.kind = VARIABLE_IDENTIFER;
		expr->u.identifer_express.u.declaration = decl;
		return expr;
	}
	
	
	ConstantDefinition *cd = mgc_search_constant(expr->u.identifer_express.name);
	if (cd) {
		expr->type = cd->type;
		expr->u.identifer_express.kind = CONSTANT_IDENTIFER;
		expr->u.identifer_express.u.constant.constant_definition = cd;
		expr->u.identifer_express.u.constant.constant_index = reserve_constant_idnex(compiler, cd, DVM_FALSE);
		return expr;
	}
	
	FunctionDefinition *fd = mgc_search_function(expr->u.identifer_express.name);
	
	if (fd) {
		expr->type = create_function_derive_type(fd);
		expr->u.identifer_express.kind = FUNCTION_IDENTIFER;
		expr->u.identifer_express.u.function.function_definition = fd;
		expr->u.identifer_express.u.function.function_index = reserve_function_index(compiler, fd);
		return expr;
	}
	
	mgc_compile_error(expr->line_number, IDENTIFIER_NOT_FOUND_ERR,STRING_MESSAGE_ARGUMENT,"name",expr->u.identifer_express.name,MESSAGE_ARGUMENT_END);
	return NULL;

}


static Expression *fix_expression(Block *current_block, Expression *expr, Expression *parent, ExceptionList **el_p);


static Expression *fix_comma_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.comma_expression.left = fix_expression(current_block, expr->u.comma_expression.left, expr, el_p);
	expr->u.comma_expression.right = fix_expression(current_block, expr->u.comma_expression.right, expr, el_p);
	expr->type = expr->u.comma_expression.right->type;
	return expr;
}

static void cast_mismatch_error(int line_number, TypeSpecifier *src, TypeSpecifier *dest){
	char *temp = mgc_get_type_name(src);
	char *src_name = MEM_strdup(temp);
	MEM_free(temp);
	
	temp = mgc_get_type_name(dest);
	char *dest_name = MEM_strdup(temp);
	MEM_free(temp);
	
	mgc_compile_error(line_number, CAST_MISMATCH_ERR,
					  STRING_MESSAGE_ARGUMENT,"src",src_name,
					  STRING_MESSAGE_ARGUMENT,"dest",dest_name,
					  MESSAGE_ARGUMENT_END);
	
}

static Expression *alloc_cast_expression(CastType cast_type,Expression *operand){
	Expression *cast_expr = mgc_alloc_expression(CAST_EXPRESSION);
	cast_expr->line_number = operand->line_number;
	cast_expr->u.cast.operand = operand;
	cast_expr->u.cast.type = cast_type;
	
	if (cast_type == INT_TO_DOUBLE_CAST) {
		cast_expr->type = mgc_alloc_type_specifier(DVM_DOUBLE_TYPE);
	}else if (cast_type == DOUBLE_TO_INT_CAST){
		cast_expr->type = mgc_alloc_type_specifier(DVM_INT_TYPE);
	}else if (cast_type == BOOLEAN_TO_STRING_CAST
			  || cast_type == INT_TO_STRING_CAST
			  || cast_type == DOUBLE_TO_STRING_CAST
			  || cast_type == ENUM_TO_STRING_CAST){
		cast_expr->type = mgc_alloc_type_specifier(DVM_STRING_TYPE);
	}else{
		DBG_assert(cast_type == FUNCTION_TO_DELEGATE_CAST, "cast_type..%d\n",cast_type);
		
	}
	return cast_expr;
}

static Expression *create_up_cast(Expression *src, ClassDefinition *dest_interface, size_t interface_index){
	TypeSpecifier *type = mgc_alloc_type_specifier(DVM_CLASS_TYPE);
	type->identifier = dest_interface->name;
	type->u.class_ref.class_definition = dest_interface;
	type->u.class_ref.class_index = interface_index;
	
	Expression *cast_expr = mgc_alloc_expression(UP_CAST_EXPRESSION);
	cast_expr->type = type;
	cast_expr->u.up_cast.interface_definition = dest_interface;
	cast_expr->u.up_cast.interface_index = interface_index;
	cast_expr->u.up_cast.operand = src;
	return cast_expr;
}

static DVM_Boolean check_throws(ExceptionList *wide, ExceptionList *narrow, ExceptionList **error_exception){
	ExceptionList *narrow_pos;
	DVM_Boolean is_thrown = DVM_FALSE;
	for (narrow_pos = narrow; narrow_pos; narrow_pos = narrow_pos->next) {
		is_thrown = DVM_FALSE;
		for (ExceptionList *wide_pos = wide; wide_pos; wide_pos = wide_pos->next) {
			ClassDefinition *narrow_cd = narrow_pos->exception->class_definition;
			ClassDefinition *wide_cd = wide_pos->exception->class_definition;
			if (narrow_cd == wide_cd || is_super_class(narrow_cd, wide_cd, NULL, NULL)) {
				is_thrown = DVM_TRUE;
				break;
			}
		}
		
		if (!is_thrown) {
			break;
		}
	}
	
	if (!is_thrown) {
		*error_exception = narrow_pos;
	}
	
	return is_thrown;


}


static void check_func_compati_sub(int line_number, char *name,
								   TypeSpecifier *type1, ParameterList *param1, ExceptionList *throws1,
								   TypeSpecifier *type2, ParameterList *param2, ExceptionList *throws2){
	ParameterList *param_pos1, *param_pos2;
	size_t param_index = 1;
	for (param_pos1 = param1, param_pos2 = param2; param_pos1 && param_pos2; param_pos1 = param_pos1->next,param_pos2 = param_pos2->next) {
		if (!check_type_compatibility(param_pos2->type, param_pos1->type)) {
			mgc_compile_error(line_number, BAD_PARAMETER_TYPE_ERR,
							  STRING_MESSAGE_ARGUMENT,"func_name",name,
							  INT_MESSAGE_ARGUMENT,"index",param_index,
							  STRING_MESSAGE_ARGUMENT,"param_name",param_pos2->name,
							  MESSAGE_ARGUMENT_END);
		}
		param_index++;
	}
	
	if (param_pos1 != NULL || param_pos2 != NULL) {
		mgc_compile_error(line_number, BAD_PARAMETER_COUNT_ERR,
						  STRING_MESSAGE_ARGUMENT,"name",name,
						  MESSAGE_ARGUMENT_END);
	}
	
	if (!check_type_compatibility(type1, type2)) {
		mgc_compile_error(line_number, BAD_RETURN_TYPE_ERR,
						  STRING_MESSAGE_ARGUMENT,"name",name,
						  MESSAGE_ARGUMENT_END);
	}
	
	ExceptionList *error_exception = NULL;
	if (!check_throws(throws1, throws2, &error_exception)) {
		mgc_compile_error(line_number, BAD_EXCEPTION_ERR,
						  STRING_MESSAGE_ARGUMENT,"func_name",name,
						  STRING_MESSAGE_ARGUMENT,"exception_name",error_exception->exception->identifer,
						  MESSAGE_ARGUMENT_END);
	}
	

}

static void check_func_compatibility(FunctionDefinition *fd1, FunctionDefinition *fd2){
	check_func_compati_sub(fd2->end_line_number, fd2->name, fd1->type, fd1->parameter_list, fd1->throws, fd2->type, fd2->parameter_list, fd2->throws);
}

static Expression *create_to_string_cast(Expression *src){
	Expression *cast = NULL;
	if (mgc_is_boolean(src->type)) {
		cast = alloc_cast_expression(BOOLEAN_TO_STRING_CAST, src);
	}else if (mgc_is_int(src->type)){
		cast = alloc_cast_expression(INT_TO_STRING_CAST, src);
	}else if (mgc_is_double(src->type)){
		cast = alloc_cast_expression(DOUBLE_TO_STRING_CAST, src);
	}else if (mgc_is_enum(src->type)){
		cast = alloc_cast_expression(ENUM_TO_STRING_CAST, src);
	}
	return cast;
}


static Expression *create_assign_cast(Expression *src, TypeSpecifier *dest){
	if (mgc_equal_type(src->type, dest)) {
		return src;
	}
	
	if (mgc_is_object(dest) && src->type->base_type == DVM_NULL_TYPE) {
		DBG_assert(src->type->derive == NULL, "derive != NULL");
	}
	
	Expression *cast_expr = NULL;
	
	if (mgc_is_class_object(src->type) && mgc_is_class_object(dest)) {
		DVM_Boolean is_interface;
		size_t interface_index;
		if (is_super_class(src->type->u.class_ref.class_definition, dest->u.class_ref.class_definition, &is_interface, &interface_index)) {
			if (is_interface) {
				 cast_expr = create_up_cast(src, dest->u.class_ref.class_definition, interface_index);
				return cast_expr;
			}
			return src;
		}
	}else{
		cast_mismatch_error(src->line_number, src->type, dest);
	}
	
	if (mgc_is_function(src->type) && mgc_is_delegate(dest)) {
		TypeSpecifier *delegate_type = mgc_alloc_type_specifier(dest->base_type);
		*delegate_type = *src->type;
		delegate_type->derive = src->type->derive->next;
		
		DelegateDefinition *delegate_define = dest->u.delegate_ref.delegate_definition;
		
		
		check_func_compati_sub(src->line_number, "", delegate_define->type, delegate_define->parameter_list, delegate_define->throws, delegate_type, src->type->derive->u.function_d.parameter_list, src->type->derive->u.function_d.throws);
		
		cast_expr = alloc_cast_expression(FUNCTION_TO_DELEGATE_CAST, src);
		cast_expr->type = delegate_type;
		return cast_expr;
		
	}
	
	if (mgc_is_int(src->type) && mgc_is_double(dest)) {
		cast_expr = alloc_cast_expression(INT_TO_DOUBLE_CAST, src);
		return cast_expr;
	}
	
	if (mgc_is_double(src->type) && mgc_is_int(dest)) {
		cast_expr = alloc_cast_expression(DOUBLE_TO_INT_CAST, src);
		return cast_expr;
	}
	
	if (mgc_is_string(dest)) {
		cast_expr = create_to_string_cast(src);
		if (cast_expr) {
			return cast_expr;
		}
	}
	
	cast_mismatch_error(src->line_number, src->type, dest);
	
	return NULL;	
	

}

static Expression *fix_assign_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.assignment_expression.left = fix_expression(current_block, expr->u.assignment_expression.left, expr, el_p);
	Expression *left = expr->u.assignment_expression.left;
	if (left->kind == IDENTIFIER_EXPRESSION && left->u.identifer_express.kind == VARIABLE_IDENTIFER) {
		if (left->u.identifer_express.u.declaration->is_final) {
			mgc_compile_error(expr->line_number, FINAL_VARIABLE_ASSIGNMENT_ERR, STRING_MESSAGE_ARGUMENT, "name", left->u.identifer_express.name, MESSAGE_ARGUMENT_END);;
		}
		
	}else if (left->kind == MEMBER_EXPRESSION && left->u.member_expression.declaration->kind == FIELD_MEMBER){
		if (left->u.member_expression.declaration->u.field.is_final) {
			mgc_compile_error(expr->line_number, FINAL_FIELD_ASSIGNMENT_ERR, STRING_MESSAGE_ARGUMENT, "name", left->u.member_expression.member_name, MESSAGE_ARGUMENT_END);
		}
	
	}else if(left->kind != INDEX_EXPRESSION){
		mgc_compile_error(expr->line_number, NOT_LVALUE_ERR, MESSAGE_ARGUMENT_END);
	}
	
	Expression *operand= expr->u.assignment_expression.operand;
	expr->u.assignment_expression.operand = create_assign_cast(operand, left->type);
	expr->type = left->type;
	return expr;
}



static Expression *eval_math_expression_int(Expression *expression, int left, int right){
	switch (expression->kind) {
		case ADD_EXPRESSION:
			expression->u.int_value = left + right;
				break;
		case SUB_EXPRESSION:
			expression->u.int_value = left - right;
			break;
		case MUL_EXPRESSION:
			expression->u.int_value = left * right;
			break;
		case DIV_EXPRESSION:
			if (right == 0) {
				mgc_compile_error(expression->line_number, DIVISION_BY_ZERO_IN_COMPILE_ERR,MESSAGE_ARGUMENT_END);
			}
			expression->u.int_value = left / right;
			break;
		case MOD_EXPRESSION:
			expression->u.int_value = left % right;
			break;
	  default:
			DBG_assert(0,"expression->kind ..%d\n", expression->kind);
			break;
			
	}
	expression->kind = INT_EXPRESSION;
	expression->type = mgc_alloc_type_specifier(DVM_INT_TYPE);
	return expression;

}

static Expression *eval_math_expression_double(Expression *expression, double left, double right){
	switch (expression->kind) {
		case ADD_EXPRESSION:
			expression->u.double_value = left + right;
			break;
		case SUB_EXPRESSION:
			expression->u.double_value = left - right;
			break;
		case MUL_EXPRESSION:
			expression->u.double_value = left * right;
			break;
		case DIV_EXPRESSION:
			if (right == 0) {
				mgc_compile_error(expression->line_number, DIVISION_BY_ZERO_IN_COMPILE_ERR,MESSAGE_ARGUMENT_END);
			}
			expression->u.double_value = left / right;
	  default:
			DBG_assert(0,"expression->kind ..%d\n", expression->kind);
			break;
	}
	expression->kind = DOUBLE_EXPRESSION;
	expression->type = mgc_alloc_type_specifier(DVM_DOUBLE_TYPE);
	return expression;
}


static Expression *chain_string(Expression *expr){
	DVM_Char *left = expr->u.binary_expression.left->u.string_value;
	DVM_Char *right = mgc_expression_to_string(expr->u.binary_expression.right);
	if (!right) {
		return expr;
	}
	
	size_t new_str_len = dvm_wcslen(left) + dvm_wcslen(right);
	DVM_Char *new_str = MEM_malloc(sizeof(DVM_Char) * (new_str_len + 1));
	dvm_wcscpy(new_str, left);
	dvm_wcscat(new_str, right);
	MEM_free(left);
	MEM_free(right);
	
	
	expr->kind = STRING_EXPRESSION;
	expr->type = mgc_alloc_type_specifier(DVM_STRING_TYPE);
	expr->u.string_value = new_str;
	return expr;
}


static Expression *eval_math_expression(Block *current_block, Expression *expr){
	if (expr->u.binary_expression.left->kind == INT_EXPRESSION
		&& expr->u.binary_expression.left->kind == INT_EXPRESSION) {
		expr = eval_math_expression_int(expr, expr->u.binary_expression.left->u.int_value, expr->u.binary_expression.right->u.int_value);
	}
	
	if (expr->u.binary_expression.left->kind == DOUBLE_EXPRESSION
		&& expr->u.binary_expression.left->kind == DOUBLE_EXPRESSION) {
		expr = eval_math_expression_double(expr, expr->u.binary_expression.left->u.double_value, expr->u.binary_expression.right->u.double_value);
	}
	
	
	if (expr->u.binary_expression.left->kind == INT_EXPRESSION
		&& expr->u.binary_expression.left->kind == DOUBLE_EXPRESSION) {
		expr = eval_math_expression_double(expr, expr->u.binary_expression.left->u.int_value, expr->u.binary_expression.right->u.double_value);
	}
	
	
	if (expr->u.binary_expression.left->kind == DOUBLE_EXPRESSION
		&& expr->u.binary_expression.left->kind == INT_EXPRESSION) {
		expr = eval_math_expression_double(expr, expr->u.binary_expression.left->u.double_value, expr->u.binary_expression.right->u.int_value);
	}
	
	if (expr->u.binary_expression.left->kind == STRING_EXPRESSION && expr->kind == ADD_EXPRESSION) {
	   expr	= chain_string(expr);
	}
	
	return expr;
	
}

static Expression *cast_binary_expression(Expression *expr){
	Expression *cast_expr = NULL;
	if (mgc_is_int(expr->u.binary_expression.left->type) && mgc_is_double(expr->u.binary_expression.right->type)) {
	    cast_expr = alloc_cast_expression(INT_TO_DOUBLE_CAST, expr->u.binary_expression.left);
		expr->u.binary_expression.left = cast_expr;
	}else if(mgc_is_double(expr->u.binary_expression.left->type) && mgc_is_int(expr->u.binary_expression.right->type)){
		cast_expr = alloc_cast_expression(INT_TO_DOUBLE_CAST, expr->u.binary_expression.right);
		expr->u.binary_expression.right = cast_expr;
	}else if (mgc_is_string(expr->u.binary_expression.left->type) && expr->kind == ADD_EXPRESSION){
		cast_expr = create_to_string_cast(expr->u.binary_expression.right);
		if (cast_expr) {
			expr->u.binary_expression.right = cast_expr;
		}
	}
	return expr;
}

static Expression *fix_math_binary_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.binary_expression.left = fix_expression(current_block, expr->u.binary_expression.left, expr, el_p);
	expr->u.binary_expression.right = fix_expression(current_block, expr->u.binary_expression.right, expr, el_p);
	
	expr = eval_math_expression(current_block, expr);
	if (expr->kind == INT_EXPRESSION || expr->kind == DOUBLE_EXPRESSION || expr->kind == STRING_EXPRESSION) {
		return expr;
	}
	
	expr = cast_binary_expression(expr);
	
	if (mgc_is_int(expr->u.binary_expression.left->type) && mgc_is_int(expr->u.binary_expression.right->type)) {
		expr->type = mgc_alloc_type_specifier(DVM_INT_TYPE);
	}else if (mgc_is_double(expr->u.binary_expression.left->type) && mgc_is_double(expr->u.binary_expression.right->type)){
		expr->type = mgc_alloc_type_specifier(DVM_DOUBLE_TYPE);
	}else if (expr->kind == ADD_EXPRESSION
			  && ((mgc_is_string(expr->u.binary_expression.left->type) && mgc_is_string(expr->u.binary_expression.right->type))
			   || (mgc_is_string(expr->u.binary_expression.left->type) && expr->u.binary_expression.right->kind == NULL_EXPRESSION))){
				  expr->type = mgc_alloc_type_specifier(DVM_STRING_TYPE);
	
	}else{
		mgc_compile_error(expr->line_number, MATH_TYPE_MISMATCH_ERR,MESSAGE_ARGUMENT_END);
			  
	}
	return expr;
}

static int eval_bit_binary_expression(ExpressionKind kind, int left, int right){
	if (kind == BIT_AND_EXPRESSION) {
		return left & right;
	}else if (kind == BIT_OR_EXPRESSION){
		return left | right;
	}else{
		DBG_assert(kind == BIT_XOR_EXPRESSION, "kind..%d",kind);
		return left ^ right;
	}
}

static Expression *fix_bit_binary_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.binary_expression.left = fix_expression(current_block, expr->u.binary_expression.left, expr, el_p);
	expr->u.binary_expression.right = fix_expression(current_block, expr->u.binary_expression.right, expr, el_p);
	
	if (!((mgc_is_int(expr->u.binary_expression.left->type) &&(mgc_is_int(expr->u.binary_expression.right->type)))
	   || (mgc_is_boolean(expr->u.binary_expression.left->type) && (mgc_is_boolean(expr->u.binary_expression.right->type))))) {
		mgc_compile_error(expr->line_number, BIT_BINARY_OPERATOR_TYPE_MISMATCH_ERR,MESSAGE_ARGUMENT_END);
	}
	expr->type = expr->u.binary_expression.left->type;
	
	if (expr->u.binary_expression.left->kind == INT_EXPRESSION && expr->u.binary_expression.right->kind == INT_EXPRESSION) {
		expr->u.int_value = eval_bit_binary_expression(expr->kind, expr->u.binary_expression.left->u.int_value, expr->u.binary_expression.right->u.int_value);
		expr->kind = INT_EXPRESSION;
	}else{
		expr->u.boolean_value = eval_bit_binary_expression(expr->kind, expr->u.binary_expression.left->u.boolean_value, expr->u.binary_expression.right->u.boolean_value);
		expr->kind = BOOLEAN_EXPRESSION;
	}
	return expr;
}


static Expression *eval_compare_expression_boolean(Expression *expr, DVM_Boolean left, DVM_Boolean right){
	if (expr->kind == EQ_EXPRESSION) {
		expr->u.boolean_value = left == right;
	}else{
		DBG_assert(expr->kind == NE_EXPRESSION, "kind..%d",expr->kind);
		expr->u.boolean_value = left != right;
	}
	expr->kind = BOOLEAN_EXPRESSION;
	expr->type = mgc_alloc_type_specifier(DVM_BOOLEAN_TYPE);
	return expr;
}

static Expression *eval_compare_expression_double(Expression *expr, double left, double right){
	if (expr->kind == EQ_EXPRESSION) {
		expr->u.boolean_value = left == right;
	}else if (expr->kind == NE_EXPRESSION){
		expr->u.boolean_value = left != right;
	}else if (expr->kind == LT_EXPRESSION){
		expr->u.boolean_value = left < right;
	}else if (expr->kind == LE_EXPRESSION){
		expr->u.boolean_value = left <= right;
	}else if (expr->kind == GT_EXPRESSION){
		expr->u.boolean_value = left > right;
	}else{
		DBG_assert(expr->kind == GE_EXPRESSION,"kind..%d",expr->kind);
		expr->u.boolean_value = left >= right;
	}
	expr->kind = BOOLEAN_EXPRESSION;
	expr->type = mgc_alloc_type_specifier(DVM_BOOLEAN_TYPE);
	return expr;
}

static Expression *eval_compare_expression_string(Expression *expr, DVM_Char *left, DVM_Char *right){
	int cmp = dvm_wcscmp(left, right);
	if (expr->kind == EQ_EXPRESSION) {
		expr->u.boolean_value = cmp == 0;
	}else if (expr->kind == NE_EXPRESSION){
		expr->u.boolean_value = cmp != 0;
	}else if (expr->kind == LT_EXPRESSION){
		expr->u.boolean_value = cmp < 0;
	}else if (expr->kind == LE_EXPRESSION){
		expr->u.boolean_value = cmp <= 0;
	}else if (expr->kind == GT_EXPRESSION){
		expr->u.boolean_value = cmp > 0;
	}else{
		DBG_assert(expr->kind == GE_EXPRESSION,"kind..%d",expr->kind);
		expr->u.boolean_value = cmp >= 0;
	}
	MEM_free(left);
	MEM_free(right);
	expr->kind = BOOLEAN_EXPRESSION;
	expr->type = mgc_alloc_type_specifier(DVM_BOOLEAN_TYPE);
	return expr;

}


static Expression *eval_compare_expression(Expression *expr){
	if (expr->u.binary_expression.left->kind == BOOLEAN_EXPRESSION
		&& expr->u.binary_expression.right->kind == BOOLEAN_EXPRESSION) {
		expr = eval_compare_expression_boolean(expr, expr->u.binary_expression.left->u.boolean_value,
											   expr->u.binary_expression.right->u.boolean_value);
	}else if (expr->u.binary_expression.left->kind == INT_EXPRESSION
			  && expr->u.binary_expression.right->kind == INT_EXPRESSION){
		expr = eval_math_expression_int(expr, expr->u.binary_expression.left->u.int_value,
										expr->u.binary_expression.right->u.int_value);
	}else if (expr->u.binary_expression.left->kind == INT_EXPRESSION
			  && expr->u.binary_expression.left->kind == DOUBLE_EXPRESSION){
		expr = eval_math_expression_double(expr, expr->u.binary_expression.left->u.int_value,
										expr->u.binary_expression.right->u.double_value);
	}else if (expr->u.binary_expression.left->kind == DOUBLE_EXPRESSION
			  && expr->u.binary_expression.left->kind == INT_EXPRESSION){
		expr = eval_math_expression_double(expr, expr->u.binary_expression.left->u.double_value,
										expr->u.binary_expression.right->u.int_value);
	}else if (expr->u.binary_expression.left->kind == DOUBLE_EXPRESSION
			  && expr->u.binary_expression.left->kind == DOUBLE_EXPRESSION){
		expr = eval_math_expression_double(expr, expr->u.binary_expression.left->u.double_value,
										expr->u.binary_expression.right->u.double_value);
	}else if (expr->u.binary_expression.left->kind == STRING_EXPRESSION
			  && expr->u.binary_expression.left->kind == STRING_EXPRESSION){
		expr = eval_compare_expression_string(expr, expr->u.binary_expression.left->u.string_value,
										expr->u.binary_expression.right->u.string_value);
	}else if (expr->u.binary_expression.left->kind == NULL_EXPRESSION
			  && expr->u.binary_expression.left->kind == NULL_EXPRESSION){
		
		expr->kind = BOOLEAN_EXPRESSION;
		expr->u.boolean_value = DVM_TRUE;
		expr->type = mgc_alloc_type_specifier(DVM_BOOLEAN_TYPE);
	
	}
	return expr;

}


static Expression *fix_compare_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.binary_expression.left = fix_expression(current_block, expr->u.binary_expression.left, expr, el_p);
	expr->u.binary_expression.right = fix_expression(current_block, expr->u.binary_expression.right, expr, el_p);
	
	expr = eval_compare_expression(expr);
	
	if (expr->kind == BOOLEAN_EXPRESSION) {
		return expr;
	}
	
	expr = cast_binary_expression(expr);
	
	if (!(mgc_equal_type(expr->u.binary_expression.left->type, expr->u.binary_expression.right->type)
		  || (mgc_is_object(expr->u.binary_expression.left->type) && expr->u.binary_expression.right->kind == NULL_EXPRESSION)
		  || (expr->u.binary_expression.left->kind == NULL_EXPRESSION && mgc_is_object(expr->u.binary_expression.right->type)))) {
		mgc_compile_error(expr->line_number, COMPARE_TYPE_MISMATCH_ERR, MESSAGE_ARGUMENT_END);
	}
	
	expr->type = mgc_alloc_type_specifier(DVM_BOOLEAN_TYPE);
	return expr;
}

Expression *eval_logical_and_or_expression(Expression *expr, DVM_Boolean left, DVM_Boolean right){
	if (expr->kind == LOGICAL_AND_EXPRESSION) {
		expr->u.boolean_value = left && right;
	}else{
		DBG_assert(expr->kind == LOGICAL_OR_EXPRESSION, "kind..%d",expr->kind);
		expr->u.boolean_value = left || right;
	}
	expr->kind = BOOLEAN_EXPRESSION;
	expr->type = mgc_alloc_type_specifier(DVM_BOOLEAN_TYPE);
	return expr;
}


static Expression *fix_logical_and_or_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.binary_expression.left = fix_expression(current_block, expr->u.binary_expression.left, expr, el_p);
	expr->u.binary_expression.right = fix_expression(current_block, expr->u.binary_expression.right, expr, el_p);
	
	if (!(mgc_is_boolean(expr->u.binary_expression.left->type) && mgc_is_boolean(expr->u.binary_expression.right->type))) {
		mgc_compile_error(expr->line_number, LOGICAL_TYPE_MISMATCH_ERR);
	}
	if (expr->u.binary_expression.left->kind == BOOLEAN_EXPRESSION
		&& expr->u.binary_expression.right->kind == BOOLEAN_EXPRESSION ) {
		expr = eval_logical_and_or_expression(expr, expr->u.binary_expression.left->u.boolean_value, expr->u.binary_expression.right->u.boolean_value);
	}else{
		expr->type = mgc_alloc_type_specifier(DVM_BOOLEAN_TYPE);
	}
	return expr;
}

static Expression *fix_minus_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.minus_expression = fix_expression(current_block, expr->u.minus_expression, expr, el_p);
	if (!(mgc_is_int(expr->u.minus_expression->type) || mgc_is_double(expr->u.minus_expression->type))) {
		mgc_compile_error(expr->line_number, MINUS_TYPE_MISMATCH_ERR,MESSAGE_ARGUMENT_END);
	}
	expr->type = expr->u.minus_expression->type;
	if (expr->u.minus_expression->kind == INT_EXPRESSION) {
		expr->kind = INT_EXPRESSION;
		expr->u.int_value = -expr->u.int_value;
	}else if (expr->u.minus_expression->kind == DOUBLE_EXPRESSION){
		expr->kind = DOUBLE_EXPRESSION;
		expr->u.double_value = -expr->u.double_value;
	}
	return expr;
}

static Expression *fix_bit_not_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.bit_not = fix_expression(current_block, expr->u.bit_not, expr, el_p);
	if (!mgc_is_int(expr->u.bit_not->type)) {
		mgc_compile_error(expr->line_number, BIT_NOT_TYPE_MISMATCH_ERR,MESSAGE_ARGUMENT_END);
	}
	expr->type = expr->u.bit_not->type;
	if (expr->u.bit_not->kind == INT_EXPRESSION) {
		expr->kind = INT_EXPRESSION;
		expr->u.int_value = ~expr->u.bit_not->u.int_value;
	}
	return expr;
}


static void check_argument(Block *current_block, int line_number,
						   ParameterList *param_list, ArgumentList *argument_list,
						   ExceptionList **el_p, TypeSpecifier *array_base){
	ParameterList *param_pos = NULL;
	ArgumentList *argument_pos = NULL;
	for (param_pos = param_list, argument_pos = argument_list;
		 param_pos && argument_pos;
		 param_pos = param_pos->next, argument_pos = argument_pos->next) {
		argument_pos->expression = fix_expression(current_block, argument_pos->expression, NULL, el_p);
		TypeSpecifier *type;
		if (param_pos->type->base_type == DVM_BASE_TYPE) {
			DBG_assert(array_base != NULL, "array_base == NULL");
			type = array_base;
		}else{
			type = param_pos->type;
		}
		
		argument_pos->expression = create_assign_cast(argument_pos->expression, type);
		
	}
	
	if (param_pos || argument_pos) {
		mgc_compile_error(line_number, ARGUMENT_COUNT_MISMATCH_ERR, MESSAGE_ARGUMENT_END);
	}

}


static ClassDefinition *search_and_add(int line_number, char *name, size_t *class_index){
	ClassDefinition *cd = mgc_search_class(name);
	if (cd == NULL) {
		mgc_compile_error(line_number, CLASS_NOT_FOUND_ERR,STRING_MESSAGE_ARGUMENT,"name",name,MESSAGE_ARGUMENT_END);
	}
	*class_index = add_class(cd);
	return cd;
}

static DVM_Boolean is_ignoreable_exception(ExceptionList *ex){
	ClassDefinition *exception_class = ex->exception->class_definition;
	ClassDefinition *bug_exception = mgc_search_class(BUG_EXCEPTION_CLASS);
	if (exception_class == bug_exception || is_super_class(exception_class, bug_exception, NULL, NULL)) {
		return DVM_TRUE;
	}
	
	
	ClassDefinition *runtime_exception = mgc_search_class(RUNTIME_EXCEPTION_CLASS);
	if (exception_class == runtime_exception || is_super_class(exception_class, runtime_exception, NULL, NULL)) {
		return DVM_TRUE;
	}
	
	return DVM_FALSE;
}
	
	




static void add_exception(ExceptionList **el_p, ExceptionList  *throws){

}

static void remove_exception(ExceptionList **el_p, ClassDefinition *cached){
	for (ExceptionList *prev = NULL, *pos = *el_p; pos; prev = pos, pos = pos->next) {
		if (pos->exception->class_definition == cached || is_super_class(pos->exception->class_definition, cached, NULL, NULL)) {
			if (prev) {
				prev->next = pos->next;
			}else{
				*el_p = pos->next;
			}
		}
	}
}

static Expression *fix_function_call_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	Expression *function_expr = fix_expression(current_block, expr->u.function_call_expression.function, expr, el_p);
	expr->u.function_call_expression.function = function_expr;
	
	TypeSpecifier array_base;
	TypeSpecifier *array_base_p = NULL;
	
	
	
	FunctionDefinition *fd = NULL;
	DelegateDefinition *dd = NULL;
	MGC_Compiler *compiler = mgc_get_current_compiler();
	if (function_expr->kind == IDENTIFIER_EXPRESSION) {
		fd = mgc_search_function(function_expr->u.identifer_express.name);
	}else if (function_expr->kind == MEMBER_EXPRESSION){
		if (mgc_is_array(function_expr->type)) {
			fd = &compiler->array_method[function_expr->u.member_expression.method_index];
			array_base = *function_expr->u.member_expression.expression->type;
			array_base.derive = array_base.derive->next;
			array_base_p = &array_base;
		}else if (mgc_is_string(function_expr->type)){
			fd = &compiler->string_method[function_expr->u.member_expression.method_index];
		}else{
			if (function_expr->u.member_expression.declaration->kind == FIELD_MEMBER) {
				mgc_compile_error(expr->line_number, FIELD_CAN_NOT_CALL_ERR, STRING_MESSAGE_ARGUMENT, "member_name", function_expr->u.member_expression.member_name, MESSAGE_ARGUMENT_END);
			}
			if (function_expr->u.member_expression.declaration->u.method.is_constructor) {
				Expression *obj = function_expr->u.member_expression.expression;
				if (!(obj->kind != THIS_EXPRESSION) && !(obj->kind != SUPER_EXPRESSION)) {
					mgc_compile_error(expr->line_number, CONSTRUCTOR_CALLED_ERR,STRING_MESSAGE_ARGUMENT, "member_name",function_expr->u.member_expression.member_name,MESSAGE_ARGUMENT_END);
				}
			}
			
			fd = function_expr->u.member_expression.declaration->u.method.function_definition;
		
		}
	}
	
	if (mgc_is_delegate(function_expr->type)) {
		dd = function_expr->type->u.delegate_ref.delegate_definition;
	}
	
	if (fd == NULL && dd == NULL) {
		mgc_compile_error(expr->line_number, FUNCTION_NOT_FOUND_ERR, STRING_MESSAGE_ARGUMENT, "name", function_expr->u.identifer_express.name, MESSAGE_ARGUMENT_END);
	}
	
	TypeSpecifier *func_type;
	ParameterList *param;
	ExceptionList *throws;
	if (fd) {
		func_type = fd->type;
		param = fd->parameter_list;
		throws = fd->throws;
	}else{
		func_type = dd->type;
		param = dd->parameter_list;
		throws = dd->throws;
	}
	
	add_exception(el_p, throws);
	check_argument(current_block, expr->line_number, param, function_expr->u.function_call_expression.argument, el_p, array_base_p);
	
	TypeSpecifier *type = malloc(sizeof(TypeSpecifier));
	*type = *func_type;
	expr->type = type;
	fix_type_specifier(expr->type);
	return expr;
}

static DVM_Boolean is_interface_method(ClassDefinition *cd, MemberDeclaration *method, ClassDefinition **target_interface, size_t *interface_index_out){
	size_t interface_index = 0;
	for (ExtendsList *interface = cd->interface_list; interface; interface = interface->next,interface_index++) {
		for (MemberDeclaration *member = interface->class_definition->member; member; member = member->next) {
			if (member->kind != METHOD_MEMBER) {
				continue;
			}
			if (strcmp(member->u.method.function_definition->name, method->u.method.function_definition->name)) {
				if (interface_index_out) {
					*interface_index_out = interface_index;
				}
				if (target_interface) {
					*target_interface = interface->class_definition;
				}
				return DVM_TRUE;
			}
		}
	}
	return DVM_FALSE;
}

static void check_member_accessibility(int line_number, ClassDefinition *target_class, MemberDeclaration *member, char *member_name){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	if (compiler->current_class_definition == NULL || compiler->current_class_definition != target_class) {
		if (member->access_modifier == DVM_PRIVATE_MODIFIER) {
			mgc_compile_error(line_number, PRIVATE_MEMBER_ACCESS_ERR, STRING_MESSAGE_ARGUMENT, "member_name",member_name);
		}
	}
	
	if (!mgc_equal_package_name(compiler->package_name, target_class->package_name)) {
		if (member->access_modifier != PUBLIC_MODIFIER) {
			mgc_compile_error(line_number, PACKAGE_MEMBER_ACCESS_ERR, STRING_MESSAGE_ARGUMENT, "member_name", member_name);
		}
	}

}

static Expression *fix_class_member_expression(Expression *expr, Expression *obj, char *member_name){
	fix_type_specifier(obj->type);
	MemberDeclaration *member	= mgc_search_member(obj->type->u.class_ref.class_definition, member_name);
	if (member == NULL) {
		mgc_compile_error(expr->line_number, MEMBER_NOT_FOUND_ERR,
						  STRING_MESSAGE_ARGUMENT, "class_name", obj->type->u.class_ref.class_definition->name,
						  STRING_MESSAGE_ARGUMENT, "member_name", expr->u.member_expression.member_name, MESSAGE_ARGUMENT_END);
	}
	
	check_member_accessibility(expr->line_number, obj->type->u.class_ref.class_definition, member, member_name);
	expr->u.member_expression.declaration = member;
	
	if (member->kind == METHOD_MEMBER) {
		ClassDefinition *interface = NULL;
		size_t interface_index = 0;
		expr->type = create_function_derive_type(member->u.method.function_definition);
		if (obj->type->u.class_ref.class_definition->class_or_interface == DVM_CLASS_DEFINITION
			&& is_interface_method(obj->type->u.class_ref.class_definition, member, &interface, &interface_index)) {
			expr->u.member_expression.expression = create_up_cast(expr->u.member_expression.expression, interface, interface_index);
		}
	}else if (member->kind == FIELD_MEMBER){
		if (obj->kind == SUPER_EXPRESSION) {
			mgc_compile_error(expr->line_number, FIELD_OF_SUPER_REFERENCED_ERR, MESSAGE_ARGUMENT_END);
		}
		expr->type = member->u.field.type;
	}
	
	return expr;
}


static Expression *fix_array_mothod_expresion(Expression *expr, Expression *obj, char *member_name){
	size_t i = 0;
	MGC_Compiler *compiler = mgc_get_current_compiler();
	for (; i < compiler->array_method_count; i++) {
		if (!strcmp(compiler->array_method[i].name, member_name)) {
			break;
		}
	}
	
	if (i == compiler->array_method_count) {
		mgc_compile_error(expr->line_number, ARRAY_METHOD_NOT_FOUND_ERR, STRING_MESSAGE_ARGUMENT, "name", member_name, MESSAGE_ARGUMENT_END);
	}
	
	FunctionDefinition *fd = &compiler->array_method[i];
	expr->type = create_function_derive_type(fd);
	expr->u.member_expression.method_index = i;
	return expr;

}

static Expression *fix_string_method_expression(Expression *expr, Expression *obj, char *member_name){
	size_t i = 0;
	MGC_Compiler *compiler = mgc_get_current_compiler();
	for (; i < compiler->string_method_count; i++) {
		if (!strcmp(compiler->string_method[i].name, member_name)) {
			break;
		}
	}
	
	if (i == compiler->string_method_count) {
		mgc_compile_error(expr->line_number, STRING_METHOD_NOT_FOUND_ERR, STRING_MESSAGE_ARGUMENT, "name", member_name, MESSAGE_ARGUMENT_END);
	}
	FunctionDefinition *fd = &compiler->array_method[i];
	expr->type = create_function_derive_type(fd);
	expr->u.member_expression.method_index = i;
	return expr;
}

static Expression *create_first_enumerator(EnumDefinition *ed){
	Expression *expr = mgc_alloc_expression(ENUMERATOR_EXPRESSION);
	expr->type = mgc_alloc_type_specifier(DVM_ENUM_TYPE);
	expr->type->identifier = ed->name;
	expr->type->u.enum_ref.enum_definition = ed;
	expr->type->u.enum_ref.enum_index = reserve_enum_index(mgc_get_current_compiler(), ed, DVM_FALSE);
	expr->u.enumerator.enum_definition = ed;
	expr->u.enumerator.enumerator = ed->enumerator;
	return expr;
}


static Expression *create_enumerator(int line_number, EnumDefinition *ed, char *enumerator_name){
	Expression *expr = create_first_enumerator(ed);
	Enumerator *enumerator = NULL;
	for (enumerator = ed->enumerator; enumerator; enumerator = enumerator->next) {
		if (!strcmp(enumerator->name, enumerator_name)) {
			expr->u.enumerator.enumerator = enumerator;
			break;
		}
	}
	
	if (enumerator == NULL) {
		mgc_compile_error(line_number,ENUMERATOR_NOT_FOUND_ERR,
						  STRING_MESSAGE_ARGUMENT, "type_name", ed->name,
						  STRING_MESSAGE_ARGUMENT, "name", enumerator_name, MESSAGE_ARGUMENT_END);
	}
	
	return expr;

	
}


static Expression *fix_member_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	Expression *obj = expr->u.member_expression.expression;
	EnumDefinition *ed;
	if (obj->kind == IDENTIFIER_EXPRESSION && (ed = mgc_search_enum(obj->u.identifer_express.name))) {
		return create_enumerator(expr->line_number, ed, expr->u.member_expression.member_name);
	}
	
	obj = fix_expression(current_block, obj, expr, el_p);
	expr->u.member_expression.expression = obj;
	
	if (mgc_is_array(obj->type)) {
		fix_array_mothod_expresion(expr, obj, expr->u.member_expression.member_name);
	}else if (mgc_is_string(obj->type)){
		fix_array_mothod_expresion(expr, obj, expr->u.member_expression.member_name);
	}else if (mgc_is_class_object(obj->type)){
		fix_class_member_expression(expr, obj, expr->u.member_expression.member_name);
	}else{
		mgc_compile_error(expr->line_number, MEMBER_EXPRESSION_TYPE_ERR, MESSAGE_ARGUMENT_END);
	}
	return expr;
}

static Expression *fix_this_expression(Expression *expr){
	ClassDefinition *cd = mgc_get_current_compiler()->current_class_definition;
	if (!cd) {
		mgc_compile_error(expr->line_number, THIS_OUT_OF_CLASS_ERR,MESSAGE_ARGUMENT_END);
	}
	TypeSpecifier *type = mgc_alloc_type_specifier(DVM_CLASS_TYPE);
	type->identifier = cd->name;
	type->u.class_ref.class_definition = cd;
	type->u.class_ref.class_index = add_class(cd);
	expr->type = type;
	return expr;
	
}

static Expression *fix_super_expression(Expression *expr, Expression *parent){
	ClassDefinition *cd = mgc_get_current_compiler()->current_class_definition;
	if (!cd) {
		mgc_compile_error(expr->line_number, SUPER_OUT_OF_CLASS_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (!cd->super_class) {
		mgc_compile_error(expr->line_number, HASNOT_SUPER_CLASS_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (!parent || parent->line_number != MEMBER_EXPRESSION) {
		mgc_compile_error(expr->line_number, SUPER_NOT_IN_MEMBER_EXPRESSION_ERR, MESSAGE_ARGUMENT_END);
	}
	
	TypeSpecifier *type = mgc_alloc_type_specifier(DVM_CLASS_TYPE);
	type->identifier = cd->super_class->name;
	type->u.class_ref.class_definition = cd->super_class;
	type->u.class_ref.class_index = add_class(cd->super_class);
	expr->type = type;
	return expr;

}


static Expression *fix_array_literal_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	ExpressionList *literal = expr->u.array_literal;
	if (literal == NULL) {
		mgc_compile_error(expr->line_number, ARRAY_LITERAL_EMPTY_ERR, MESSAGE_ARGUMENT_END);
	}
	literal->expression = fix_expression(current_block, literal->expression, expr, el_p);
	TypeSpecifier *elem_type = literal->expression->type;
	
	for (ExpressionList *pos = literal->next; pos; pos = pos->next) {
		pos->expression = fix_expression(current_block, pos->expression, expr, el_p);
		pos->expression = create_assign_cast(pos->expression, elem_type);
	}
	
	TypeSpecifier *type = mgc_alloc_type_specifier(elem_type->base_type);
	*type = *elem_type;
	type->derive = mgc_alloc_type_derive(ARRAY_DERIVE);
	type->derive->next = elem_type->derive;
	fix_type_specifier(type);
	expr->type = type;
	return expr;
}

static Expression *fix_index_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	IndexExpression *ie = &expr->u.index_expression;
	ie->array = fix_expression(current_block, ie->array, expr, el_p);
	ie->index = fix_expression(current_block, ie->index, expr, el_p);
	
	TypeSpecifier *type = NULL;
	if (mgc_is_array(ie->array->type)) {
		type = mgc_copy_type_specifier(ie->array->type);
		type->derive = ie->array->type->derive->next;
		
	}else if (mgc_is_string(ie->array->type)){
		type = mgc_alloc_type_specifier(DVM_INT_TYPE);
	}else{
		mgc_compile_error(expr->line_number, INDEX_LEFT_OPERAND_NOT_ARRAY_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (mgc_is_int(ie->index->type)) {
		mgc_compile_error(expr->line_number, INDEX_NOT_INT_ERR, MESSAGE_ARGUMENT_END);
	}
	expr->type = type;
	return expr;

}

static Expression *fix_inc_dec_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.inc_dec.operand = fix_expression(current_block, expr->u.inc_dec.operand, expr, el_p);
	if (!mgc_is_int(expr->u.inc_dec.operand->type)) {
		mgc_compile_error(expr->line_number, INC_DEC_TYPE_MISMATCH_ERR, MESSAGE_ARGUMENT_END);
	}
	expr->type = expr->u.inc_dec.operand->type;
	return expr;
	
}

static Expression *fix_instanceof_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.instanceof.expression = fix_expression(current_block, expr->u.instanceof.expression, expr, el_p);
	Expression *operand = expr->u.instanceof.expression;
	TypeSpecifier *type = expr->u.instanceof.type;
	fix_type_specifier(type);
	
	if (!mgc_is_object(operand->type)) {
		mgc_compile_error(expr->line_number, INSTANCEOF_OPERAND_NOT_REFERENCE_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (!mgc_is_object(type)) {
		mgc_compile_error(expr->line_number, INSTANCEOF_TYPE_NOT_REFERENCE_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (!mgc_is_class_object(type) || !mgc_is_class_object(operand->type)) {
		mgc_compile_error(expr->line_number, INSTANCEOF_FOR_NOT_CLASS_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (mgc_equal_type(operand->type, type)) {
		mgc_compile_error(expr->line_number, INSTANCEOF_MUST_RETURN_TRUE_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (is_super_class(operand->type->u.class_ref.class_definition, type->u.class_ref.class_definition, NULL, NULL)) {
		mgc_compile_error(expr->line_number, INSTANCEOF_MUST_RETURN_TRUE_ERR, MESSAGE_ARGUMENT_END);
	}
	if (type->u.class_ref.class_definition->class_or_interface == DVM_CLASS_DEFINITION
		&&!is_super_class(type->u.class_ref.class_definition, operand->type->u.class_ref.class_definition, NULL, NULL)) {
		mgc_compile_error(expr->line_number, INSTANCEOF_MUST_RETURN_FALSE_ERR, MESSAGE_ARGUMENT_END);
	}
	
	expr->type = mgc_alloc_type_specifier(DVM_BOOLEAN_TYPE);
	return expr;
	
}

static Expression *fix_down_cast_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	expr->u.down_cast.operand = fix_expression(current_block, expr->u.down_cast.operand, expr, el_p);
	fix_type_specifier(expr->u.down_cast.type);
	
	TypeSpecifier *org_type = expr->u.down_cast.operand->type;
	TypeSpecifier *target_type = expr->u.down_cast.type;
	
	if (!mgc_is_class_object(org_type)) {
		mgc_compile_error(expr->line_number, DOWN_CAST_OPERAND_IS_NOT_CLASS_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (!mgc_is_class_object(target_type)) {
		mgc_compile_error(expr->line_number, DOWN_CAST_TARGET_IS_NOT_CLASS_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (mgc_equal_type(org_type, target_type)) {
		mgc_compile_error(expr->line_number, DOWN_CAST_DO_NOTHING_ERR, MESSAGE_ARGUMENT_END);
	}
	
	if (target_type->u.class_ref.class_definition->class_or_interface == DVM_CLASS_DEFINITION
		&& !is_super_class(target_type->u.class_ref.class_definition, org_type->u.class_ref.class_definition, NULL, NULL)) {
		mgc_compile_error(expr->line_number, DOWN_CAST_TO_BAD_CLASS_ERR, MESSAGE_ARGUMENT_END);
	}
	
	expr->type = target_type;
	return expr;

}


static Expression *fix_new_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	size_t index = -1;
	ClassDefinition *cd = search_and_add(expr->line_number, expr->u.new_e.class_name, &index);
	expr->u.new_e.class_definition = cd;
	expr->u.new_e.class_index = index;
	
	if (cd->is_abstract) {
		mgc_compile_error(expr->kind, NEW_ABSTRACT_CLASS_ERR, STRING_MESSAGE_ARGUMENT, "name", cd->name);
	}
	
	if (expr->u.new_e.method_name == NULL) {
		expr->u.new_e.method_name = DEFAULT_CONSTRUCTOR_NAME;
	}
	
	MemberDeclaration *member = mgc_search_member(cd, expr->u.new_e.method_name);
	
	if (member == NULL) {
		mgc_compile_error(expr->line_number, MEMBER_NOT_FOUND_ERR, STRING_MESSAGE_ARGUMENT,"class_name", cd->name,
						  STRING_MESSAGE_ARGUMENT, "member_name", expr->u.new_e.method_name, MESSAGE_ARGUMENT_END);
	}
	
	if (member->kind != METHOD_MEMBER) {
		mgc_compile_error(expr->line_number, CONSTRUCTOR_IS_FIELD_ERR, STRING_MESSAGE_ARGUMENT, "member_name",expr->u.new_e.method_name, MESSAGE_ARGUMENT_END);
	}
	
	if (!member->u.method.is_constructor) {
		mgc_compile_error(expr->line_number, NOT_CONSTRUCTOR_ERR, STRING_MESSAGE_ARGUMENT, "member_name", expr->u.new_e.method_name, MESSAGE_ARGUMENT_END);
	}
	
	DBG_assert(member->u.method.function_definition->type->derive == NULL && member->u.method.function_definition->type->base_type == DVM_VOID_TYPE, "constructor is not void \n");
	
	check_argument(current_block, expr->line_number, member->u.method.function_definition->parameter_list, expr->u.new_e.argument, el_p, NULL);
	
	TypeSpecifier *type = mgc_alloc_type_specifier(DVM_CLASS_TYPE);
	type->identifier = cd->name;
	type->u.class_ref.class_definition = cd;
	type->u.class_ref.class_index = index;
	expr->type = type;
	return expr;
	
}


static Expression *fix_array_creation_expression(Block *current_block, Expression *expr, ExceptionList **el_p){
	fix_type_specifier(expr->u.array_creation.type);
	TypeDerive *derive = NULL;
	for (ArrayDimension *pos = expr->u.array_creation.dimension; pos; pos = pos->next) {
		if (pos->expression) {
			pos->expression= fix_expression(current_block, pos->expression, expr, el_p);
			if (!mgc_is_int(pos->expression->type)) {
				mgc_compile_error(expr->line_number, ARRAY_SIZE_NOT_INT_ERR, MESSAGE_ARGUMENT_END);
			}
		}
		TypeDerive *temp = mgc_alloc_type_derive(ARRAY_DERIVE);
		temp->next = derive;
		derive = temp;
	}
	
	TypeSpecifier *type = mgc_copy_type_specifier(expr->u.array_creation.type);
	type->derive = derive;
	expr->type = type;
	return expr;

}


static Expression *
fix_expression(Block *current_block, Expression *expr, Expression *parent,
			   ExceptionList **el_p)
{
	if (expr == NULL)
		return NULL;
	
	switch (expr->kind) {
		case BOOLEAN_EXPRESSION:
			expr->type = mgc_alloc_type_specifier(DVM_BOOLEAN_TYPE);
			break;
		case INT_EXPRESSION:
			expr->type = mgc_alloc_type_specifier(DVM_INT_TYPE);
			break;
		case DOUBLE_EXPRESSION:
			expr->type = mgc_alloc_type_specifier(DVM_DOUBLE_TYPE);
			break;
		case STRING_EXPRESSION:
			expr->type = mgc_alloc_type_specifier(DVM_STRING_TYPE);
			break;
		case IDENTIFIER_EXPRESSION:
			expr = fix_identifier_expression(current_block, expr);
			break;
		case COMMA_EXPRESSION:
			expr = fix_comma_expression(current_block, expr, el_p);
			break;
		case ASSIGN_EXPRESSION:
			expr = fix_assign_expression(current_block, expr, el_p);
			break;
		case ADD_EXPRESSION:        /* FALLTHRU */
		case SUB_EXPRESSION:        /* FALLTHRU */
		case MUL_EXPRESSION:        /* FALLTHRU */
		case DIV_EXPRESSION:        /* FALLTHRU */
		case MOD_EXPRESSION:
			expr = fix_math_binary_expression(current_block, expr, el_p);
			break;
		case BIT_AND_EXPRESSION:    /* FALLTHRU */
		case BIT_OR_EXPRESSION:     /* FALLTHRU */
		case BIT_XOR_EXPRESSION:
			expr = fix_bit_binary_expression(current_block, expr, el_p);
			break;
		case EQ_EXPRESSION: /* FALLTHRU */
		case NE_EXPRESSION: /* FALLTHRU */
		case GT_EXPRESSION: /* FALLTHRU */
		case GE_EXPRESSION: /* FALLTHRU */
		case LT_EXPRESSION: /* FALLTHRU */
		case LE_EXPRESSION:
			expr = fix_compare_expression(current_block, expr, el_p);
			break;
		case LOGICAL_AND_EXPRESSION:        /* FALLTHRU */
		case LOGICAL_OR_EXPRESSION:
			expr = fix_logical_and_or_expression(current_block, expr, el_p);
			break;
		case MINUS_EXPRESSION:
			expr = fix_minus_expression(current_block, expr, el_p);
			break;
		case BIT_NOT_EXPRESSION:
			expr = fix_bit_not_expression(current_block, expr, el_p);
			break;
		case LOGICAL_NOT_EXPRESSION:
			expr = fix_logical_not_expression(current_block, expr, el_p);
			break;
		case FUNCTION_CALL_EXPRESSION:
			expr = fix_function_call_expression(current_block, expr, el_p);
			break;
		case MEMBER_EXPRESSION:
			expr = fix_member_expression(current_block, expr, el_p);
			break;
		case NULL_EXPRESSION:
			expr->type = mgc_alloc_type_specifier(DVM_NULL_TYPE);
			break;
		case THIS_EXPRESSION:
			expr = fix_this_expression(expr);
			break;
		case SUPER_EXPRESSION:
			expr = fix_super_expression(expr, parent);
			break;
		case ARRAY_LITERAL_EXPRESSION:
			expr = fix_array_literal_expression(current_block, expr, el_p);
			break;
		case INDEX_EXPRESSION:
			expr = fix_index_expression(current_block, expr, el_p);
			break;
		case INCREMENT_EXPRESSION:  /* FALLTHRU */
		case DECREMENT_EXPRESSION:
			expr = fix_inc_dec_expression(current_block, expr, el_p);
			break;
		case INSTANCEOF_EXPRESSION:
			expr = fix_instanceof_expression(current_block, expr, el_p);
			break;
		case DOWN_CAST_EXPRESSION:
			expr = fix_down_cast_expression(current_block, expr, el_p);
			break;
		case CAST_EXPRESSION:
			break;
		case UP_CAST_EXPRESSION:
			break;
		case NEW_EXPRESSION:
			expr = fix_new_expression(current_block, expr, el_p);
			break;
		case ARRAY_CREATION_EXPRESSION:
			expr = fix_array_creation_expression(current_block, expr, el_p);
			break;
		case ENUMERATOR_EXPRESSION: /* FALLTHRU */
		case EXPRESSION_KIND_COUNT_PLUS_1:
			break;
		default:
			DBG_assert(0, "bad case. kind..%d\n", expr->kind);
	}
	fix_type_specifier(expr->type);
	
	return expr;
}

static void add_local_variable(FunctionDefinition *fd, Declaration *decl, DVM_Boolean is_parameter){
	fd->local_variable = MEM_realloc(fd->local_variable, sizeof(Declaration) * (fd->local_variable_count + 1));
	fd->local_variable[fd->local_variable_count] = decl;
	if (fd->class_definition && !is_parameter) {
		decl->variable_index = fd->local_variable_count + 1;
	}else{
		decl->variable_index = fd->local_variable_count;
	}
	fd->local_variable_count++;
	
}



static void fix_statement_list(Block *current_block, StatementList *list, FunctionDefinition *fd, ExceptionList **el_p);


static void fix_if_statement(Block *current_block, IfStatement *if_s, FunctionDefinition *fd, ExceptionList **el_p){
	if_s->condition = fix_expression(current_block, if_s->condition, NULL, el_p);
	if (!mgc_is_boolean(if_s->condition->type)) {
		mgc_compile_error(if_s->condition->line_number, IF_CONDITION_NOT_BOOLEAN_ERR, MESSAGE_ARGUMENT_END);
	}
	fix_statement_list(if_s->then_block, if_s->then_block->statement_list, fd, el_p);
	
	for (Elsif *pos = if_s->elsif_list; pos; pos = pos->next) {
		pos->condition = fix_expression(pos->then_block, pos->condition, NULL, el_p);
		if (!mgc_is_boolean(pos->condition->type)) {
			mgc_compile_error(pos->condition->line_number, IF_CONDITION_NOT_BOOLEAN_ERR, MESSAGE_ARGUMENT_END);
		}
		
		if (pos->then_block) {
			fix_statement_list(pos->then_block, pos->then_block->statement_list, fd, el_p);
		}
	}
	
	if (if_s->else_block) {
		fix_statement_list(if_s->then_block, if_s->then_block->statement_list, fd, el_p);
	}
}



static void fix_switch_statement(Block *current_block, SwitchStatement *switch_s, FunctionDefinition *fd, ExceptionList **el_p){
	switch_s->expression = fix_expression(current_block, switch_s->expression, NULL, el_p);
	for (CaseList *case_pos = switch_s->case_list; case_pos; case_pos = case_pos->next) {
		for (ExpressionList *expr_pos = case_pos->expression_list; expr_pos; expr_pos = expr_pos->next) {
			expr_pos->expression = fix_expression(current_block, expr_pos->expression, NULL, el_p);
			if (!(mgc_equal_type(switch_s->expression->type, expr_pos->expression->type)
				  || (switch_s->expression->kind == DVM_NULL_TYPE && mgc_is_object(expr_pos->expression->type))
				  || (expr_pos->expression->kind == DVM_NULL_TYPE && mgc_is_object(switch_s->expression->type)))) {
				mgc_compile_error(expr_pos->expression->line_number, CASE_TYPE_MISMATCH_ERR, MESSAGE_ARGUMENT_END);
    
			}
			fix_statement_list(case_pos->block, case_pos->block->statement_list, fd, el_p);
		}
	}
	
	if (switch_s->default_block) {
		fix_statement_list(switch_s->default_block, switch_s->default_block->statement_list, fd, el_p);
	}
}


static void fix_while_statement(Block *current_block, WhileStatement *while_s, FunctionDefinition *fd, ExceptionList **el_p){
	while_s->condition = fix_expression(current_block, while_s->condition, NULL, el_p);
	if (!mgc_is_boolean(while_s->condition->type)) {
		mgc_compile_error(while_s->condition->line_number, WHILE_CONDITION_NOT_BOOLEAN_ERR, MESSAGE_ARGUMENT_END);
	}
	fix_statement_list(while_s->block, while_s->block->statement_list, fd, el_p);
}


static void fix_for_statement(Block *current_block, ForStatement *for_s, FunctionDefinition *fd, ExceptionList **el_p){
	for_s->init= fix_expression(current_block, for_s->init, NULL, el_p);
	for_s->condition = fix_expression(current_block, for_s->condition, NULL, el_p);
	if (!mgc_is_boolean(for_s->condition->type)) {
		mgc_compile_error(for_s->condition->line_number, FOR_CONDITION_NOT_BOOLEAN_ERR, MESSAGE_ARGUMENT_END);
	}
	for_s->post = fix_expression(current_block, for_s->post, NULL, el_p);
	fix_statement_list(for_s->block, for_s->block->statement_list, fd, el_p);
}


static void fix_do_while_statement(Block *current_block, DoWhileStatement *do_while_s, FunctionDefinition *fd, ExceptionList **el_p){
	fix_statement_list(do_while_s->block, do_while_s->block->statement_list, fd, el_p);
	fix_expression(current_block, do_while_s->condition, NULL, el_p);
	if (!mgc_is_boolean(do_while_s->condition->type)) {
		mgc_compile_error(do_while_s->condition->line_number, DO_WHILE_CONDITION_NOT_BOOLEAN_ERR, MESSAGE_ARGUMENT_END);
	}
}

static void fix_return_statement(Block *current_block, Statement *return_s, FunctionDefinition *fd, ExceptionList **el_p){
	if (fd->type->base_type == DVM_VOID_TYPE && fd->type->derive == NULL && return_s->u.return_s.return_value != NULL) {
		mgc_compile_error(return_s->line_number, RETURN_IN_VOID_FUNCTION_ERR, MESSAGE_ARGUMENT_END);
	}

	if (return_s->u.return_s.return_value == NULL) {
		Expression *return_value = NULL;
		if (fd->type->derive != NULL) {
			DBG_assert(fd->type->derive->tag == ARRAY_DERIVE, "fd->type->derive..tag %d",fd->type->derive->tag);
			return_value = mgc_alloc_expression(NULL_EXPRESSION);
			
		}else{
			switch (fd->type->base_type) {
				case DVM_VOID_TYPE:
					return_value = mgc_alloc_expression(INT_EXPRESSION);
					return_value->u.int_value = 0;
					break;
				case DVM_BOOLEAN_TYPE:
					return_value = mgc_alloc_expression(BOOLEAN_EXPRESSION);
					return_value->u.boolean_value = DVM_FALSE;
					break;
				case DVM_INT_TYPE:
					return_value = mgc_alloc_expression(INT_EXPRESSION);
					return_value->u.int_value = 0;
					break;
				case DVM_DOUBLE_TYPE:
					return_value = mgc_alloc_expression(DOUBLE_EXPRESSION);
					return_value->u.double_value = 0.0;
					break;
				case DVM_STRING_TYPE:
				case DVM_CLASS_TYPE:
				case DVM_DELEGAET_TYPE:
				case DVM_NATIVE_POINTER_TYPE:
					return_value = mgc_alloc_expression(NULL_EXPRESSION);
					break;
				case DVM_ENUM_TYPE:
					return_value = create_first_enumerator(fd->type->u.enum_ref.enum_definition);
				case DVM_NULL_TYPE: /* FALLTHRU */
				case DVM_BASE_TYPE: /* FALLTHRU */
				case DVM_UNSPECIFIED_IDENTIFIER_TYPE:
					DBG_assert(0, "basic_type %d",fd->type->base_type);
				default:
					break;
			}
		}
		return_s->u.return_s.return_value = return_value;
		return;
	}
	
	return_s->u.return_s.return_value = create_assign_cast(return_s->u.return_s.return_value, fd->type);
}

static void add_declaration(Block *current_block, Declaration *declaration, FunctionDefinition *fd, int line_number, DVM_Boolean is_parameter){
	if (mgc_search_declaration(declaration->name, current_block)) {
		mgc_compile_error(line_number, VARIABLE_MULTIPLE_DEFINE_ERR, STRING_MESSAGE_ARGUMENT, "name", declaration->name, MESSAGE_ARGUMENT_END);
	}
	
	if (current_block) {
		current_block->declaration_list = mgc_chain_declaration(current_block->declaration_list, declaration);
	}
	
	if (fd) {
		declaration->is_loacl = DVM_TRUE;
		add_local_variable(fd, declaration, is_parameter);
	}else{
		MGC_Compiler *compiler = mgc_get_current_compiler();
		compiler->declaration_list = mgc_chain_declaration(compiler->declaration_list, declaration);
	}

}


static void fix_try_statement(Block *current_block, Statement *statement, FunctionDefinition *fd, ExceptionList **el_p){
	TryStatement *try_s = &statement->u.try_s;
	MGC_Compiler *compiler = mgc_get_current_compiler();
	TryStatement *try_backup = compiler->current_try_statement;
	compiler->current_try_statement = try_s;
	ExceptionList *new_el_p = NULL;
	fix_statement_list(try_s->try_block, try_s->try_block->statement_list, fd, &new_el_p);
	
	for (CatchClause *catch = try_s->catch_clause; catch; catch = catch->next) {
		CatchClause *catch_backup = compiler->current_catch_clause;
		compiler->current_catch_clause = catch;
		fix_type_specifier(catch->type);
		if (!mgc_is_object(catch->type)) {
			mgc_compile_error(catch->line_number, CATCH_TYPE_IS_NOT_CLASS_ERR, MESSAGE_ARGUMENT_END);
		}
		if (!is_exception_class(catch->type->u.class_ref.class_definition)) {
			mgc_compile_error(catch->line_number, CATCH_TYPE_IS_NOT_EXCEPTION_ERR, STRING_MESSAGE_ARGUMENT, "name", catch->type->u.class_ref.class_definition->name, MESSAGE_ARGUMENT_END);
		}
		remove_exception(&new_el_p, catch->type->u.class_ref.class_definition);
		
		Declaration *declaration = mgc_alloc_declaration(DVM_TRUE, catch->type, catch->variable_name);
		catch->variable_declaration = declaration;
		add_declaration(catch->block, declaration, fd, catch->line_number, DVM_FALSE);
		fix_statement_list(catch->block, catch->block->statement_list, fd, el_p);
		compiler->current_catch_clause = catch_backup;
	}
	
	add_exception(el_p, new_el_p);
	
	if (try_s->finally_block) {
		fix_statement_list(try_s->finally_block, try_s->finally_block->statement_list, fd, el_p);
	}
	
	compiler->current_try_statement = try_backup;
	
}

static ExceptionList *type_to_exception_list(TypeSpecifier *type, int line_number){
	ExceptionList *excetpion_list = MEM_malloc(sizeof(ExceptionList));
	excetpion_list->exception = MEM_malloc(sizeof(ExceptionRef));
	excetpion_list->exception->class_definition = type->u.class_ref.class_definition;
	excetpion_list->exception->identifer = type->identifier;
	excetpion_list->exception->line_number = line_number;
	excetpion_list->next = NULL;
	return excetpion_list;
}

static void fix_throw_statement(Block *current_block, Statement *statement, FunctionDefinition *fd, ExceptionList **el_p){
	statement->u.throw_s.expression = fix_expression(current_block, statement->u.throw_s.expression, NULL, el_p);
	Expression *expression = statement->u.throw_s.expression;
	ExceptionList *el = NULL;
	if (expression) {
		if (!mgc_is_object(expression->type)) {
			mgc_compile_error(expression->line_number, THROW_TYPE_IS_NOT_CLASS_ERR, MESSAGE_ARGUMENT_END);
		}
		
		if (!is_exception_class(expression->type->u.class_ref.class_definition)) {
			mgc_compile_error(expression->line_number, THROW_TYPE_IS_NOT_EXCEPTION_ERR, STRING_MESSAGE_ARGUMENT, "name", expression->type->u.class_ref.class_definition->name, MESSAGE_ARGUMENT_END);
		}
		 el = type_to_exception_list(expression->type, expression->line_number);
	}else{
		MGC_Compiler *compiler = mgc_get_current_compiler();
		if (compiler->current_catch_clause == NULL) {
			mgc_compile_error(statement->line_number, RETHOROW_OUT_OF_CATCH_ERR, MESSAGE_ARGUMENT_END);
		}
		statement->u.throw_s.variable_declaration = compiler->current_catch_clause->variable_declaration;
		el = type_to_exception_list(statement->u.throw_s.variable_declaration->type, statement->line_number);
	}
	
	add_exception(el_p, el);
}


static void check_in_finally(Block *block, Statement *statement){
	char *str = NULL;
	char *label = NULL;
	
	if (statement->type == RETURN_STATEMENT) {
		str = "return";
	}else if (statement->type == BREAK_STATEMENT){
		str = "break";
		label = statement->u.break_s.label;
	}else if(statement->type == CONTINUE_STATEMENT){
		str = "continue";
		label = statement->u.continue_s.label;
	}
	
	DVM_Boolean is_in_finally = DVM_FALSE;
	for (Block *pos = block; pos; pos = pos->out_block) {
		if (pos->type == FINALLY_CLAUSE_BLOCK) {
			is_in_finally = DVM_TRUE;
			break;
		}
		
		if(statement->type == BREAK_STATEMENT || statement->type == CONTINUE_STATEMENT){
			
			if (pos->type == WHILE_STATEMENT_BLOCK && dvm_equal_string(label, pos->parent.statement_info.statement->u.while_s.label)) {
				break;
			}
			
			if (pos->type == DO_WHILE_STATEMENT_BLOCK && dvm_equal_string(label, pos->parent.statement_info.statement->u.do_while_s.label)) {
				break;
			}
			
			if (pos->type == FOR_STATEMENT_BLOCK && dvm_equal_string(label, pos->parent.statement_info.statement->u.for_s.label)) {
				break;
			}
			
		}
	}
	
	if (is_in_finally) {
		mgc_compile_error(statement->line_number, GOTO_STATEMENT_IN_FINALLY_ERR,STRING_MESSAGE_ARGUMENT, "statement_name",str, MESSAGE_ARGUMENT_END);
	}
	
}


static void
fix_statement(Block *current_block, Statement *statement,
			  FunctionDefinition *fd, ExceptionList **el_p)
{
	switch (statement->type) {
		case EXPRESSION_STATEMENT:
			statement->u.expression_s
			= fix_expression(current_block, statement->u.expression_s,
							 NULL, el_p);
			break;
		case IF_STATEMENT:
			fix_if_statement(current_block, &statement->u.if_s, fd, el_p);
			break;
		case SWITCH_STATEMENT:
			fix_switch_statement(current_block, &statement->u.switch_s, fd, el_p);
			break;
		case WHILE_STATEMENT:
			fix_while_statement(current_block, &statement->u.while_s, fd, el_p);
			break;
		case FOR_STATEMENT:
			fix_for_statement(current_block, &statement->u.for_s, fd, el_p);
			break;
		case DO_WHILE_STATEMENT:
			fix_do_while_statement(current_block, &statement->u.do_while_s,
								   fd, el_p);
			break;
		case FOREACH_STATEMENT:
			statement->u.foreach_s.collection
			= fix_expression(current_block, statement->u.foreach_s.collection,
							 NULL, el_p);
			fix_statement_list(statement->u.for_s.block,
							   statement->u.for_s.block->statement_list, fd,
							   el_p);
			break;
		case RETURN_STATEMENT:
			check_in_finally(current_block, statement);
			fix_return_statement(current_block, statement, fd, el_p);
			break;
		case BREAK_STATEMENT:
			check_in_finally(current_block, statement);
			break;
		case CONTINUE_STATEMENT:
			check_in_finally(current_block, statement);
			break;
		case TRY_STATEMENT:
			fix_try_statement(current_block, statement, fd, el_p);
			break;
		case THROW_STATEMENT:
			fix_throw_statement(current_block, statement, fd, el_p);
			break;
		case DECLARATION_STATEMENT:
			add_declaration(current_block, statement->u.declaration_s, fd, statement->line_number, DVM_FALSE);
			fix_type_specifier(statement->u.declaration_s->type);
			if (statement->u.declaration_s->initializer) {
				Expression *temp = fix_expression(current_block, statement->u.declaration_s->initializer, NULL, el_p);
				statement->u.declaration_s->initializer = create_assign_cast(temp, statement->u.declaration_s->type);
			}
			
			break;
		case STATEMENT_TYPE_COUNT_PLUS_1: /* FALLTHRU */
		default:
			DBG_assert(0, "bad case. type..%d\n", statement->type);
	}
}

static void fix_statement_list(Block *current_block, StatementList *list, FunctionDefinition *fd, ExceptionList **el_p){
	for (StatementList *pos = list; pos; pos = pos->next) {
		fix_statement(current_block, pos->statement, fd, el_p);
	}
}

static void add_parameter_as_declaration(FunctionDefinition *fd){
	for (ParameterList *pos = fd->parameter_list; pos; pos = pos->next) {
		if (mgc_search_declaration(pos->name, fd->block)) {
			mgc_compile_error(pos->line_number, PARAMETER_MULTIPLE_DEFINE_ERR,STRING_MESSAGE_ARGUMENT, "name", pos->name);
		}
		fix_type_specifier(pos->type);
		Declaration *decl = mgc_alloc_declaration(DVM_TRUE, pos->type, pos->name);
		
		add_declaration(fd->block, decl, fd, pos->line_number, DVM_TRUE);
	}

}

static void add_return_statement(FunctionDefinition *fd, ExceptionList **el_p){
	StatementList **last_p;
	if (fd->block->statement_list == NULL) {
		last_p = & fd->block->statement_list;
	}else{
		StatementList *last = NULL;
		for (last = fd->block->statement_list; last; last  = last->next){
			if (last->statement->type == RETURN_STATEMENT) {
				return;
			}
		}
		last_p = &last->next;
	}
	
	Statement *ret = mgc_create_return_statement(NULL);
	ret->line_number = fd->end_line_number;
	fix_statement(fd->block, ret, fd, el_p);
	*last_p = mgc_create_statement_list(ret);
}

static void fix_function(FunctionDefinition *fd){
	ExceptionList *el = NULL;
	ExceptionList *error_exception = NULL;
	
	add_parameter_as_declaration(fd);
	fix_type_specifier(fd->type);
	fix_thorws(fd->throws);
	
	if (fd->block) {
		fix_statement_list(fd->block, fd->block->statement_list, fd, &el);
		add_return_statement(fd, &el);
	}
	
	if (!check_throws(fd->throws, el, &error_exception)) {
		mgc_compile_error(fd->end_line_number, EXCEPTION_HAS_TO_BE_THROWN_ERR, STRING_MESSAGE_ARGUMENT, "name", error_exception->exception->identifer, MESSAGE_ARGUMENT_END);
	}
	
}

static void add_super_interface(ClassDefinition *cd){
	
	ExtendsList *tail = NULL;
	for (tail = cd->interface_list; tail->next; tail = tail->next)
		;
	for (ClassDefinition *super = cd->super_class; super; super = super->super_class) {
		for (ExtendsList *pos = super->interface_list; pos;pos = pos->next) {
			ExtendsList *temp = mgc_malloc(sizeof(ExtendsList));
			*temp = *pos;
			temp->next = NULL;
			if (tail) {
				tail->next = temp;
			}else{
				cd->interface_list = temp;
			}
			tail = temp;
		}
	}
}

static void fix_extends(ClassDefinition *cd){
	for (ExtendsList *extend_pos = cd->extends; extend_pos; extend_pos = extend_pos->next) {
		size_t index = 0;
		ClassDefinition *super = search_and_add(cd->line_number, cd->name, &index);
		
		ExtendsList *last_interface = NULL;
		extend_pos->class_definition = super;
		if (super->class_or_interface == DVM_CLASS_DEFINITION) {
			if (cd->super_class) {
				mgc_compile_error(cd->line_number, MULTIPLE_INHERITANCE_ERR, STRING_MESSAGE_ARGUMENT, "name", super->name, MESSAGE_ARGUMENT_END);
			}
			
			if (!super->is_abstract) {
				mgc_compile_error(cd->line_number, INHERIT_CONCRETE_CLASS_ERR, STRING_MESSAGE_ARGUMENT, "name", super->name, MESSAGE_ARGUMENT_END);
			}
			
			cd->super_class = super;
			
		}else{
			DBG_assert(super->class_or_interface == DVM_INTERFACE_DEFINITION, "super...%d",super->class_or_interface);
			ExtendsList *temp = mgc_malloc(sizeof(ExtendsList));
			*temp = *extend_pos;
			temp->next = NULL;
			
			if (last_interface == NULL) {
				cd->interface_list = temp;
			}else{
				last_interface->next = temp;
			}
			
			last_interface = temp;
		}
	}
}


static void add_default_constructor(ClassDefinition *cd){
	MemberDeclaration *last_member = NULL;
	for (last_member = cd->member; last_member; last_member = last_member->next) {
		if (last_member->kind == METHOD_MEMBER && last_member->u.method.is_constructor) {
			return;
		}
		if (last_member->next == NULL) {
			break;
		}
	}
	
	TypeSpecifier *type = mgc_alloc_type_specifier(DVM_VOID_TYPE);
	Block *block = mgc_alloc_block();
	ClassOrMemberModifierList v_modifier = mgc_create_class_or_member_modifier(VIRTUAL_MODIFIER);
	ClassOrMemberModifierList p_modifier = mgc_create_class_or_member_modifier(PUBLIC_MODIFIER);
	ClassOrMemberModifierList modifier = mgc_chain_class_or_member_modifier(v_modifier,p_modifier);
	if (cd->super_class) {
		ClassOrMemberModifierList o_modifier = mgc_create_class_or_member_modifier(OVERRIDE_MODIFIER);
		modifier = mgc_chain_class_or_member_modifier(modifier, o_modifier);
		Expression *super = mgc_create_super_expression();
		Expression *method = mgc_create_member_expression(super, DEFAULT_CONSTRUCTOR_NAME);
		Expression *call = mgc_create_function_call_expression(method, NULL);
		Statement *statement = mgc_alloc_statement(EXPRESSION_STATEMENT);
		statement->u.expression_s = call;
		block->statement_list= mgc_create_statement_list(statement);
	}else{
		block->statement_list = NULL;
	}
	
    FunctionDefinition *fd	= mgc_create_function_definition(type, DEFAULT_CONSTRUCTOR_NAME, NULL, NULL, block);
	MemberDeclaration *member = mgc_create_method_member(&modifier, fd, DVM_TRUE);
	
	if (last_member) {
		last_member->next = member;
	}else{
		cd->member = member;
	}
 
}


static void get_super_field_method_count(ClassDefinition *cd, size_t *field_index_out, size_t *method_index_out){
	size_t field_index = -1;
	size_t method_index = -1;
	for (ClassDefinition *class_pos = cd->super_class; class_pos; class_pos = class_pos->next) {
		for (MemberDeclaration *member_pos = class_pos->member; member_pos; member_pos = member_pos->next) {
			if (member_pos->kind == METHOD_MEMBER) {
				if (member_pos->u.method.method_index > method_index || method_index == (size_t)-1) {
					method_index = member_pos->u.method.method_index;
				}
    
			}else{
				DBG_assert(member_pos->kind == FIELD_MEMBER, "member_pos->kind..%d",member_pos->kind);
				if (member_pos->u.field.field_index > field_index || field_index == (size_t)-1) {
					field_index = member_pos->u.field.field_index ;
				}
				
			}
		}
	}
	*field_index_out = field_index + 1;
	*method_index_out = method_index + 1;

}


static MemberDeclaration *search_member_in_super(ClassDefinition *cd, char *member_name){
	MemberDeclaration *member = NULL;
	if (cd->super_class) {
		member = mgc_search_member(cd->super_class, member_name);
	}
	if (member) {
		return member;
	}
	
	for (ExtendsList *pos = cd->interface_list; pos; pos = pos->next) {
		member = mgc_search_member(pos->class_definition, member_name);
		if (member_name) {
			return member;
		}
	}
	
	return NULL;
}

static void check_method_override(MemberDeclaration *super_method, MemberDeclaration *sub_method){
	if (sub_method->access_modifier < sub_method->access_modifier) {
		mgc_compile_error(sub_method->line_number, OVERRIDE_METHOD_ACCESSIBILITY_ERR, STRING_MESSAGE_ARGUMENT, "name", sub_method->u.method.function_definition->name, MESSAGE_ARGUMENT_END);
	}
	
	if (!sub_method->u.method.is_constructor) {
		check_func_compatibility(super_method->u.method.function_definition, sub_method->u.method.function_definition);
	}
}
















void mgc_fix_tree(MGC_Compiler *compiler){

}
