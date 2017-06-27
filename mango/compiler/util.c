//
//  util.c
//  mango
//
//  Created by jerry.yong on 2017/6/5.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mangoc.h"
#include "DVM.h"

static MGC_Compiler *st_current_Compiler;


MGC_Compiler *mgc_get_current_compiler(void){
	return st_current_Compiler;
}

void mgc_set_current_compiler(MGC_Compiler *compiler){
	st_current_Compiler = compiler;
}

void *mgc_malloc(size_t size){
	MGC_Compiler *compler = mgc_get_current_compiler();
    return  MEM_storage_malloc(compler->compile_storage, size);
}



char *mgc_package_name_to_string(PackageName *package_name){
	
	if (package_name == NULL) {
		return NULL;
	}
	size_t len = 0;
	PackageName *fragment = package_name;
	while (fragment != NULL) {
		len += (strlen(fragment->name) + 1);
		fragment = fragment->next;
	}
	
	char *ret_val = MEM_malloc(len);
	fragment = package_name;
	ret_val[0] = '\0';
	while (fragment != NULL) {
		strcat(ret_val, fragment->name);
		if (fragment->next) {
			strcat(ret_val, ".");
		}
		fragment = fragment->next;
	}
	return ret_val;
}


DVM_Boolean mgc_equal_package_name(PackageName *package_name1, PackageName *package_name2){
	if (package_name1 == NULL || package_name2 == NULL) {
		return DVM_FALSE;
	}
	
	
	PackageName *last1 = package_name1;
	PackageName *last2 = package_name2;
	
	while (last1 && last2) {
		if (strcmp(last1->name, last2->name) != 0) {
			return DVM_FALSE;
		}
		last1 = last1->next;
		last2 = last2->next;
	}
	
	if (last1 || last2) {
		return DVM_FALSE;
	}
	
	return DVM_TRUE;
	
	
	
	
	
}
static FunctionDefinition *search_rename_function(MGC_Compiler *compiler, RenameList *rename){
	CompilerList *compiler_pos = compiler->required_list;
	while (compiler_pos) {
		if (!mgc_equal_package_name(compiler_pos->compiler->package_name, rename->package_name)) {
			
			compiler_pos = compiler_pos->next;
			continue;
		}
		
		for (FunctionDefinition *func_pos = compiler_pos->compiler->function_list; func_pos; func_pos = func_pos->next) {
			if (!strcmp(func_pos->name, rename->original_name) && func_pos->class_definition == NULL) {
				return func_pos;
			}
		}
		
	}
	
	return NULL;
	
}


FunctionDefinition *mgc_search_function(char *name){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	
	for (FunctionDefinition	*fun_pos = compiler->function_list; fun_pos; fun_pos = fun_pos->next) {
		if (!strcmp(fun_pos->name, name) && fun_pos->class_definition != NULL) {
			return fun_pos;
		}
	}
	
	for (RenameList	*rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, name)) {
			FunctionDefinition *fd = search_rename_function(compiler, rename_pos);
			if (fd) {
				return fd;
			}
		}
	}
	
	for (CompilerList *complier_pos = compiler->required_list; complier_pos; complier_pos = complier_pos->next) {
		for (FunctionDefinition *fun_pos = complier_pos->compiler->function_list; fun_pos; fun_pos = fun_pos->next) {
			if (!strcmp(fun_pos->name, name) && fun_pos->class_definition == NULL) {
				return fun_pos;
			}
		}
	}
	
	return NULL;
}

Declaration *mgc_search_declaration(char *identifier, Block *block){
	for (Block *block_pos = block; block; block_pos = block->out_block) {
		for (DeclarationList *declaration_pos = block_pos->declaration_list; declaration_pos; declaration_pos = declaration_pos->next) {
			if (!strcmp(declaration_pos->declaration->name, identifier)) {
				return declaration_pos->declaration;
			}
		}
	}
	
	for (DeclarationList *declaration_pos = mgc_get_current_compiler()->declaration_list; declaration_pos; declaration_pos = declaration_pos->next) {
		if (!strcmp(declaration_pos->declaration->name, identifier)) {
			return declaration_pos->declaration;
		}
	}
	
	return NULL;
}


static ConstantDefinition *search_renamed_constant(MGC_Compiler *compiler, RenameList *rename){
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		if (!mgc_equal_package_name(compiler_pos->compiler->package_name, rename->package_name)) {
			continue;
		}
		
		for (ConstantDefinition *const_pos = compiler_pos->compiler->constant_definition_list; compiler_pos; compiler_pos = compiler_pos->next) {
			if (!strcmp(const_pos->name, rename->original_name)) {
				return const_pos;
			}
		}
	}
	
	return NULL;
}


ConstantDefinition *mgc_search_constant(char *identifier){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	
	for (ConstantDefinition * const_pos = compiler->constant_definition_list; const_pos; const_pos = const_pos->next) {
		if (!strcmp(const_pos->name, identifier)) {
			return const_pos;
		}
	}
	
	for (RenameList *rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, identifier)) {
			ConstantDefinition *cd = search_renamed_constant(compiler, rename_pos);
			if (cd) {
				return cd;
			}
		}
		
	}
	
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		for (ConstantDefinition *const_pos = compiler_pos->compiler->constant_definition_list; const_pos; const_pos = const_pos->next) {
			if (!strcmp(const_pos->name, identifier)) {
				return const_pos;
			}
		}
	}
	return NULL;
}


static ClassDefinition *search_rename_class(MGC_Compiler *compiler,RenameList *rename){
	for (CompilerList *compile_pos = compiler->required_list; compile_pos; compile_pos = compile_pos->next) {
		if (!mgc_equal_package_name(compile_pos->compiler->package_name, rename->package_name)) {
			continue;
		}
		
		for (ClassDefinition *class_pos = compile_pos->compiler->class_definition_list; class_pos; class_pos = class_pos->next) {
			if (!strcmp(class_pos->name, rename->original_name)) {
				return class_pos;
			}
		}
	}
	
	return NULL;
	
}



ClassDefinition *mgc_search_class(char *identifier){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	for (ClassDefinition *class_pos = compiler->class_definition_list; class_pos; class_pos = class_pos->next) {
		if (!strcmp(class_pos->name, identifier)) {
			return class_pos;
		}
	}
	
	for (RenameList *rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, identifier)) {
			ClassDefinition *cd = search_rename_class(compiler, rename_pos);
			if (cd) {
				return cd;
			}
		}
	}
	
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		for (ClassDefinition *class_pos = compiler_pos->compiler->class_definition_list; class_pos; class_pos = class_pos->next) {
			if (!strcmp(class_pos->name, identifier)) {
				return class_pos;
			}
		}
	}
	
	return NULL;
	
}


static DelegateDefinition *search_rename_delegate(MGC_Compiler *compiler, RenameList *rename){
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		if (!mgc_equal_package_name(compiler_pos->compiler->package_name, rename->package_name)) {
			continue;
		}
		
		for (DelegateDefinition *delegate_pos = compiler_pos->compiler->delegate_definition_list; delegate_pos; delegate_pos = delegate_pos->next) {
			if (!strcmp(delegate_pos->name, rename->original_name)) {
				return delegate_pos;
			}
		}
	}
	return NULL;
}


DelegateDefinition *mgc_search_delegate(char *identifier){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	for (DelegateDefinition *delegate_pos = compiler->delegate_definition_list; delegate_pos; delegate_pos = delegate_pos->next) {
		if (!strcmp(delegate_pos->name, identifier)) {
			return delegate_pos;
		}
	}
	
	for (RenameList *rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, identifier)) {
			DelegateDefinition *dd = search_rename_delegate(compiler, rename_pos);
			if (dd) {
				return dd;
			}
		}
	}
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		for (DelegateDefinition *delegate_pos = compiler_pos->compiler->delegate_definition_list; delegate_pos; delegate_pos = delegate_pos->next) {
			if (!strcmp(delegate_pos->name, identifier)) {
				return delegate_pos;
			}
		}
	}
	
	return NULL;
	
}


static EnumDefinition *search_rename_enum(MGC_Compiler *compiler, RenameList *rename){
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		if (!mgc_equal_package_name(compiler_pos->compiler->package_name, rename->package_name)) {
			continue;
		}
		
		for (EnumDefinition *enum_pos = compiler_pos->compiler->enum_definition_list; enum_pos; enum_pos = enum_pos->next) {
			if (!strcmp(enum_pos->name, rename->original_name)) {
				return enum_pos;
			}
		}
		
		
	}
	
	return NULL;
}

EnumDefinition *mg_search_enum(char *identifier){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	for (EnumDefinition *enum_pos = compiler->enum_definition_list; enum_pos; enum_pos = enum_pos->next) {
		if (!strcmp(enum_pos->name, identifier)) {
			return enum_pos;
		}
	}
	
	for (RenameList *rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, identifier)) {
			EnumDefinition *ed = search_rename_enum(compiler,rename_pos);
			if (ed) {
				return ed;
			}
		}
	}
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		for (EnumDefinition *enum_pos = compiler_pos->compiler->enum_definition_list; enum_pos; enum_pos = enum_pos->next) {
			if (!strcmp(enum_pos->name, identifier)) {
				return enum_pos;
			}
		}
	}
	
	return NULL;
}


MemberDeclaration *mgc_search_member(ClassDefinition *class_def,char *member_name){
	MemberDeclaration *member;
	for (member = class_def->member; member; member = member->next) {
		if (member->kind == METHOD_MEMBER) {
			break;
			
		}else if (member->kind == FIELD_MEMBER){
			break;
		}else{
			
		}
	}
	
	if (member) {
		return member;
	}
	
	if (class_def->super_class) {
		member = mgc_search_member(class_def->super_class, member_name);
	}
	
	if (member) {
		return member;
	}
	
	for (ExtendsList *extends_p = class_def->interface_list; extends_p; extends_p = extends_p->next) {
		member = mgc_search_member(extends_p->class_definition,member_name);
		if (member) {
			return member;
		}
	}
	
	return NULL;
}

TypeSpecifier *mgc_alloc_type_specifier(DVM_BaseType type){
	TypeSpecifier *ts = mgc_malloc(sizeof(*ts));
	ts->base_type = type;
	ts->line_number = 0;
	ts->derive = NULL;
	if (type == DVM_CLASS_TYPE) {
		ts->identifier = NULL;
		ts->u.class_ref.class_definition = NULL;
	}
	return ts;
}

TypeDerive *mgc_alloc_type_derive(DeriveTag derive_tag){
	TypeDerive *type_derive = mgc_malloc(sizeof(*type_derive));
	type_derive->tag = derive_tag;
	type_derive->next = NULL;
	return type_derive;
}



