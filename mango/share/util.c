//
//  util.c
//  mango
//
//  Created by yongpengliang on 2017/6/10.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DVM.h"
#include "mangoc.h"


DVM_Boolean dvm_equal_string(char *str1, char *str2){
	if (str1 == NULL && str2 == NULL) {
		return DVM_TRUE;
	}
	
	if (str1 == NULL || str2 == NULL) {
		return DVM_FALSE;
	}
	
	
	return !strcmp(str1, str2);
}

void dvm_strncopy(char *dest, char *src, size_t buf_size){
	size_t i;
	for ( i = 0 ; i < buf_size - 1  && src[i] != '\0' ; i++) {
		dest[i] = src[i];
	}
	dest[i] = '\0';
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




