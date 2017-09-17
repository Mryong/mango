//
//  load.c
//  mango
//
//  Created by jerry.yong on 2017/8/30.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "MEM.h"
#include "dvm_pri.h"
#include "share.h"
#include <string.h>
#include "DVM_dev.h"
#include "DBG.h"
#include "MGC.h"


extern DVM_ObjectRef dvm_null_object_ref;
extern OpcodeInfo dvm_opcode_info[];



size_t dvm_search_function(DVM_VirtualMachine *dvm, char *package_name, char *name){
	for (size_t i = 0; i < dvm->function_count; i++) {
		if (dvm_equal_package_name(package_name, dvm->function[i]->package_name) && !strcmp(name, dvm->function[i]->name)) {
			return i;
		}
	}
	return FUNCTION_NOT_FOUND;
}


size_t dvm_search_enum(DVM_VirtualMachine *dvm, char *package_name, char *name){
	for (size_t i = 0; i <dvm->enum_count; i++) {
		if (dvm_equal_package_name(package_name, dvm->enum_[i]->package_name) && !strcmp(name, dvm->enum_[i]->name)) {
			return i;
		}
	}
	return ENUM_NOT_FOUND;
}

size_t dvm_sezrch_constant(DVM_VirtualMachine *dvm, char *package_name, char *name){
	for (size_t i = 0; i < dvm->constant_count; i++) {
		if (dvm_equal_package_name(package_name, dvm->constant[i]->package_name) && !strcmp(name, dvm->constant[i]->name)) {
			return i;
		}

	}
	return CONSTANT_NOT_FOUND;
}







static DVM_VTable *alloc_v_table(ExecClass *exe_class){
	DVM_VTable *table = MEM_malloc(sizeof(DVM_VTable));
	table->exec_class = exe_class;
	table->table = NULL;
	table->table_size = 0;
	return table;

}


static VTableItem st_array_method_v_table[] = {
	{ARRAY_PREFIX ARRAY_METHOD_SIZE, FUNCTION_NOT_FOUND},
	{ARRAY_PREFIX ARRAY_METHOD_RESIZE, FUNCTION_NOT_FOUND},
	{ARRAY_PREFIX ARRAY_METHOD_INSERT, FUNCTION_NOT_FOUND},
	{ARRAY_PREFIX ARRAY_METHOD_REMOVE, FUNCTION_NOT_FOUND},
	{ARRAY_PREFIX ARRAY_METHOD_ADD, FUNCTION_NOT_FOUND}
};

static VTableItem st_string_method_v_table[] = {
	{STRING_PREFIX STRING_METHOD_LENGTH, FUNCTION_NOT_FOUND},
	{STRING_PREFIX STRING_METHOD_SUBSTR, FUNCTION_NOT_FOUND}
};




static void set_built_in_methods(DVM_VirtualMachine *dvm){
	DVM_VTable *array_table = alloc_v_table(NULL);
	array_table->table_size = ARRAY_SZIE(st_array_method_v_table);
	array_table->table = MEM_malloc(sizeof(VTableItem) * array_table->table_size);
	for (size_t i = 0; i < array_table->table_size; i++) {
		array_table->table[i] = st_array_method_v_table[i];
		array_table->table[i].index = dvm_search_function(dvm, BUILT_IN_METHOD_PACKAGE_NAME, st_array_method_v_table[i].name);
	}
	dvm->array_v_table = array_table;
	
	
	DVM_VTable *string_table = alloc_v_table(NULL);
	string_table->table_size = ARRAY_SZIE(st_string_method_v_table);
	string_table->table = MEM_malloc(sizeof(VTableItem) * string_table->table_size);
	for (size_t i = 0; i < string_table->table_size; i++) {
		string_table->table[i] = st_string_method_v_table[i];
		string_table->table[i].index = dvm_search_function(dvm, BUILT_IN_METHOD_PACKAGE_NAME, st_string_method_v_table[i].name);
	}
	dvm->string_v_table = string_table;

}


void dvm_add_native_function(DVM_VirtualMachine *dvm, char *package_name, char *name, DVM_NativeFunctionProc *proc,
							 size_t argc, DVM_Boolean is_method, DVM_Boolean return_pointer){
	dvm->function = MEM_malloc(sizeof(Function) * (dvm->function_count + 1));
	dvm->current_function[dvm->function_count].is_implemented = DVM_TRUE;
	dvm->current_function[dvm->function_count].kind = NATIVE_FUNCTION;
	dvm->current_function[dvm->function_count].name = MEM_strdup(name);
	dvm->current_function[dvm->function_count].package_name = MEM_strdup(package_name);
	dvm->current_function[dvm->function_count].u.native_f.argc = argc;
	dvm->current_function[dvm->function_count].u.native_f.is_method = is_method;
	dvm->current_function[dvm->function_count].u.native_f.proc = proc;
	dvm->current_function[dvm->function_count].u.native_f.return_pointer = return_pointer;
	dvm->function_count++;
}
	


DVM_VirtualMachine *dvm_create_virtual_machine(){
	DVM_VirtualMachine *dvm = MEM_malloc(sizeof(DVM_VirtualMachine));
	dvm->stack.alloc_size = STACK_ALLOC_SIZE;
	dvm->stack.stack = MEM_malloc(sizeof(DVM_Value) * STACK_ALLOC_SIZE);
	dvm->stack.pointer_flags = MEM_malloc(sizeof(DVM_Boolean) * STACK_ALLOC_SIZE);
	dvm->stack.stack_pointer = 0;
	
	dvm->heap.current_threshold = HEAP_THRESHOLD_SIZE;
	dvm->heap.current_heap_size = 0;
	dvm->heap.header = NULL;
	
	dvm->current_executable = NULL;
	dvm->current_function = NULL;
	dvm->current_context = NULL;
	dvm->current_exception = dvm_null_object_ref;
	
	dvm->function = NULL;
	dvm->function_count = 0;
	dvm->class_ = NULL;
	dvm->class_count = 0;
	dvm->enum_ = NULL;
	dvm->enum_count = 0;
	dvm->constant = NULL;
	dvm->constant_count = 0;
	
	dvm->executable_list = NULL;
	dvm->executable_entry = NULL;
	dvm->top_level = NULL;
	dvm->free_context  = NULL;
	
	dvm_add_native_functions(dvm);
	set_built_in_methods(dvm);
	
	return dvm;
}


static void implement_mango_function(DVM_VirtualMachine *dvm, size_t dest_index, ExecutableEntry *entry, size_t index_in_enrty){
	dvm->function[dest_index]->is_implemented = DVM_TRUE;
	dvm->function[dest_index]->u.mango_f.exe = entry;
	dvm->function[dest_index]->u.mango_f.index = index_in_enrty;
}


static void add_functions(DVM_VirtualMachine *dvm, ExecutableEntry *entry){
	size_t function_count = entry->executable->function_count;
	size_t add_count = 0;
	DVM_Boolean *new_func_flags = MEM_malloc(sizeof(DVM_Boolean) * function_count);
	for (size_t i = 0; function_count; i++) {
		new_func_flags[i] = DVM_FALSE;
		size_t j = 0;
		for (; j < dvm->function_count; j++) {
			if (dvm_equal_package_name(entry->executable->function[i].package_name, dvm->function[j]->package_name)
				&& !strcmp(entry->executable->function[i].name, dvm->function[j]->name)) {
				
				if (entry->executable->function[i].is_implemented && dvm->function[j]->is_implemented) {
					//error
				}
				
				if (entry->executable->function[i].is_implemented) {
					implement_mango_function(dvm, j, entry, i);
				}
				break;
			}
		}
		if (j == dvm->function_count) {
			new_func_flags[i] = DVM_TRUE;
			add_count++;
		}
	}
	
	dvm->function = MEM_realloc(dvm->function, sizeof(Function *) * (dvm->function_count + add_count));
	size_t dest_index = dvm->function_count;
	size_t src_index= 0;
	for (; src_index < function_count; src_index++) {
		if (!new_func_flags[src_index]) {
			continue;
		}
		Function *function = MEM_malloc(sizeof(Function));
		dvm->function[dest_index] = function;
		function->is_implemented = entry->executable->function[src_index].is_implemented;
		function->name = MEM_strdup(entry->executable->function[src_index].name);
		function->kind = MANGO_FUNCTION;
		if (entry->executable->function[src_index].package_name) {
			function->package_name = MEM_strdup(entry->executable->function[src_index].package_name);
		}else{
			function->package_name = NULL;
		}
		if (entry->executable->function[src_index].is_implemented) {
			implement_mango_function(dvm, dest_index, entry, src_index);
		}
		
		dest_index++;
		
	}
	dvm->function_count += add_count;
	MEM_free(new_func_flags);
	
}


static void add_enum(DVM_VirtualMachine *dvm, ExecutableEntry *entry){
	size_t add_count = 0;
	size_t enum_count = entry->executable->enum_count;
	DVM_Boolean *new_enum_flags = MEM_malloc(sizeof(DVM_Boolean) * enum_count);
	for (size_t i = 0; i < enum_count; i++) {
		new_enum_flags[i] = DVM_FALSE;
		size_t j = 0;
		for (; j < dvm->enum_count; j++) {
			if (dvm_equal_package_name(entry->executable->enum_definition[i].package_name, dvm->enum_[j]->package_name)
				&& !strcmp(entry->executable->enum_definition[i].name, dvm->enum_[j]->name)) {
				if (entry->executable->enum_definition[i].is_defined && dvm->enum_[j]->is_defined) {
					//error
				}
				
				break;
			}
		}
		if (j == dvm->enum_count) {
			new_enum_flags[i] = DVM_TRUE;
			add_count++;
		}
	}
	
	dvm->enum_ = MEM_realloc(dvm->enum_, sizeof(Enum*)*(dvm->enum_count + add_count));
	size_t src_index = 0;
	size_t dest_index = dvm->enum_count;
	for (; src_index < enum_count; src_index++) {
		if (!new_enum_flags[src_index]) {
			continue;
		}
		Enum *enum_ = MEM_malloc(sizeof(Enum));
		dvm->enum_[dest_index] = enum_;
		if (entry->executable->enum_definition[src_index].package_name) {
			enum_->package_name = MEM_strdup(entry->executable->enum_definition[src_index].package_name);
		}else{
			enum_->package_name = NULL;
		}
		enum_->name = MEM_strdup(entry->executable->enum_definition[src_index].name);
		enum_->dvm_enum = &entry->executable->enum_definition[src_index];
		enum_->is_defined = entry->executable->enum_definition[src_index].is_defined;
		dest_index++;
	}
	dvm->enum_count += add_count;
	MEM_free(new_enum_flags);
}

static void add_constant(DVM_VirtualMachine *dvm, ExecutableEntry *entry){
	size_t add_count = 0;
	size_t constant_count = entry->executable->constant_count;
	DVM_Boolean *new_constant_flags = MEM_malloc(sizeof(DVM_Boolean) * constant_count);
	for (size_t i = 0; i < constant_count; i++) {
		new_constant_flags[i] = DVM_FALSE;
		size_t j = 0;
		for (; j < dvm->constant_count; j++) {
			if (dvm_equal_package_name(entry->executable->constant_definition[i].package_name, dvm->constant[j]->package_name)
				&& !strcmp(entry->executable->constant_definition[i].name, dvm->constant[j]->name)) {
				
				if (entry->executable->constant_definition[i].is_defined && dvm->constant[j]->is_defined) {
					//error
				}
				break;
			}
						   
		}
		
		if (j == dvm->constant_count) {
			new_constant_flags[i] = DVM_TRUE;
			add_count++;
		}
	}
	
	size_t src_index = 0;
	size_t dest_index = dvm->constant_count;
	dvm->constant = MEM_realloc(dvm->constant, sizeof(Constant *) * (dvm->constant_count + add_count));
	
	for (; src_index < constant_count; src_index++) {
		if (!new_constant_flags[src_index]) {
			continue;
		}
		Constant *constant = MEM_malloc(sizeof(Constant *));
		DVM_Constant *src = &entry->executable->constant_definition[src_index];
		if (src->package_name) {
			constant->package_name = strdup(src->package_name);
		}else{
			constant->package_name = NULL;
		}
		
		constant->name = strdup(src->name);
		
		constant->is_defined =  src->is_defined;
		
		dest_index++;
	}
	dvm->constant_count += add_count;
	MEM_free(new_constant_flags);

}



static DVM_Class *search_class_from_executable(DVM_Executable *exe, char *name){
	return NULL;
}

static size_t set_filed_type(DVM_Executable *exe, DVM_Class *pos, DVM_TypeSpecifier **types){
	size_t from_index = 0;
	if (pos->super_class) {
		from_index = set_filed_type(exe,search_class_from_executable(exe, pos->super_class->name) , types);
	}
	
	for (size_t i = 0; i < pos->field_count; i++) {
		types[from_index] = pos[i].field->type;
		from_index++;
	}
	
	return from_index;
	
}

static void add_fields(DVM_Executable *exe, DVM_Class *src, ExecClass *dest){
	size_t filed_count = 0;
	for (DVM_Class *pos = src; pos; pos = search_class_from_executable(exe,pos->super_class->name)) {
		filed_count += pos->field_count;
	}
	dest->field_count = filed_count;
	dest->field_type = MEM_malloc(sizeof(DVM_TypeSpecifier *) * filed_count);
	set_filed_type(exe, src, dest->field_type);
	
}


static void set_v_table(DVM_VirtualMachine *dvm, DVM_Class *class_p, DVM_Method *src, VTableItem *dest, DVM_Boolean set_name){
	if (set_name) {
		dest->name = MEM_strdup(src->name);
	}
	
	char *func_name = dvm_create_method_function_name(class_p->name, src->name);
	size_t func_index = dvm_search_function(dvm, class_p->package_name, func_name);
	if (func_index == FUNCTION_NOT_FOUND && !src->is_abstract) {
		//error
	}
	MEM_free(func_name);
	dest->index = func_index;
	
	
}



static size_t add_method(DVM_VirtualMachine *dvm, DVM_Executable *exe, DVM_Class *pos, DVM_VTable *v_table){
	size_t super_method_count = 0;
	if (pos->super_class) {
		super_method_count = add_method(dvm, exe, search_class_from_executable(exe, pos->super_class->name), v_table);
	}
	for (size_t i = 0; i < pos->method_count; i++) {
		size_t j = 0;
		for (; j < super_method_count; j++) {
			if (!strcmp(pos->method[i].name, v_table->table[j].name)) {
				set_v_table(dvm, pos, &pos->method[i], &v_table->table[j], DVM_FALSE);
				break;
			}
		}
		
		if (j == super_method_count && !pos->method[i].is_override) {
			v_table->table = MEM_realloc(v_table->table, sizeof(VTableItem *)* (v_table->table_size + 1));
			set_v_table(dvm, pos, &pos->method[i], &v_table->table[v_table->table_size], DVM_TRUE);
			v_table->table_size++;
		}
	}
	return v_table->table_size;
	
	
	
}

static void add_methods(DVM_VirtualMachine *dvm, DVM_Executable *exe, DVM_Class *src, ExecClass *dest){
	DVM_VTable *v_table = alloc_v_table(dest);
	add_method(dvm, exe, src, v_table);
	dest->class_table = v_table;
	dest->interface_count = src->interface_count;
	dest->interface_table = MEM_malloc(sizeof(DVM_VTable *) * src->interface_count);
	for (size_t i = 0; i < src->interface_count; i++) {
		DVM_Class *interface = search_class_from_executable(exe, src->interface_[i].name);
		dest->interface_table[i] = alloc_v_table(dest);
		dest->interface_table[i]->table = MEM_malloc(sizeof(VTableItem *) * interface->method_count);
		dest->interface_table[i]->table_size = interface->method_count;
		for (size_t j = 0; j < interface->method_count; j++) {
			set_v_table(dvm, src, &interface->method[j], &dest->interface_table[i]->table[j], DVM_TRUE);
		}
		
		
	}
	

}


static void add_class(DVM_VirtualMachine *dvm, DVM_Executable *exe, DVM_Class *src, ExecClass *dest){
	add_fields(exe, src, dest);
	add_methods(dvm, exe, src, dest);
}


size_t dvm_search_class(DVM_VirtualMachine *dvm, char *package_name, char *name){
	for (size_t i = 0; dvm->class_count; i++) {
		ExecClass *exe_class = dvm->class_[i];
		if (dvm_equal_package_name(exe_class->package_name, package_name)
			&& strcmp(exe_class->name, name)) {
			return i;
		}
	}
	//error
	return -1;
}


static void set_super_class(DVM_VirtualMachine *dvm, DVM_Executable *exe, size_t old_class_count){
	for (size_t i = old_class_count; i < dvm->class_count; i++) {
		ExecClass *exe_class = dvm->class_[i];
		DVM_Class *dvm_class = search_class_from_executable(exe, exe_class->name);
		if (dvm_class->super_class) {
			exe_class->super_class = dvm->class_[dvm_search_class(dvm, dvm_class->super_class->package_name, dvm_class->super_class->name)];
		}else{
			exe_class->super_class = NULL;
		}
		
		exe_class->interface_count = dvm_class->interface_count;
		exe_class->interface = MEM_malloc(sizeof(ExecClass *) * dvm_class->interface_count);
		for (size_t j = 0; j < dvm_class->interface_count; j++) {
			DVM_ClassIdentifier interface= dvm_class->interface_[j];
			exe_class->interface[j] = dvm->class_[dvm_search_class(dvm, interface.package_name, interface.name)];
		}
	}
}


static void add_classes(DVM_VirtualMachine *dvm, ExecutableEntry *entry){
	size_t add_count = 0;
	size_t class_count = entry->executable->class_count;
	DVM_Boolean *new_class_flags = MEM_malloc(sizeof(DVM_Boolean) * class_count);
	for (size_t i = 0; i < class_count; i++) {
		new_class_flags[i] = DVM_FALSE;
		size_t j = 0;
		for (; j < dvm->class_count; j++) {
			DVM_Class *src = &entry->executable->class_definition[i];
			ExecClass *dest = dvm->class_[j];
			if (dvm_equal_package_name(src->package_name, dest->package_name)
				&& !strcmp(src->name, dest->name)) {
				if (src->is_implemented && dest->is_implemented) {
					//error
				}
				if (src->is_implemented) {
					add_class(dvm, entry->executable, src, dest);
				}
				break;
			}
		}
		if (j == dvm->class_count) {
			new_class_flags[i] = DVM_TRUE;
			add_count++;
		}
	}
	
	dvm->class_ = MEM_malloc(sizeof(ExecClass *) * (dvm->class_count + add_count));
	size_t src_index = 0;
	size_t dest_index = dvm->class_count;

	for (; src_index < class_count; src_index++) {
		if (!new_class_flags[src_index]) {
			continue;
		}
		ExecClass *exe_class = MEM_malloc(sizeof(ExecClass));
		dvm->class_[dest_index] = exe_class;
		DVM_Class *src_class = &entry->executable->class_definition[src_index];
		if (src_class->package_name) {
			exe_class->package_name = MEM_strdup(src_class->package_name);
		}else{
			exe_class->package_name = NULL;
		}
		exe_class->name = MEM_strdup(src_class->name);
		exe_class->executable = entry;
		exe_class->is_implemented = src_class->is_implemented;
		exe_class->class_index = dest_index;
		if (src_class->is_implemented) {
			add_class(dvm, entry->executable, src_class, exe_class);
		}
		
		dest_index++;
	}
	size_t old_class_count  = dvm->class_count;
	dvm->class_count += add_count;
	set_super_class(dvm, entry->executable, old_class_count);
	MEM_free(new_class_flags);


}

static void conver_code(DVM_Byte *code, size_t code_size, DVM_Function *func){
	for (size_t i = 0; i < code_size; i++) {
		if (code[i] == DVM_PUSH_STACK_INT
			|| code[i] == DVM_PUSH_STACK_DOUBLE
			|| code[i] == DVM_PUSH_STACK_OBJECT
			|| code[i] == DVM_POP_STACK_INT
			|| code[i] == DVM_POP_STACK_DOUBLE
			|| code[i] == DVM_POP_STACK_OBJECT) {
			size_t param_count = 0;
			if (func->is_method) {
				param_count = func->parameter_count + 1;
			}else{
				param_count = func->parameter_count;
			}
			size_t  index = GET_2BYTE_INT(&code[i+1]);
			if (index > param_count) {
				index = index + CALL_INFO_ALIGN_ZISE;
				SET_2BYTE_INT(&code[i+1], index);
			}
			
		}
		OpcodeInfo info = dvm_opcode_info[code[i]];
		for (char *p = info.parameter; *p != '\0'; p++) {
			switch (*p) {
				case 'b':
					i += 1;
					break;
				case 's':
				case 'p':
					i += 2;
					break;
					
				default:
					DBG_assert(0, "parameter..%c", *p);
					break;
			}
		}
		
		
		
		
		
	}
}


static void add_reference_table(DVM_VirtualMachine *dvm, ExecutableEntry *entry, DVM_Executable *exe){
	
	entry->function_table = MEM_malloc(sizeof(size_t) * exe->function_count);
	for (size_t i = 0; i < exe->function_count; i++) {
		entry->function_table[i] = dvm_search_function(dvm, exe->function[i].package_name, exe->function[i].name);
	}
	
	entry->class_table = MEM_malloc(sizeof(size_t) * exe->class_count);
	for (size_t i = 0; exe->class_count; i++) {
		entry->class_table[i] = dvm_search_class(dvm, exe->class_definition[i].package_name, exe->class_definition[i].name);
	}
	
	entry->enum_table = MEM_malloc(sizeof(size_t) * exe->enum_count);
	for (size_t i = 0; i < exe->enum_count; i++) {
		entry->enum_table[i] = dvm_search_enum(dvm, exe->enum_definition[i].package_name, exe->enum_definition[i].name);
	}
	
	entry->constant_table = MEM_malloc(sizeof(size_t) * exe->constant_count);
	for (size_t i = 0; i < exe->constant_count; i++) {
		entry->constant_table[i] = dvm_sezrch_constant(dvm, exe->constant_definition[i].package_name, exe->constant_definition[i].name);
	}
	
	
	

}


static void add_static_variable(ExecutableEntry *entry, DVM_Executable *exe){
	entry->static_v.vars_count = exe->global_variable_count;
	entry->static_v.vars = MEM_malloc(sizeof(DVM_Value) * exe->global_variable_count);
	for (size_t i = 0; i < exe->global_variable_count; i++) {
		if (exe->global_variable[i].type->base_type == DVM_STRING_TYPE) {
			entry->static_v.vars[i].object = dvm_null_object_ref;
		}
	}
	
	for (size_t i = 0; i < exe->global_variable_count; i++) {
		dvm_initial_value(exe->global_variable[i].type, &entry->static_v.vars[i]);
	}

}

static ExecutableEntry *add_executable_to_dvm(DVM_VirtualMachine *dvm, DVM_Executable *executable, DVM_Boolean is_top_level){
	
	ExecutableEntry *entry = MEM_malloc(sizeof(ExecutableEntry));
	if (dvm->executable_entry == NULL) {
		dvm->executable_entry = entry;
	}else{
		ExecutableEntry *pos = dvm->executable_entry;
		for (; pos->next; pos = pos->next)
			;
		pos->next = entry;
	}
	
	add_functions(dvm, entry);
	add_enum(dvm, entry);
	add_constant(dvm, entry);
	add_classes(dvm, entry);
	
	conver_code(executable->top_level.code, executable->top_level.code_size, NULL);
	
	for (size_t i = 0; i < executable->function_count; i++) {
		DVM_Function *func = &executable->function[i];
		conver_code(func->code_block.code, func->code_block.code_size, func);
	}
	
	add_reference_table(dvm, entry, executable);
	add_static_variable(entry, executable);
	
	if (is_top_level) {
		dvm->top_level = entry;
	}
	
	return entry;
	


}

static void initialize_contant(DVM_VirtualMachine *dvm, ExecutableEntry *ee){
	dvm->current_executable = ee;
	dvm->current_function = NULL;
	dvm->pc= 0;
	dvm_expend_stack(dvm, ee->executable->constant_initializer.need_stack_size);
	dvm_execute_i(dvm, NULL, ee->executable->constant_initializer.code, ee->executable->constant_initializer.code_size, 0);

}


void DVM_set_executable(DVM_VirtualMachine *dvm, DVM_ExecutableList *list){
	dvm->executable_list = list;
	
	for (DVM_ExecutableItem *pos = list->list; pos; pos = pos->next) {
		ExecutableEntry *ee = add_executable_to_dvm(dvm,pos->executable, pos->executable == list->top_level);
		initialize_contant(dvm, ee);
	}
}



void dvm_dynamic_load(DVM_VirtualMachine *dvm, DVM_Executable *caller_exe, Function *caller, size_t pc, Function *func){
	if (func->package_name == NULL) {
		//error
	}
	
	char search_file[FILENAME_MAX];
	MGC_Compiler *compiler = mgc_create_compiler();
	DVM_ExecutableItem *add_top = NULL;
	SearchFileStatus status = mgc_dynamic_compile(compiler, func->package_name, dvm->executable_list, &add_top, search_file);
	if (status != SEARCH_FILE_SUCCESS) {
		//error
	}
	
	for (DVM_ExecutableItem *pos = add_top; pos; pos = pos->next) {
		ExecutableEntry  *ee = add_executable_to_dvm(dvm, pos->executable, DVM_FALSE);
		initialize_contant(dvm, ee);
	}
	
	mgc_dispose_compiler(compiler);
	


}




