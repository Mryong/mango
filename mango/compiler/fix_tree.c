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
	
	
	
	
	
}























void mgc_fix_tree(MGC_Compiler *compiler){

}
