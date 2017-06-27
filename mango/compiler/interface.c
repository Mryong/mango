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
#include <string.h>
#include "DBG.h"
#include "DVM.h"
#include "DVM_code.h"


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


static CompilerList *add_compiler_to_list(CompilerList *list, MGC_Compiler *compiler){
	CompilerList *new_item = MEM_malloc(sizeof(*new_item));
	new_item->compiler = compiler;
	new_item->next = NULL;
	
	if (list == NULL) {
		return new_item;
	}
	
	CompilerList *pos = list;
	for (; pos->next; pos = pos->next)
		;
	pos->next = new_item;
	return list;
	
}


static MGC_Compiler *search_compiler(CompilerList *list, PackageName *package_name){
	for (CompilerList *pos = list; pos; pos = pos->next) {
		if (mgc_equal_package_name(pos->compiler->package_name, package_name)) {
			return pos->compiler;
		}
	}
	
	return NULL;
}

static DVM_Boolean search_buildin_source(char *package_name, SourceSuffix suffix,SourceInput *input){
	for (int i = 0; mgc_buildin_script[i].source_string != NULL; i++) {
		if (mgc_buildin_script[i].suffix == suffix && dvm_equal_string(mgc_buildin_script[i].package_name, package_name)) {
			input->mode = STRING_INPUT_MODE;
			input->u.string.lines = mgc_buildin_script[i].source_string;
			return DVM_TRUE;
		}
	}
	return DVM_FALSE;
}


static void make_search_path(int line_number, PackageName *package_name, char *buf){
	size_t len = 0;
	size_t suffix_len = sizeof(MANGO_REQUIRE_SUFFIX);
	buf[0] = '\0';
	for (PackageName *pos = package_name; pos; pos = pos->next) {
		len += strlen(pos->name);
		if (len > FILENAME_MAX - (2 + suffix_len)) {
			mgc_compile_error(line_number, PACKAGE_NAME_TOO_LONG_ERR,MESSAGE_ARGUMENT_END);
		}
		strcat(buf, pos->name);
		if (pos->next) {
			buf[strlen(buf)] = FILE_SEPARATOR;
			buf[strlen(buf)] = '\0';
			len++;
		}
		
	}
	strcat(buf, MANGO_REQUIRE_SUFFIX);
}


static void make_search_path_impl(char *package_name,char *buf){
	size_t suffix_len = strlen(MANGO_IMPLEMENTATION_FUFFIX);
	size_t package_len = strlen(package_name);
	DBG_assert(suffix_len + package_len < FILENAME_MAX - (2 + suffix_len), "package name is too long(%s)", package_name);
	size_t i;
	for (i = 0; package_name[i] != '\0'; i++) {
		if (package_name[i] == '.') {
			buf[i] = FILE_SEPARATOR;
		}else{
			buf[i] = package_name[i];
		}
	}
	buf[i] = '\0';
	strcat(buf, MANGO_IMPLEMENTATION_FUFFIX);

}

static void get_requre_input(RequireList *req, char *found_path, SourceInput *source_input){
	char search_file[FILENAME_MAX];
	char *package_name = mgc_package_name_to_string(req->package_name);
	if (search_buildin_source(package_name, MGH_SOURCE, source_input)) {
		MEM_free(package_name);
		found_path[0] = '\0';
		return;
	}
	MEM_free(package_name);
	
	char *search_path = getenv("MANGO_REQUIRE_SEARCH_PATH");
	if (search_path == NULL) {
		search_path = ".";
	}
	
	make_search_path(req->line_number, req->package_name, search_file);
	FILE *fp;
	SearchFileStatus status = dvm_search_file(search_path, search_file, found_path, &fp);
	
	if (status != SEARCH_FILE_SUCCESS) {
		if (status == SEARCH_FILE_NOT_FOUND) {
			mgc_compile_error(req->line_number, REQUIRE_FILE_NOT_FOUND_ERR,STRING_MESSAGE_ARGUMENT,"file",search_file,MESSAGE_ARGUMENT_END);
		}else{
			mgc_compile_error(req->line_number, REQUIRE_FILE_ERR,INT_MESSAGE_ARGUMENT,"status",status,MESSAGE_ARGUMENT_END);
		}
		
	}
	source_input->mode = FILE_INPUT_MODE;
	source_input->u.file.fp = fp;

}


static DVM_Boolean add_exe_to_list(DVM_Executable *exe, DVM_ExecutableList *list){
	DVM_ExecutableItem *item = MEM_malloc(sizeof(*item));
	item->executable = exe;
	item->next = NULL;
	
	if (list->list == NULL) {
		list->list = item;
		return DVM_TRUE;
	}
	DVM_ExecutableItem *tail = NULL;
	DVM_ExecutableItem *pos = list->list;
	for (; pos; pos = pos->next) {
		if (dvm_equal_package_name(pos->executable->package_name, exe->package_name)
			&& pos->executable->is_required == exe->is_required) {
			return DVM_FALSE;
		}
		tail = pos;
	}
	
	tail->next = item;
	return DVM_TRUE;
}


static void set_path_to_compiler(MGC_Compiler *compiler, char *path){
	compiler->path = MEM_storage_malloc(compiler->compile_storage, strlen(path) +1 );
	strcpy(compiler->path, path);
}


static DVM_Executable *do_compiler(MGC_Compiler *compiler, DVM_ExecutableList *list, char *path, DVM_Boolean is_require){
	extern FILE *yyin;
	extern int yyparse(void);
	
	MGC_Compiler *compiler_backup = mgc_get_current_compiler();
	mgc_set_current_compiler(compiler);
	if (yyparse()) {
		fprintf(stderr,"Error! Error! Error!\n");
		exit(1);
	}
	
	
	char found_path[FILENAME_MAX];
	SourceInput source_input;
	for (RequireList *req_pos = compiler->require_list; req_pos; req_pos = req_pos->next) {
		 MGC_Compiler *req_compiler = search_compiler(st_compiler_list, req_pos->package_name);
		if (req_compiler) {
			compiler->required_list = add_compiler_to_list(compiler->required_list, req_compiler);
			continue;
		}
		req_compiler = mgc_create_compiler();
		req_compiler->package_name = req_pos->package_name;
		req_compiler->source_suffix = MGH_SOURCE;
		
		compiler->required_list = add_compiler_to_list(compiler->required_list, req_compiler);
		st_compiler_list = add_compiler_to_list(st_compiler_list, req_compiler);
		get_requre_input(req_pos, found_path, &source_input);
		set_path_to_compiler(req_compiler, found_path);
		req_compiler->input_mode = source_input.mode;
		if (req_compiler->input_mode == FILE_INPUT_MODE) {
			yyin = source_input.u.file.fp;
		}else{
			mgc_set_source_string(source_input.u.string.lines);
		}
		do_compiler(req_compiler, list, found_path, DVM_TRUE);
		
	}
	
	mgc_fix_tree(compiler);
	DVM_Executable *exe = mgc_generate(compiler);
	
	if (path) {
		exe->path = MEM_strdup(path);
	}else{
		exe->path = NULL;
	}
	
	exe->is_required = is_require;
	
	if (add_exe_to_list(exe, list)) {
		
	}
	
	mgc_set_current_compiler(compiler_backup);
	return exe;
}

static void dispose_compiler_list(void){
	CompilerList *temp;
	while (st_compiler_list) {
		temp = st_compiler_list->next;
		MEM_free(st_compiler_list);
		st_compiler_list = temp;
	}
}

DVM_ExecutableList *mgc_compile(MGC_Compiler *compiler, FILE *fp, char *path){
	extern FILE *yyin;
	DBG_assert(st_compiler_list == NULL, "st_compiler_list != NULL (%p)",st_compiler_list);
	set_path_to_compiler(compiler, path);
	compiler->input_mode = FILE_INPUT_MODE;
	
	yyin = fp;
	DVM_ExecutableList *list = MEM_malloc(sizeof(*list));
	list->list = NULL;
	
	DVM_Executable *exe = do_compiler(compiler, list, NULL, DVM_FALSE);
	exe->path = MEM_strdup(path);
	list->top_level = exe;
	
	dispose_compiler_list();
	mgc_rest_string_literal_buffer();
	
	return  list;
}


PackageName *create_one_package_name(MGC_Compiler *compiler, char *str, size_t start_idx, size_t end_idx){
	MEM_Storage storage = compiler->compile_storage;
	PackageName *pn = MEM_storage_malloc(storage, sizeof(*pn));
	pn->name = MEM_storage_malloc(storage, end_idx - start_idx + 1);
	
	size_t i = 0;
	for (; i < end_idx - start_idx; i++) {
		pn->name[i] = str[start_idx + i];
	}
	pn->name[i] = '\0';
	pn->name = NULL;
	return pn;
}


static PackageName *string_to_package_name(MGC_Compiler *compiler, char *str){
	PackageName *top = NULL;
	PackageName *tail = NULL;
	
	size_t start_idx = 0;
	for (size_t i = 0; ; i++) {
		char pos = str[i];
		if (pos == '.' || pos == '\0') {
			PackageName *pn = create_one_package_name(compiler, str, start_idx, i);
			if (top) {
				tail->next = pn;
			}else{
				top = pn;
			}
			
			tail = pn;
		}
		if (pos == '\0') {
			break;
		}
		
	}
	return top;
}





SearchFileStatus get_dynamic_load_input(char *package_name, char *found_path, char *search_file, SourceInput *source_input){
	if (search_buildin_source(package_name, MGM_SOURCE, source_input)) {
		found_path[0] = '\0';
		return SEARCH_FILE_SUCCESS;
	}
	char *search_path = getenv("MANGO_REQUIRE_SEARCH_PATH");
	if (search_path == NULL) {
		search_path = ".";
	}
	
	make_search_path_impl(package_name, search_file);
	FILE *fp;
	SearchFileStatus status = dvm_search_file(search_path, search_file, found_path, &fp);
	
	if (status != SEARCH_FILE_SUCCESS) {
		return status;
	}
	
	source_input->mode = FILE_INPUT_MODE;
	source_input->u.file.fp = fp;
	return SEARCH_FILE_SUCCESS;

}

SearchFileStatus mgc_dynamic_compile(MGC_Compiler *compiler, char *package_name,
									 DVM_ExecutableList *list, DVM_ExecutableItem **add_top,
									 char *search_file){
	char found_path[FILENAME_MAX];
	SourceInput source_input;
	SearchFileStatus status = get_dynamic_load_input(package_name, found_path, search_file, &source_input);
	if (status != SEARCH_FILE_SUCCESS) {
		return status;
	}
	
	DBG_assert(st_compiler_list == NULL, "st_compiler_list != NULL (%p)",st_compiler_list);
	
	DVM_ExecutableItem *tail = NULL;
	for (tail = list->list; tail->next; tail = tail->next)
		;
	compiler->package_name = string_to_package_name(compiler, package_name);
	set_path_to_compiler(compiler, found_path);
	compiler->input_mode = source_input.mode;
	
	if (source_input.mode == FILE_INPUT_MODE) {
		extern FILE *yyin;
		yyin = source_input.u.file.fp;
	}else{
		mgc_set_source_string(source_input.u.string.lines);
	}
	
	do_compiler(compiler, list, found_path, DVM_FALSE);
	
	dispose_compiler_list();
	mgc_rest_string_literal_buffer();
	
	*add_top = tail->next;
	return SEARCH_FILE_SUCCESS;
		
	
}


static CompilerList *traversal_compiler(CompilerList *list, MGC_Compiler *compiler){
	CompilerList *list_pos;
	for (list_pos = list; list_pos; list_pos = list_pos->next) {
		if (list_pos->compiler == compiler) {
			break;
		}
	}
	
	if (list_pos == NULL) {
		list = add_compiler_to_list(list, compiler);
	}
	
	for (CompilerList *pos = compiler->required_list; pos; pos = pos->next) {
		traversal_compiler(list, pos->compiler);
	}
	
	return list;
}

void mgc_dispose_compiler(MGC_Compiler *compiler){
	CompilerList *list = NULL;
	list = traversal_compiler(list, compiler);
	CompilerList *temp = NULL;
	for (CompilerList *pos = list; pos;) {
		for (FunctionDefinition *fun_pos = pos->compiler->function_list; fun_pos; fun_pos = fun_pos->next) {
			MEM_free(fun_pos->local_variable);
		}
		while (pos->compiler->required_list) {
			temp = pos->compiler->required_list;
			pos->compiler->required_list = temp->next;
			MEM_free(temp);
		}
		MEM_dispose_storage(pos->compiler->compile_storage);
		temp = pos->next;
		MEM_free(pos);
		pos = temp;
	}
	
}














