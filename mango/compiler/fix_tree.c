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
			*is_interface = DVM_FALSE;
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


static Expression *fix_expression(Block *current_block, Expression *expr, Expression *parent, ExpressionList **el_p);


static Expression *fix_comma_expression(Block *current_block, Expression *expr, ExpressionList **el_p){
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
	
	Expression *cast_expr = mgc_alloc_expression(UP_CASE_EXPRESSION);
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






















void mgc_fix_tree(MGC_Compiler *compiler){

}
