//
//  interface.c
//  mango
//
//  Created by jerry.yong on 2017/6/16.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "mangoc.h"
#include "DBG.h"
#include "MGC.h"
#include "share.h"


static CompilerList *st_compiler_list = NULL;
extern BuiltinScript mgc_buildin_script[];

typedef struct {
	char			*name;
	DVM_BaseType	type;
}BuildInMethodParameter;

typedef struct {
	char					*name;
	DVM_BaseType			return_type;
	BuildInMethodParameter	*parameter;
	int						parameter_count;
}BuildInMethod;

static BuildInMethodParameter st_array_resize_arg[] = {
	{"new_size", DVM_INT_TYPE}
};

static	BuildInMethodParameter st_array_insert_arg[] = {
	{"index", DVM_INT_TYPE},
	{"new_item", DVM_BASE_TYPE}
};


static BuildInMethodParameter st_array_remove_arg[] = {
	{"index", DVM_INT_TYPE}
};


static BuildInMethodParameter st_array_add_arg[] = {
	{"new_item", DVM_BASE_TYPE}
};


static BuildInMethod st_array_method[] = {
	{ARRAY_METHOD_SIZE, DVM_INT_TYPE, NULL, 0},
	{ARRAY_METHOD_RESIZE, DVM_VOID_TYPE, st_array_resize_arg, ARRAY_SZIE(st_array_resize_arg)},
	{ARRAY_METHOD_INSERT, DVM_VOID_TYPE, st_array_insert_arg, ARRAY_SZIE(st_array_insert_arg)},
	{ARRAY_METHOD_REMOVE, DVM_VOID_TYPE, st_array_remove_arg, ARRAY_SZIE(st_array_remove_arg)},
	{ARRAY_METHOD_ADD, DVM_VOID_TYPE, st_array_add_arg, ARRAY_SZIE(st_array_add_arg)}
};


static BuildInMethodParameter st_string_substr_arg[] = {
	{"start", DVM_INT_TYPE},
	{"length", DVM_INT_TYPE}
};


static BuildInMethod st_string_method[] = {
	{STRING_METHOD_LENGTH, DVM_INT_TYPE, NULL, 0},
	{STRING_METHOD_SUBSTR, DVM_STRING_TYPE, st_string_substr_arg, ARRAY_SZIE(st_string_substr_arg)}
};


static FunctionDefinition *create_build_in_method(BuildInMethod *src, int method_count){
	FunctionDefinition *fd_array = mgc_malloc(sizeof(FunctionDefinition) * method_count);
	for (int i = 0; i < method_count; i++) {
		BuildInMethod build_in_method = src[i];
		fd_array[i].name = build_in_method.name;
		fd_array[i].type = mgc_alloc_type_specifier(build_in_method.return_type);
		ParameterList *parameter_list = NULL;
		for (int j = 0; j < build_in_method.parameter_count; j++) {
			BuildInMethodParameter build_in_parameter = build_in_method.parameter[j];
			TypeSpecifier *type = mgc_alloc_type_specifier(build_in_parameter.type);
			char *identifier = build_in_parameter.name;
			if (parameter_list == NULL) {
				parameter_list = mgc_create_parameter(type,identifier);
			}else{
				mgc_chain_parameter(parameter_list, type, identifier);
			}
		}
		
		
	}
	
	return fd_array;
}



MGC_Compiler *mgc_create_compiler(void){
	MGC_Compiler *compiler_backup = mgc_get_current_compiler();
	MEM_Storage storage = MEM_open_storage(0);
	MGC_Compiler *compiler = MEM_storage_malloc(storage, sizeof(struct MGC_Compiler_tag));
	mgc_set_current_compiler(compiler);
	compiler->compile_storage = storage;
	compiler->package_name = NULL;
	compiler->source_suffix = MGM_SOURCE;
	compiler->require_list = NULL;
	compiler->rename_list = NULL;
	compiler->function_list = NULL;
	compiler->dvm_function_count = 0;
	compiler->dvm_function = NULL;
	compiler->dvm_enum_count = 0;
	compiler->dvm_enum = NULL;
	compiler->dvm_constant_count = 0;
	compiler->dvm_constant = NULL;
	compiler->dvm_class_count = 0;
	compiler->dvm_class = NULL;
	compiler->declaration_list = NULL;
	compiler->statement_list = NULL;
	compiler->class_definition_list = NULL;
	compiler->enum_definition_list = NULL;
	compiler->delegate_definition_list = NULL;
	compiler->constant_definition_list = NULL;
	compiler->current_block = NULL;
	compiler->current_catch_clause = NULL;
	compiler->current_class_definition = NULL;
	compiler->current_line_number = 1;
	compiler->input_mode = FILE_INPUT_MODE;
	compiler->required_list = NULL;
	compiler->array_method_count = ARRAY_SZIE(st_array_method);
	compiler->array_method = create_build_in_method(st_array_method,compiler->array_method_count);
	compiler->string_method_count = ARRAY_SZIE(st_string_method);
	compiler->string_method = create_build_in_method(st_string_method, compiler->string_method_count);
	mgc_set_current_compiler(compiler_backup);
	return compiler;
}


