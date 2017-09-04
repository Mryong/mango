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

extern DVM_ObjectRef dvm_null_object_ref;


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
	
	return dvm;
}
