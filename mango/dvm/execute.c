//
//  execute.c
//  mango
//
//  Created by jerry.yong on 2017/9/11.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "dvm_pri.h"
#include "MEM.h"
#include <math.h>
#include "DBG.h"


#define ST_TOP (((dvm)->stack.stack[dvm->stack.stack_pointer -1 ]))

#define STI(dvm, sp) ((dvm)->stack.stack[dvm->stack.stack_pointer + (sp)].int_value)
#define STD(dvm, sp) ((dvm)->stack.stack[dvm->stack.stack_pointer + (sp)].double_value)
#define STO(dvm, sp) ((dvm)->stack.stack[dvm->stack.stack_pointer + (sp)].object)


#define STI_I(dvm, sp) ((dvm)->stack.stack[sp].int_value)
#define STD_I(dvm, sp) ((dvm)->stack.stack[sp].double_value)
#define STO_I(dvm, sp) ((dvm)->stack.stack[sp].object)

#define STI_WRITE(dvm, sp, val) \
((dvm)->stack.stack[dvm->stack.stack_pointer + (sp)].int_value = (val),\
(dvm)->stack.pointer_flags[dvm->stack.stack_pointer + (sp)] = DVM_FALSE)

#define STD_WRITE(dvm, sp, val) \
((dvm)->stack.stack[dvm->stack.stack_pointer + (sp)].double_value = (val),\
(dvm)->stack.pointer_flags[dvm->stack.stack_pointer + (sp)] = DVM_FALSE)

#define STO_WRITE(dvm, sp, val) \
((dvm)->stack.stack[dvm->stack.stack_pointer + (sp)].object = (val),\
(dvm)->stack.pointer_flags[dvm->stack.stack_pointer + (sp)] = DVM_TRUE)


#define STI_WRITE_I(dvm, sp, val) \
((dvm)->stack.stack[sp].int_value = (val),\
(dvm)->stack.pointer_flags[sp] = DVM_FALSE)

#define STD_WRITE_I(dvm, sp, val) \
((dvm)->stack.stack[sp].double_value = (val),\
(dvm)->stack.pointer_flags[sp] = DVM_FALSE)

#define STO_WRITE_I(dvm, sp, val) \
((dvm)->stack.stack[sp].object = (val),\
(dvm)->stack.pointer_flags[sp] = DVM_TRUE)

extern DVM_ObjectRef dvm_null_object_ref;

static inline DVM_Boolean is_null_pointer(DVM_ObjectRef ref){
	return ref.data == NULL;
	
}



DVM_ObjectRef
create_array_sub(DVM_VirtualMachine *dvm, int dim, int dim_index,
				 DVM_TypeSpecifier *type)
{
	DVM_ObjectRef ret = {NULL,NULL};
	int size;
	int i;
	DVM_ObjectRef exception_dummy;
	
	size = STI(dvm, -dim);
	
	if (dim_index == type->derive_count-1) {
		switch (type->base_type) {
			case DVM_VOID_TYPE:
				DBG_panic(("creating void array"));
				break;
			case DVM_BOOLEAN_TYPE: /* FALLTHRU */
			case DVM_INT_TYPE:
			case DVM_ENUM_TYPE:
				ret = dvm_create_array_int_i(dvm, size);
				break;
			case DVM_DOUBLE_TYPE:
				ret = dvm_create_array_double_i(dvm, size);
				break;
			case DVM_STRING_TYPE: /* FALLTHRU */
			case DVM_NATIVE_POINTER_TYPE:
			case DVM_CLASS_TYPE:
			case DVM_DELEGAET_TYPE:
				ret = dvm_create_array_object_i(dvm, size);
				break;
			case DVM_NULL_TYPE: /* FALLTHRU */
			case DVM_BASE_TYPE: /* FALLTHRU */
			case DVM_UNSPECIFIED_IDENTIFIER_TYPE: /* FALLTHRU */
			default:
				DBG_assert(0, "type->basic_type..%d\n", type->base_type);
				break;
		}
	} else if (type->derive[dim_index].tag == DVM_FUNCTION_DERIVE) {
		DBG_panic(("Function type in array literal.\n"));
	} else {
		ret = dvm_create_array_object_i(dvm, size);
		if (dim_index < dim - 1) {
			STO_WRITE(dvm, 0, ret);
			dvm->stack.stack_pointer++;
			for (i = 0; i < size; i++) {
				DVM_ObjectRef child;
				child = create_array_sub(dvm, dim, dim_index+1, type);
				dvm_array_set_object(dvm, ret, i, child, &exception_dummy);
			}
			dvm->stack.stack_pointer--;
		}
	}
	return ret;
}

DVM_ObjectRef
create_array(DVM_VirtualMachine *dvm, int dim, DVM_TypeSpecifier *type)
{
	return create_array_sub(dvm, dim, 0, type);
}


DVM_ObjectRef
create_array_literal_int(DVM_VirtualMachine *dvm, int size)
{
	DVM_ObjectRef array;
	int i;
	
	array = dvm_create_array_int_i(dvm, size);
	for (i = 0; i < size; i++) {
		array.data->u.array.u.int_array[i] = STI(dvm, -size+i);
	}
	
	return array;
}

DVM_ObjectRef
create_array_literal_double(DVM_VirtualMachine *dvm, int size)
{
	DVM_ObjectRef array;
	int i;
	
	array = dvm_create_array_double_i(dvm, size);
	for (i = 0; i < size; i++) {
		array.data->u.array.u.double_array[i] = STD(dvm, -size+i);
	}
	
	return array;
}

DVM_ObjectRef
create_array_literal_object(DVM_VirtualMachine *dvm, int size)
{
	DVM_ObjectRef array;
	int i;
	
	array = dvm_create_array_object_i(dvm, size);
	for (i = 0; i < size; i++) {
		array.data->u.array.u.object_array[i] = STO(dvm, -size+i);
	}
	
	return array;
}

static void initialize_local_variable(DVM_VirtualMachine *dvm, DVM_Function *func, size_t from_sp);
static DVM_Value invoke_mango_from_native(DVM_VirtualMachine *dvm,Function *callee, DVM_ObjectRef obj, DVM_Value *args){
    ExecutableEntry *ee_backup	= dvm->current_executable;
	Function *func_backup = dvm->current_function;
	size_t pc_backup= dvm->pc;
	
	size_t base = dvm->stack.stack_pointer;
	DVM_Executable *dvm_exe = callee->u.mango_f.exe->executable;
	DVM_Function *dvm_func = &dvm_exe->function[callee->u.mango_f.index];
	for (size_t i = 0; i < dvm_func->parameter_count; i++) {
		dvm->stack.stack[dvm->stack.stack_pointer] = args[i];
		dvm->stack.pointer_flags[dvm->stack.stack_pointer] = is_pointer_type(dvm_func->parameter[i].type);
		dvm->stack.stack_pointer++;
	}
	if (!is_null_pointer(obj)) {
		STO_WRITE(dvm, 0, obj);
		dvm->stack.stack_pointer++;
	}
	
	CallInfo *call_info = (CallInfo *)&dvm->stack.stack[dvm->stack.stack_pointer];
	call_info->base = 0;
	call_info->caller = dvm->current_function;
	call_info->caller_address = CALL_FORM_NATIVE;
	for (size_t i =0; i < CALL_INFO_ALIGN_ZISE; i++) {
		dvm->stack.pointer_flags[dvm->stack.stack_pointer + i] = DVM_FALSE;
	}
	dvm->stack.stack_pointer += CALL_INFO_ALIGN_ZISE;
	
	initialize_local_variable(dvm, dvm_func, dvm->stack.stack_pointer);
	dvm->stack.stack_pointer += dvm_func->parameter_count;
	
	DVM_Value ret = dvm_execute_i(dvm, callee, dvm_func->code_block.code, dvm_func->code_block.code_size, base);
	dvm->stack.stack_pointer--;
	
	dvm->current_function = func_backup;
	dvm->current_executable = ee_backup;
	dvm->pc = pc_backup;
	
	return ret;
}

static void rest_stack_pointer(DVM_Function *func, size_t *sp_p, size_t base){
	if (func) {
		*sp_p = base + func->parameter_count + (func->is_method ? 1 : 0 ) + CALL_INFO_ALIGN_ZISE + func->local_variable_count;
	}else{
		*sp_p = 0;
	}
}

static DVM_Boolean check_instanceof(DVM_VirtualMachine *dvm, DVM_ObjectRef *obj,
				 size_t target_idx);
static DVM_Boolean throw_in_try(DVM_VirtualMachine *dvm, DVM_Executable *exe, ExecutableEntry *ee, Function *func,
								size_t *pc_p, size_t *sp_p, size_t base){
	DVM_Function *dvm_func = NULL;
	DVM_CodeBlock *cb;
	if (func) {
		dvm_func = &exe->function[func->u.mango_f.index];
		cb = &dvm_func->code_block;
	}else{
		cb = &exe->top_level;
	}
	size_t try_index = 0;
	for (; try_index < cb->try_size; try_index++) {
		if ((*pc_p) >= cb->try[try_index].try_start_pc && (*pc_p) <= cb->try[try_index].try_end_pc) {
			break;
		}
	}
	
	if (try_index == cb->try_size) {
		return DVM_FALSE;
	}
	size_t exception_class_index = dvm->current_exception.v_table->exec_class->class_index;
	DVM_CatchClause *catch = cb->try[try_index].catch_clause;
	size_t catch_count = cb->try[try_index].catch_count;
	for (size_t catch_index = 0; catch_index < catch_count; catch_index++) {
		size_t catch_class_index = catch[catch_index].class_index;
		size_t catch_start_pc = catch[catch_index].start_pc;
		if (exception_class_index == catch_class_index || check_instanceof(dvm, &dvm->current_exception, catch_class_index)) {
			(*pc_p) = catch_start_pc;
			rest_stack_pointer(dvm_func, sp_p, base);
			dvm->current_exception = dvm_null_object_ref;
			return DVM_TRUE;
		}
		
	}
	
	rest_stack_pointer(dvm_func, sp_p, base);
	(*pc_p) = cb->try[try_index].finally_start_pc;
	return DVM_TRUE;
}

static void add_stack_trace(DVM_VirtualMachine *dvm, DVM_Executable *exe, Function *func, size_t pc){
	int line_number = dvm_conv_pc_to_line_number(exe, func, pc);
	size_t class_index = dvm_search_class(dvm, DVM_MANGO_DEFAULT_PACKAGE, STACK_TRACE_CLASS);
	DVM_ObjectRef stack_trace = dvm_create_class_object_i(dvm, class_index);
	STO_WRITE(dvm, 0, stack_trace);
	dvm->stack.stack_pointer++;
	
	size_t line_number_index = dvm_get_field_index(dvm, stack_trace, "line_number");
	stack_trace.data->u.class_object.field[line_number_index].int_value = line_number;
	
	size_t file_name_index = dvm_get_field_index(dvm, stack_trace, "file_name");
	stack_trace.data->u.class_object.field[file_name_index].object = dvm_create_dvm_string_i(dvm, dvm_mbs2wcs(exe->path));
	
	size_t fun_name_index = dvm_get_field_index(dvm, stack_trace, "function_name");
	char *func_name;
	if (func) {
		func_name = exe->function[func->u.mango_f.index].name;
	}else{
		func_name = "top level";
	}
	stack_trace.data->u.class_object.field[fun_name_index].object = dvm_create_dvm_string_i(dvm, dvm_mbs2wcs(func_name));
	
	size_t stack_trace_index = dvm_get_field_index(dvm, dvm->current_exception, "stack_trace");
	
	DVM_ObjectRef stack_track_arr = dvm->current_exception.data->u.class_object.field[stack_trace_index].object;
	size_t arr_size = dvm_array_size(dvm, stack_trace.data);
	DVM_Value value;
	value.object = stack_trace;
	dvm_array_insert(dvm, stack_track_arr.data, arr_size, value);
	
	dvm->stack.stack_pointer--;
	
	
	
	
}


static DVM_Boolean do_return(DVM_VirtualMachine *dvm, Function **func_p, DVM_Byte **code_p, size_t *code_size_p, size_t *base_p, size_t *pc_p,
							 ExecutableEntry **ee_p, DVM_Executable **exe_p);
static DVM_Boolean do_throw(DVM_VirtualMachine *dvm, Function **func_p, DVM_Byte **code_p, size_t *code_p_size, size_t *pc_p,
							size_t *base_p, ExecutableEntry **ee_p, DVM_Executable **exe_p, DVM_ObjectRef *exception){
	dvm->current_exception = *exception;
	for (; ; ) {
		DVM_Boolean in_try  = throw_in_try(dvm, *exe_p, *ee_p, *func_p, pc_p, &dvm->stack.stack_pointer, *base_p);
		if (in_try) {
			break;
		}
		if (func_p) {
			add_stack_trace(dvm, *exe_p, *func_p, *pc_p);
			if (do_return(dvm, func_p, code_p, code_p_size, base_p, pc_p, ee_p, exe_p)) {
				return DVM_TRUE;
			}
		}else{
			add_stack_trace(dvm, *exe_p, *func_p, *pc_p);
			size_t func_index = dvm_search_function(dvm, DVM_MANGO_DEFAULT_PACKAGE, PRINT_STACK_TRACE_FUNC);
			invoke_mango_from_native(dvm, dvm->function[func_index], dvm->current_exception, NULL);
			exit(1);
			
		}
		
		
	}
	
	
	return DVM_TRUE;
}

static DVM_ObjectRef chain_string(DVM_VirtualMachine *dvm, DVM_ObjectRef str1, DVM_ObjectRef str2){
	DVM_Char *left_str;
	size_t left_len;
	DVM_Char *right_str;
	size_t right_len;
	if (str1.data == NULL) {
		left_str = NULL_STRING;
		left_len = dvm_wcslen(NULL_STRING);
	}else{
		left_str = str1.data->u.string.string;
		left_len = str1.data->u.string.length;
	}
	
	
	if (str2.data == NULL) {
		right_str = NULL_STRING;
		right_len = dvm_wcslen(NULL_STRING);
	}else{
		right_str = str2.data->u.string.string;
		right_len = str2.data->u.string.length;
	}
	
	size_t len = left_len + right_len;
	DVM_Char *str = MEM_malloc(sizeof(DVM_Char) * (len + 1));
	dvm_wcscpy(str, left_str);
	dvm_wcscat(str, right_str);
	 return dvm_create_dvm_string_i(dvm, str);
	
}

static DVM_Boolean throw_null_pointer_exception(DVM_VirtualMachine *dvm, Function **func_p, DVM_Byte **code_p, size_t *code_p_size, size_t *pc_p,
												 size_t *base_p, ExecutableEntry **ee_p, DVM_Executable **exe_p){
	DVM_ObjectRef exception = dvm_create_exception(dvm, DVM_NULL_POINTER_EXCEPTION_CLASS, NULL_POINTER_ERR, DVM_MESSAGE_ARGUMENT_END);
	STO_WRITE(dvm, 0, exception);
	dvm->stack.stack_pointer++;
	return do_throw(dvm, func_p, code_p, code_p_size, pc_p, base_p, ee_p, exe_p, &exception);
}

void dvm_expend_stack(DVM_VirtualMachine *dvm, size_t need_stack_size){
	size_t rest = dvm->stack.alloc_size - dvm->stack.stack_pointer;
	if (rest <= need_stack_size) {
		size_t revalue_up = ((need_stack_size/STACK_ALLOC_SIZE) + 1) * STACK_ALLOC_SIZE;
		dvm->stack.alloc_size += revalue_up;
		dvm->stack.stack = MEM_malloc(sizeof(DVM_Value) * dvm->stack.alloc_size);
		dvm->stack.pointer_flags = MEM_malloc(sizeof(DVM_Boolean) * dvm->stack.alloc_size);
	}
}

static void restore_pc(DVM_VirtualMachine *dvm, ExecutableEntry *ee, Function *func, size_t pc){
	dvm->current_executable = ee;
	dvm->current_function  = func;
	dvm->pc = pc;
}

DVM_ObjectRef dvm_create_exception(DVM_VirtualMachine *dvm, char *class_name, RuntimeError id, ...){
	va_list ap;
	va_start(ap, id);
	size_t class_index = dvm_search_class(dvm, DVM_MANGO_DEFAULT_PACKAGE, class_name);
	DVM_ObjectRef obj = dvm_create_class_object_i(dvm, class_index);
	STO_WRITE(dvm, 0, obj);
	dvm->stack.stack_pointer++;
	VString message;
	dvm_format_message(dvm_error_message_format, id, &message, ap);
	va_end(ap);
	
	return obj;

}

static void clear_stack_trace(DVM_VirtualMachine *dvm, DVM_ObjectRef *ex){
	 size_t stack_trace_index = dvm_get_field_index(dvm, *ex, "stack_trace");
	ex->data->u.class_object.field[stack_trace_index].object = dvm_create_array_object_i(dvm, 0);
}

static DVM_Boolean
check_instanceof_i(DVM_VirtualMachine *dvm, DVM_ObjectRef *obj,
				   size_t target_idx,
				   DVM_Boolean *is_interface, size_t *interface_idx)
{
	ExecClass *pos;
	int i;
	
	for (pos = obj->v_table->exec_class->super_class; pos;
		 pos = pos->super_class) {
		if (pos->class_index == target_idx) {
			*is_interface = DVM_FALSE;
			return DVM_TRUE;
		}
	}
	
	for (i = 0; i < obj->v_table->exec_class->interface_count; i++) {
		if (obj->v_table->exec_class->interface[i]->class_index
			== target_idx) {
			*is_interface = DVM_TRUE;
			*interface_idx = i;
			return DVM_TRUE;
		}
	}
	return DVM_FALSE;
}


static DVM_Boolean
check_instanceof(DVM_VirtualMachine *dvm, DVM_ObjectRef *obj,
				 size_t target_idx)
{
	DVM_Boolean is_interface_dummy;
	size_t interface_idx_dummy;
	
	return check_instanceof_i(dvm, obj, target_idx,
							  &is_interface_dummy, &interface_idx_dummy);
}
static DVM_Context *create_context(DVM_VirtualMachine *dvm){
	DVM_Context *context = MEM_malloc(sizeof(DVM_Context));
	context->ref_in_native_method = NULL;
	return context;
}

DVM_Context *dvm_push_context(DVM_VirtualMachine *dvm){
	DVM_Context *context = create_context(dvm);
	context->next = dvm->current_context;
	dvm->current_context = context;
	return context;
}

DVM_Context *dvm_create_context(DVM_VirtualMachine *dvm){
	DVM_Context *context = create_context(dvm);
	context->next = dvm->free_context;
	dvm->free_context = context;
	return context;
}

static void dispose_context(DVM_Context *context){
	for (RefNativeFunc *pos = context->ref_in_native_method; pos; ) {
		RefNativeFunc *temp = pos;
		pos = pos->next;
		MEM_free(temp);
	}
	MEM_free(context);
}

void dvm_pop_context(DVM_VirtualMachine *dvm, DVM_Context *context){
	DBG_assert(context == dvm->current_context, "contxt..%p,dvm->current_context..%p",context,dvm->current_context);
	dvm->current_context = context->next;
	dispose_context(context);
}

void dvm_dispose_context(DVM_VirtualMachine *dvm, DVM_Context *context){
	DVM_Context *pre = NULL;
	DVM_Context *pos = NULL;
	for (pos = dvm->current_context; pos; pos = pos->next) {
		if (pos == context) {
			break;
		}
		pre = pos;
	}
	if (pre == NULL) {
		dvm->free_context = context->next;
	}else{
		pre->next = context->next;
	}
	dispose_context(context);
	
	
}

static void invoke_native_function(DVM_VirtualMachine *dvm, Function *caller, Function *callee, size_t pc, size_t *sp_p, size_t base){
	(*sp_p)--;
	DVM_Value *stack = dvm->stack.stack;
	DBG_assert(callee->kind == NATIVE_FUNCTION, "callee->kind..%d",callee->kind);
	size_t arg_count;
	if (callee->u.native_f.is_method) {
		arg_count = callee->u.native_f.argc + 1;
	}else{
		arg_count = callee->u.native_f.argc;
	}
	
	DVM_Context *context = dvm_push_context(dvm);
	CallInfo *info = (CallInfo *)&dvm->stack.stack[*sp_p];
	info->caller = caller;
	info->base = base;
	info->caller_address = pc;
	for (size_t i = 0; i< CALL_INFO_ALIGN_ZISE; i++) {
		dvm->stack.pointer_flags[*sp_p + i] = DVM_FALSE;
	}
	*sp_p += CALL_INFO_ALIGN_ZISE;
	dvm->current_function = callee;
	DVM_Value ret_val = callee->u.native_f.proc(dvm,context,callee->u.native_f.argc,&stack[*sp_p - arg_count - CALL_INFO_ALIGN_ZISE]);
	dvm->current_function = caller;
	*sp_p = *sp_p - arg_count - CALL_INFO_ALIGN_ZISE;
	stack[*sp_p] = ret_val;
	dvm->stack.pointer_flags[*sp_p] = callee->u.native_f.return_pointer;
	(*sp_p)++;
	dvm_pop_context(dvm, context);
}

static void initialize_local_variable(DVM_VirtualMachine *dvm, DVM_Function *func, size_t from_sp){
	for (size_t i = 0; i < func->parameter_count; i++) {
		DVM_TypeSpecifier *type = func->parameter[from_sp+i].type;
		dvm_initial_value(type, &dvm->stack.stack[from_sp + i]);
		if (is_pointer_type(type)) {
			dvm->stack.pointer_flags[from_sp + i] = DVM_TRUE;
		}else{
			dvm->stack.pointer_flags[from_sp + i] = DVM_FALSE;
		}
	}
}

static void invoke_mango_function(DVM_VirtualMachine *dvm, Function **caller_p,Function *callee, DVM_Byte **code_p, size_t *code_size_p,
								  size_t *pc_p, size_t *sp_p, size_t *base_p, ExecutableEntry **ee_p, DVM_Executable **exe_p){
	if (!callee->is_implemented) {
		dvm_dynamic_load(dvm, *exe_p, *caller_p, *pc_p, callee);
	}
	*ee_p = callee->u.mango_f.exe;
	*exe_p = (*ee_p)->executable;
	DVM_Function *dvm_callee = &(*exe_p)->function[callee->u.mango_f.index];
	dvm_expend_stack(dvm, CALL_INFO_ALIGN_ZISE + dvm_callee->local_variable_count + dvm_callee->code_block.need_stack_size);
	CallInfo *call_info = (CallInfo *)&dvm->stack.stack[*sp_p -1];
	call_info->base = (int)(*base_p);
	call_info->caller = *caller_p;
	call_info->caller_address = *pc_p;
	for (size_t i = 0; i < CALL_INFO_ALIGN_ZISE; i++) {
		dvm->stack.pointer_flags[*sp_p - 1 + i] = DVM_FALSE;
	}
	*base_p = *sp_p - 1 - dvm_callee->parameter_count;
	if (dvm_callee->is_method) {
		*base_p = *base_p -1;
	}
	*caller_p = callee;
	
	initialize_local_variable(dvm, dvm_callee, *sp_p - 1 + CALL_INFO_ALIGN_ZISE);
	*sp_p = *sp_p + CALL_INFO_ALIGN_ZISE + dvm_callee->local_variable_count - 1;
	*pc_p = 0;
	*code_p = dvm_callee->code_block.code;
	*code_size_p = dvm_callee->code_block.code_size;
	
	
	
	
}

static DVM_Boolean do_return(DVM_VirtualMachine *dvm, Function **func_p, DVM_Byte **code_p, size_t *code_size_p, size_t *base_p, size_t *pc_p,
					  ExecutableEntry **ee_p, DVM_Executable **exe_p){
	DVM_Function *dvm_func = &(*exe_p)->function[(*func_p)->u.mango_f.index];
	size_t arg_count =  dvm_func->parameter_count;
	if (dvm_func->is_method) {
		arg_count++;
	}
	CallInfo *call_info = (CallInfo *)&dvm->stack.stack[*base_p + arg_count];
	if (call_info->caller) {
		*ee_p = call_info->caller->u.mango_f.exe;
		*exe_p = (*ee_p)->executable;
		if (call_info->caller->kind == MANGO_FUNCTION) {
			DVM_Function *dvm_caller_func = &(*exe_p)->function[call_info->caller->u.mango_f.index];
			*code_p = dvm_caller_func->code_block.code;
			*code_size_p = dvm_caller_func->code_block.code_size;
		}
	}else{
		*ee_p = dvm->top_level;
		*exe_p = (*ee_p)->executable;
		*code_p = dvm->top_level->executable->top_level.code;
		*code_size_p = dvm->top_level->executable->top_level.code_size;
	}
	dvm->stack.stack_pointer = *base_p;
	*func_p = call_info->caller;
	*base_p = call_info->base;
	*pc_p = call_info->caller_address + 1;
	return call_info->caller_address == CALL_FORM_NATIVE;
	
}

static DVM_Boolean rerurn_function(DVM_VirtualMachine *dvm, Function **func_p, DVM_Byte **code_p,
							size_t *code_szie_p,size_t *base_p, size_t *pc_p,
							ExecutableEntry **ee_p, DVM_Executable **exe_p){
	DVM_Value ret_value = dvm->stack.stack[dvm->stack.stack_pointer - 1];
	dvm->stack.stack_pointer--;
	DVM_Function *dvm_func = &(*exe_p)->function[(*func_p)->u.mango_f.index];

	DVM_Boolean ret = do_return(dvm, func_p, code_p, code_szie_p, base_p, pc_p, ee_p, exe_p);
	
	dvm->stack.stack[dvm->stack.stack_pointer] = ret_value;
	dvm->stack.pointer_flags[dvm->stack.stack_pointer] = is_pointer_type(dvm_func->type);
	dvm->stack.stack_pointer++;
	
	return ret;
}



static DVM_ErrorStatus
check_down_cast(DVM_VirtualMachine *dvm, DVM_ObjectRef *obj, size_t target_idx,
				DVM_Boolean *is_same_class,
				DVM_Boolean *is_interface, size_t *interface_index)
{
	if (obj->v_table->exec_class->class_index == target_idx) {
		*is_same_class = DVM_TRUE;
		return DVM_SUCCESS;
	}
	*is_same_class = DVM_FALSE;
	
	if (!check_instanceof_i(dvm, obj, target_idx,
							is_interface, interface_index)) {
		return DVM_ERROR;
	}
	
	return DVM_SUCCESS;
}

void dvm_print_code(DVM_Byte *code, size_t code_size){
	for (size_t i = 0; i < code_size; i++) {
		DVM_Opcode op_code = code[i];
		char *param = dvm_opcode_info[op_code].parameter;
		char *name = dvm_opcode_info[op_code].mnemonic;
		printf("%s(%d) ",name, op_code);
		for (size_t j = 0; param[j] != '\0'; j++) {
			switch (param[j]) {
				case 'b':
					printf("%d",code[i +1]);
					i++;
					break;
				case 'p':
				case 's':
					printf("%d", GET_2BYTE_INT(&code[i +1]));
					i+=2;
					break;
					
				default:
					break;
			}
		}
		printf("\n");
	}
}


DVM_Value dvm_execute_i(DVM_VirtualMachine *dvm, Function *func, DVM_Byte *code, size_t code_size, size_t base){
	int x = DVM_PUSH_STACK_OBJECT;
	ExecutableEntry *ee = dvm->current_executable;
	DVM_Executable *exe = ee->executable;
	size_t pc = dvm->pc;
	DVM_Value ret;
	while (pc < code_size) {
		switch ((DVM_Opcode)code[pc]) {
			case DVM_PUSH_INT_1BYTE:
				STI_WRITE(dvm, 0, code[pc +1]);
				pc += 2;
				dvm->stack.stack_pointer++;
				break;
			case DVM_PUSH_INT_2BYTE:
				STI_WRITE(dvm, 0, GET_2BYTE_INT(&code[pc+1]));
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			case DVM_PUSH_INT:
				STI_WRITE(dvm, 0, exe->constant_pool[GET_2BYTE_INT(&code[pc + 1])].u.c_int);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			case DVM_PUSH_DOUBLE_0:
				STD_WRITE(dvm, 0, 0.0);
				pc++;
				dvm->stack.stack_pointer++;
				break;
			case DVM_PUSH_DOUBLE_1:
				STD_WRITE(dvm, 0, 1.0);
				pc++;
				dvm->stack.stack_pointer++;
				break;
			case DVM_PUSH_DOUBLE:
				STD_WRITE(dvm, 0, exe->constant_pool[GET_2BYTE_INT(&code[pc + 1])].u.c_double);
				pc+=3;
				dvm->stack.stack_pointer++;
				break;
			case DVM_PUSH_STRING:{
				DVM_Char *str = exe->constant_pool[GET_2BYTE_INT(&code[pc + 1])].u.c_string;
				DVM_ObjectRef ref = dvm_create_dvm_string_i(dvm, str);
				STO_WRITE(dvm, 0, ref);
				pc+=3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_NULL:
				STO_WRITE(dvm, 0, dvm_null_object_ref);
				pc++;
				dvm->stack.stack_pointer++;
				break;
			case DVM_PUSH_STACK_INT:{
				int index = GET_2BYTE_INT(&code[pc + 1]);
				int value = STI_I(dvm, base + index);
				STI_WRITE(dvm, 0, value);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_STACK_DOUBLE:{
				int index = GET_2BYTE_INT(&code[pc + 1]);
				double value = STD_I(dvm, base + index);
				STD_WRITE(dvm, 0, value);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_STACK_OBJECT:{
				int index= GET_2BYTE_INT(&code[pc + 1]);
				DVM_ObjectRef ref = STO_I(dvm, base + index);
				STO_WRITE(dvm, 0, ref);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_POP_STACK_INT:{
				int value = STI(dvm, -1);
				int index= GET_2BYTE_INT(&code[pc + 1]);
				STI_WRITE_I(dvm, base + index, value);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_POP_STACK_DOUBLE:{
				double value = STD(dvm, -1);
				int index= GET_2BYTE_INT(&code[pc + 1]);
				STD_WRITE_I(dvm, base + index, value);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_POP_STACK_OBJECT:{
				DVM_ObjectRef ref= STO(dvm, -1);
				int index= GET_2BYTE_INT(&code[pc +1]);
				STO_WRITE_I(dvm, base + index, ref);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_STATIC_INT:{
				int index= GET_2BYTE_INT(&code[pc + 1]);
				int value = ee->static_v.vars[index].int_value;
				STI_WRITE(dvm, 0, value);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_STATIC_DOUBLE:{
				int index= GET_2BYTE_INT(&code[pc + 1]);
				double value = ee->static_v.vars[index].double_value;
				STD_WRITE(dvm, 0, value);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_STATIC_OBJECT:{
				int index= GET_2BYTE_INT(&code[pc + 1]);
				DVM_ObjectRef ref = ee->static_v.vars[index].object;
				STO_WRITE(dvm, 0, ref);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_POP_STATIC_INT:{
				int value = STI(dvm, -1);
				int index = GET_2BYTE_INT(&code[pc + 1]);
				ee->static_v.vars[index].int_value = value;
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_POP_STATIC_DOUBLE:{
				double value = STD(dvm, -1);
				int index= GET_2BYTE_INT(&code[pc + 1]);
				ee->static_v.vars[index].double_value = value;
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_POP_STATIC_OBJECT:{
				DVM_ObjectRef ref = STO(dvm, -1);
				int index= GET_2BYTE_INT(&code[pc + 1]);
				ee->static_v.vars[index].object = ref;
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_CONSTANT_INT:{
				int index = GET_2BYTE_INT(&code[pc + 1]);
				size_t index_g = ee->constant_table[index];
				int value = dvm->constant[index_g]->value.int_value;
				STI_WRITE(dvm, 0, value);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_CONSTANT_DOUBLE:{
				int index = GET_2BYTE_INT(&code[pc + 1]);
				size_t index_g = ee->constant_table[index];
				double value = dvm->constant[index_g]->value.double_value;
				STD_WRITE(dvm, 0, value);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_CONSTANT_OBJECT:{
				int index = GET_2BYTE_INT(&code[pc + 1]);
				size_t index_g = ee->constant_table[index];
				DVM_ObjectRef ref = dvm->constant[index_g]->value.object;
				STO_WRITE(dvm, 0, ref);
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_POP_CONSTANT_INT:{
				int value = STI(dvm, -1);
				int index= GET_2BYTE_INT(&code[pc + 1]);
				size_t index_g = ee->constant_table[index];
				dvm->constant[index_g]->value.int_value = value;
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_POP_CONSTANT_DOUBLE:{
				double value = STD(dvm, -1);
				int index = GET_2BYTE_INT(&code[pc + 1]);
				size_t index_g = ee->constant_table[index];
				dvm->constant[index_g]->value.double_value = value;
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_POP_CONSTANT_OBJECT:{
				DVM_ObjectRef ref = STO(dvm, -1);
				int index = GET_2BYTE_INT(&code[pc + 1]);
				size_t index_g = ee->constant_table[index];
				dvm->constant[index_g]->value.object = ref;
				pc += 3;
				dvm->stack.stack_pointer++;
				break;
			}
			case DVM_PUSH_ARRAY_INT:{
				DVM_ObjectRef arr = STO(dvm, -2);
				int index = STI(dvm, -1);
				restore_pc(dvm, ee, func, pc);
				int value;
				DVM_ObjectRef exception;
				restore_pc(dvm, ee, func, pc);
				DVM_ErrorStatus ststus = dvm_array_get_int(dvm, arr, index, &value, &exception);
				if (ststus == DVM_SUCCESS) {
					STI_WRITE(dvm, -2, value);
					dvm->stack.stack_pointer--;
					pc++;
				}else{
					
				}
				break;
			}
			case DVM_PUSH_ARRAY_DOUBLE:{
				DVM_ObjectRef arr = STO(dvm, -2);
				int index = STI(dvm, -1);
				restore_pc(dvm, ee, func, pc);
				double value;
				DVM_ObjectRef exception;
				restore_pc(dvm, ee, func, pc);
				DVM_ErrorStatus status = dvm_array_get_double(dvm, arr, index, &value, &exception);
				if (status == DVM_SUCCESS) {
					STD_WRITE(dvm, -2, value);
					dvm->stack.stack_pointer--;
					pc++;
				}else{
					
				}
				break;
			}
			case DVM_PUSH_ARRAY_OBJECT:{
				DVM_ObjectRef arr = STO(dvm, -2);
				int index = STI(dvm, -1);
				DVM_ObjectRef value;
				DVM_ObjectRef exception;
				restore_pc(dvm, ee, func, pc);
				DVM_ErrorStatus status = dvm_array_get_object(dvm, arr, index, &value, &exception);
				if (status == DVM_SUCCESS) {
					STO_WRITE(dvm, -2, value);
					dvm->stack.stack_pointer--;
					pc++;
				}else{
					
				}
				break;
			}
			case DVM_POP_ARRAY_INT:{
				int value = STI(dvm, -3);
				DVM_ObjectRef arr = STO(dvm, -2);
				int index = STI(dvm, -1);
				DVM_ObjectRef exception;
				restore_pc(dvm, ee, func, pc);
				DVM_ErrorStatus status = dvm_array_set_int(dvm, arr, index, value, &exception);
				if (status == DVM_SUCCESS) {
					dvm->stack.stack_pointer -= 3;
					pc++;
				}else{
					
					
				}
				break;
			}
			case DVM_POP_ARRAY_DOUBLE:{
				double value = STD(dvm, -3);
				DVM_ObjectRef arr = STO(dvm, -2);
				int index = STI(dvm, -1);
				DVM_ObjectRef exception;
				restore_pc(dvm, ee, func, pc);
				DVM_ErrorStatus status = dvm_array_set_double(dvm, arr, index, value, &exception);
				if (status == DVM_SUCCESS) {
					dvm->stack.stack_pointer -= 3;
					pc++;
				}else{
					
				}
				break;
			}
			case DVM_POP_ARRAY_OBJECT:{
				DVM_ObjectRef value = STO(dvm, -3);
				DVM_ObjectRef arr = STO(dvm, -2);
				int index = STI(dvm, -1);
				DVM_ObjectRef exception;
				restore_pc(dvm, ee, func, pc);
				DVM_ErrorStatus status = dvm_array_set_object(dvm, arr, index, value, &exception);
				if (status == DVM_SUCCESS) {
					dvm->stack.stack_pointer -= 3;
					pc++;
				}else{
					
					
				}
				break;
				
			}
			case DVM_PUSH_CHARACTER_IN_STRING:{
				DVM_ObjectRef str = STO(dvm, -2);
				int index = STI(dvm, -1);
				DVM_Char value;
				DVM_ObjectRef exception;
				restore_pc(dvm, ee, func, pc);
				DVM_ErrorStatus status = dvm_get_character(dvm, str, index, &value, &exception);
				if (status == DVM_SUCCESS) {
					STI_WRITE(dvm, -2, value);
					dvm->stack.stack_pointer--;
					pc++;
				}else{
					
				}
				break;
			}
			case DVM_PUSH_FIELD_INT:{
				DVM_ObjectRef obj = STO(dvm, -1);
				int index = GET_2BYTE_INT(&code[pc+1]);
				if (is_null_pointer(obj)) {
					if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
						goto EXECUTE_END;
					}
				}else{
					int	value = obj.data->u.class_object.field[index].int_value;
					STI_WRITE(dvm, -1, value);
					pc += 3;
				}
				
				break;
			}
			case DVM_PUSH_FIELD_DOUBLE:{
				DVM_ObjectRef obj = STO(dvm, -1);
				int index= GET_2BYTE_INT(&code[pc + 1]);
				if (is_null_pointer(obj)) {
					if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
						if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
							goto EXECUTE_END;
						}
					}
				}else{
					double value = obj.data->u.class_object.field[index].double_value;
					STD_WRITE(dvm, -1, value);
					pc += 3;
				}
				break;
			}
			case DVM_PUSH_FIELD_OBJECT:{
				DVM_ObjectRef obj = STO(dvm, -1);
				int index = GET_2BYTE_INT(&code[pc + 1]);
				if (is_null_pointer(obj)) {
					if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
						if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
							goto EXECUTE_END;
						}
					}
				}else{
					DVM_ObjectRef value = obj.data->u.class_object.field[index].object;
					STO_WRITE(dvm, -1, value);
					pc += 3;
				}
				break;
			}
			case DVM_POP_FIELD_INT:{
				int value = STI(dvm, -2);
				DVM_ObjectRef obj = STO(dvm, -1);
				int index = GET_2BYTE_INT(&code[pc + 1]);
				if (is_null_pointer(obj)) {
					if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
						if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
							goto EXECUTE_END;
						}
					}
				}else{
					obj.data->u.class_object.field[index].int_value = value;
					dvm->stack.stack_pointer -= 2;
					pc += 3;
				}
				break;
			}
				
			case DVM_POP_FIELD_DOUBLE:{
				double value = STD(dvm, -2);
				DVM_ObjectRef obj = STO(dvm, -1);
				int index = GET_2BYTE_INT(&code[pc + 1]);
				if (is_null_pointer(obj)) {
					if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
						if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
							goto EXECUTE_END;
						}
					}
				}else{
					obj.data->u.class_object.field[index].double_value = value;
					dvm->stack.stack_pointer -= 2;
					pc += 3;
				}
				break;
			}
			
			case DVM_POP_FIELD_OBJECT:{
				DVM_ObjectRef value = STO(dvm, -2);
				DVM_ObjectRef obj = STO(dvm, -1);
				int index = GET_2BYTE_INT(&code[pc + 1]);
				if (is_null_pointer(obj)) {
					if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
						if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
							goto EXECUTE_END;
						}
					}
				}else{
					obj.data->u.class_object.field[index].object = value;
					dvm->stack.stack_pointer -= 2;
					pc += 3;
				}
				break;
			}
			case DVM_ADD_INT:{
				int v1 = STI(dvm, -2);
				int v2 = STI(dvm, -1);
				STI_WRITE(dvm, -2, v1 + v2);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_ADD_DOUBLE:{
				double v1 = STD(dvm, -2);
				double v2 = STD(dvm, -1);
				STD_WRITE(dvm, -2, v1 + v2);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_ADD_STRING:{
				STO(dvm, -2) = chain_string(dvm, STO(dvm, -2), STO(dvm, -1));
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			
			case DVM_SUB_INT:{
				int v1 = STI(dvm, -2);
				int v2 = STI(dvm, -1);
				STI_WRITE(dvm, -2, v1 - v2);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_SUB_DOUBLE:{
				double v1 = STD(dvm, -2);
				double v2 = STD(dvm, -1);
				STD_WRITE(dvm, -2, v1 - v2);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_MUL_INT:{
				int v1 = STI(dvm, -2);
				int v2 = STI(dvm, -1);
				STI_WRITE(dvm, -2, v1 * v2);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_MUL_DOUBLE:{
				double v1 = STD(dvm, -2);
				double v2 = STD(dvm, -1);
				STD_WRITE(dvm, -2, v1 * v2);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_DIV_INT:{
				int v2 = STI(dvm, -1);
				if (v2 == 0) {
					DVM_ObjectRef exception = dvm_create_exception(dvm, DIVISION_BY_ZERO_EXCEPTION_CLASS, DIVISION_BY_ZERO_ERR,DVM_MESSAGE_ARGUMENT_END);
					if (do_throw(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe, &exception)) {
						goto EXECUTE_END;
					}
				}else{
					STI(dvm, -2) = STI(dvm, -1) / STI(dvm, -2);
					dvm->stack.stack_pointer--;
					pc++;
				}
				break;
			}
			case DVM_DIV_DOUBLE:{
				double v2 = STD(dvm, -1);
				if (v2 == 0) {
					DVM_ObjectRef exception = dvm_create_exception(dvm, DIVISION_BY_ZERO_EXCEPTION_CLASS, DIVISION_BY_ZERO_ERR,DVM_MESSAGE_ARGUMENT_END);
					if (do_throw(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe, &exception)) {
						goto EXECUTE_END;
					}
				}else{
					STD(dvm, -2) = STD(dvm, -1) / STD(dvm, -2);
					dvm->stack.stack_pointer--;
					pc++;
				}
				break;
			}
			case DVM_MOD_INT:{
				int v1 = STI(dvm, -2);
				int v2 = STI(dvm, -1);
				STI_WRITE(dvm, -2, v1 % v2);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_MOD_DOUBLE:{
				double v1 = STD(dvm, -2);
				double v2 = STD(dvm, -1);
				STD_WRITE(dvm, -2, fmod(v1, v2));
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_BIT_AND:{
				STI(dvm, -2) = STI(dvm, -2) & STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_BIT_OR:{
				STI(dvm, -2) = STI(dvm, -2) | STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_BIT_XOR:{
				STI(dvm, -2) = STI(dvm, -2) ^ STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_MINUS_INT:{
				STI(dvm, -1) = -STI(dvm, -1);
				pc++;
				break;
			}
			case DVM_MINUS_DOUBLE:{
				STD(dvm, -1) = - STD(dvm, -1);
				pc++;
				break;
			}
			case DVM_BIT_NOT:{
				STI(dvm, -1) = ~STI(dvm, -1);
				pc++;
				break;
			}
			case DVM_INCREMENT:{
				STI(dvm, -1)++;
				pc++;
				break;
			}
			case DVM_DECREMENT:{
				STI(dvm, -1)--;
				pc++;
				break;
			}
			case DVM_CAST_INT_TO_DOUBLE:{
				STD(dvm, -1) = (double)STI(dvm, -1);
				pc++;
				break;
			}
			case DVM_CAST_DOUBLE_TO_INT:{
				STI(dvm, -1) = (int)STD(dvm, -1);
				pc++;
				break;
			}
			case DVM_CAST_BOOLEAN_TO_STRING:{
				if (STI(dvm, -1)) {
					STO_WRITE(dvm, -1, dvm_literal_create_dvm_string_i(dvm, TRUE_STRING));
				}else{
					STO_WRITE(dvm, -1, dvm_literal_create_dvm_string_i(dvm, FALSE_STRING));
				}
				pc++;
				break;
			}
			case DVM_CAST_INT_TO_STRING:{
				int value = STI(dvm, -1);
				char buf[LINE_BUF_SZIE];
				sprintf(buf, "%d", value);
				DVM_Char *str = dvm_mbs2wcs(buf);
				DVM_ObjectRef str_ref = dvm_create_dvm_string_i(dvm, str);
				STO_WRITE(dvm, -1, str_ref);
				pc++;
				break;
			}
			case DVM_CAST_DOUBLE_TO_STRING:{
				double value = STD(dvm, -1);
				char buf[LINE_BUF_SZIE];
				sprintf(buf, "%lf", value);
				DVM_Char *str = dvm_mbs2wcs(buf);
				DVM_ObjectRef str_ref = dvm_create_dvm_string_i(dvm, str);
				STO_WRITE(dvm, -1, str_ref);
				pc++;
				break;
			}
			case DVM_CAST_ENUM_TO_STRING:{
				size_t index = ee->enum_table[GET_2BYTE_INT(&code[pc + 1])];
				char *str = dvm->enum_[index]->dvm_enum->enumerator[STI(dvm, -1)];
				DVM_ObjectRef str_ref = dvm_create_dvm_string_i(dvm, dvm_mbs2wcs(str));
				STO_WRITE(dvm, -1, str_ref);
				pc += 3;
				break;
			}
			case DVM_UP_CAST:{
				DVM_ObjectRef obj = STO(dvm, -1);
				if (is_null_pointer(obj)) {
					if (throw_null_pointer_exception(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe)) {
						goto EXECUTE_END;
					}
				}
				int index= GET_2BYTE_INT(&code[pc + 1]);
				obj.v_table = obj.v_table->exec_class->interface_table[index];
				STO_WRITE(dvm, -1, obj);
				pc += 3;
				break;
			}
			case DVM_DOWN_CAST:
			{
				DVM_ObjectRef obj = STO(dvm, -1);
				int idx_in_exe = GET_2BYTE_INT(&code[pc+1]);
				size_t index = ee->class_table[idx_in_exe];
				DVM_Boolean is_same_class;
				DVM_Boolean is_interface;
				size_t interface_idx;
				DVM_ErrorStatus status;
				DVM_ObjectRef exception;
				
				do {
					if (is_null_pointer(obj)) {
						if (throw_null_pointer_exception(dvm, &func,
														 &code, &code_size,
														 &pc, &base, &ee, &exe)) {
							goto EXECUTE_END;
						}
						break;
					}
					status = check_down_cast(dvm, &obj, index,
											 &is_same_class,
											 &is_interface, &interface_idx);
					if (status != DVM_SUCCESS) {
						exception
						= dvm_create_exception(dvm,
											   CLASS_CAST_EXCEPTION_CLASS,
											   CLASS_CAST_ERR,
											   DVM_STRING_MESSAGE_ARGUMENT,
											   "org",
											   obj.v_table->exec_class->name,
											   DVM_STRING_MESSAGE_ARGUMENT,
											   "target",
											   dvm->class_[index]->name,
											   DVM_MESSAGE_ARGUMENT_END);
						if (do_throw(dvm, &func, &code, &code_size, &pc,
									 &base, &ee, &exe, &exception)) {
							goto EXECUTE_END;
						}
						break;
					}
					if (!is_same_class) {
						if (is_interface) {
							obj.v_table
							= obj.v_table->exec_class
							->interface_table[interface_idx];
						} else {
							obj.v_table = obj.v_table->exec_class->class_table;
						}
					}
					STO_WRITE(dvm, -1, obj);
					pc += 3;
				} while (0);
				break;
			}
			case DVM_EQ_INT:{
				STI(dvm, -2) = STI(dvm, -2) == STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_EQ_DOUBLE:{
				STI(dvm, -2) = STD(dvm, -2) == STD(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_EQ_OBJECT:{
				STI_WRITE(dvm, -2, STO(dvm, -2).data == STO(dvm, -1).data);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_EQ_STRING:{
				STI_WRITE(dvm, -2, dvm_wcscmp(STO(dvm, -2).data->u.string.string, STO(dvm, -1).data->u.string.string) == 0);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_GT_INT:{
				STI(dvm, -2) = STI(dvm, -2) > STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_GT_DOUBLE:{
				STI(dvm, -2) = STD(dvm, -2) > STD(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_GT_STIRNG:{
				STI_WRITE(dvm, -2, dvm_wcscmp(STO(dvm, -2).data->u.string.string, STO(dvm, -1).data->u.string.string) > 0);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_GE_INT:{
				STI(dvm, -2) = STI(dvm, -2) >= STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_GE_DOUBLE:{
				STI(dvm, -2) = STD(dvm, -2) >= STD(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_GE_STRING:{
				STI_WRITE(dvm, -2, dvm_wcscmp(STO(dvm, -2).data->u.string.string, STO(dvm, -1).data->u.string.string) >= 0);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
				
			case DVM_LT_INT:{
				STI(dvm, -2) = STI(dvm, -2) < STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_LT_DOUBLE:{
				STI(dvm, -2) = STD(dvm, -2) < STD(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_LT_STRING:{
				STI_WRITE(dvm, -2, dvm_wcscmp(STO(dvm, -2).data->u.string.string, STO(dvm, -1).data->u.string.string) < 0);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_LE_INT:{
				STI(dvm, -2) = STI(dvm, -2) <= STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_LE_DOUBLE:{
				STI(dvm, -2) = STD(dvm, -2) <= STD(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_LE_STRING:{
				STI_WRITE(dvm, -2, dvm_wcscmp(STO(dvm, -2).data->u.string.string, STO(dvm, -1).data->u.string.string) <= 0);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_NE_INT:{
				STI(dvm, -2) = STI(dvm, -2) == STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_NE_DOUBLE:{
				STI(dvm, -2) = STD(dvm, -2) != STD(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_NE_OBJECT:{
				STI(dvm, -2) = STO(dvm, -2).data != STO(dvm, -1).data;
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_NE_STRING:{
				STI_WRITE(dvm, -2, dvm_wcscmp(STO(dvm, -2).data->u.string.string, STO(dvm, -1).data->u.string.string) != 0);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_LOGIC_AND:{
				STI(dvm, -2) = STI(dvm, -2) && STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_LOGIC_OR:{
				STI(dvm, -2) = STI(dvm, -2) || STI(dvm, -1);
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_LOGIC_NOT:{
				STI(dvm, -1) =  !STI(dvm, -1);
				pc++;
				break;
				
			}
			case DVM_POP:{
				dvm->stack.stack_pointer--;
				pc++;
				break;
			}
			case DVM_DUPLICATE:{
				dvm->stack.stack[dvm->stack.stack_pointer] =  dvm->stack.stack[dvm->stack.stack_pointer - 1];
				dvm->stack.pointer_flags[dvm->stack.stack_pointer] = dvm->stack.pointer_flags[dvm->stack.stack_pointer-1];
				dvm->stack.stack_pointer++;
				pc++;
				break;
			}
			case DVM_DUPLICATE_OFFSET:{
				int offset = GET_2BYTE_INT(&code[pc + 1]);
				dvm->stack.stack[dvm->stack.stack_pointer] =  dvm->stack.stack[dvm->stack.stack_pointer - 1 - offset];
				dvm->stack.pointer_flags[dvm->stack.stack_pointer] = dvm->stack.pointer_flags[dvm->stack.stack_pointer-1 - offset];
				dvm->stack.stack_pointer++;
				pc+=3;
				break;
			}
			case DVM_JUMP:{
				pc = GET_2BYTE_INT(&code[pc+1]);
				break;
			}
			case DVM_JUMP_IF_TRUE:{
				if (STI(dvm, -1)) {
					pc = GET_2BYTE_INT(&code[pc+1]);
				}else{
					pc += 3;
				}
				dvm->stack.stack_pointer--;
				break;
			}
			case DVM_JUMP_IF_FALSE:{
				if (!STI(dvm, -1)) {
					pc = GET_2BYTE_INT(&code[pc+1]);
				}else{
					pc += 3;
				}
				dvm->stack.stack_pointer--;
				break;
			}
			case DVM_PUSH_FUNCTION:{
				int index= GET_2BYTE_INT(&code[pc + 1]);
				STI_WRITE(dvm, 0, (int)ee->function_table[index]);
				dvm->stack.stack_pointer++;
				pc += 3;
				break;
			}
			case DVM_PUSH_METHOD:{
				DVM_ObjectRef ref = STO(dvm, -1);
				if (is_null_pointer(ref)) {
					DVM_ObjectRef exception;
					if (do_throw(dvm, &func, &code, &code_size, &pc,
								 &base, &ee, &exe, &exception)) {
						goto EXECUTE_END;
					}
				}
				int index = GET_2BYTE_INT(&code[pc + 1]);
				int fun_index = (int)ref.v_table->table[index].index;
				STI_WRITE(dvm, 0,fun_index);
				dvm->stack.stack_pointer++;
				pc += 3;
				break;
			}
			case DVM_PUSH_DELEGATE:{
				int index = GET_2BYTE_INT(&code[pc + 1]);
				index = (int)ee->function_table[index];
				DVM_ObjectRef delegate = dvm_create_delegate(dvm, dvm_null_object_ref, index);
				STO_WRITE(dvm, 0, delegate);
				dvm->stack.stack_pointer++;
				pc += 3;
				break;
			}
			case DVM_PUSH_METHOD_DELEGATE:{
				DVM_ObjectRef obj = STO(dvm, -1);
				int index = GET_2BYTE_INT(&code[pc + 1]);
				DVM_ObjectRef delegate = dvm_create_delegate(dvm, obj, index);
				STO_WRITE(dvm, 0, delegate);
				dvm->stack.stack_pointer++;
				pc += 3;
				break;
			}
			case DVM_INVOKE:
			case DVM_INVOKE_DELEGATE:{
				size_t func_index;
				if ((DVM_Opcode)code[pc] == DVM_INVOKE_DELEGATE) {
					DVM_ObjectRef delegate = STO(dvm, -1);
					if (is_null_pointer(delegate)) {
						if (throw_null_pointer_exception(dvm, &func,
														 &code, &code_size,
														 &pc, &base, &ee, &exe)) {
							goto EXECUTE_END;
						}
					}
					
					if (is_null_pointer(delegate.data->u.delegate.obj)) {
						func_index = delegate.data->u.delegate.index;
					}else{
						func_index = delegate.data->u.delegate.obj.v_table->table[delegate.data->u.delegate.index].index;
						STO_WRITE(dvm, -1, delegate.data->u.delegate.obj);
						dvm->stack.stack_pointer++;
						
					}
					
				}else{
					func_index = STI(dvm, -1);
				}
				
				if (dvm->function[func_index]->kind == NATIVE_FUNCTION) {
					restore_pc(dvm, ee, func, pc);
					dvm->current_exception = dvm_null_object_ref;
					invoke_native_function(dvm, func, dvm->function[func_index], pc, &dvm->stack.stack_pointer, base);
					if (!is_object_null(dvm->current_exception)) {
						if (do_throw(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe, &dvm->current_exception)) {
							goto EXECUTE_END;
						}
					}else{
						pc++;
					}
					
				}else{
					invoke_mango_function(dvm, &func, dvm->function[func_index], &code, &code_size, &pc, &dvm->stack.stack_pointer, &base, &ee, &exe);
				}
				
				
				break;
			}
			case DVM_RETURN:{
				if (rerurn_function(dvm, &func, &code, &code_size, &base, &pc, &ee, &exe)) {
					ret = dvm->stack.stack[dvm->stack.stack_pointer - 1];
					goto EXECUTE_END;
				}
				break;
			}
			case DVM_NEW:{
				size_t index = GET_2BYTE_INT(&code[pc +1]);
				index = ee->class_table[index];
				STO_WRITE(dvm, 0, dvm_create_class_object_i(dvm, index));
				dvm->stack.stack_pointer++;
				pc += 3;
				break;
			}
			case DVM_NEW_ARRAY:{
				int dim = code[pc + 1];
				DVM_TypeSpecifier *type = &exe->type_specifier[GET_2BYTE_INT(&code[pc + 2])];
				restore_pc(dvm, ee, func, pc);
				DVM_ObjectRef array = create_array(dvm, dim, type);
				dvm->stack.stack_pointer -= dim;
				STO_WRITE(dvm, 0, array);
				dvm->stack.stack_pointer++;
				pc += 4;
				break;
			}
			case DVM_NEW_ARRAY_LTTERAL_INT:{
				int size = GET_2BYTE_INT(&code[pc + 1]);
				restore_pc(dvm, ee, func, pc);
				DVM_ObjectRef array = create_array_literal_int(dvm, size);
				dvm->stack.stack_pointer -= size;
				STO_WRITE(dvm, 0, array);
				dvm->stack.stack_pointer++;
				pc += 3;
				break;
			}
			case DVM_NEW_ARRAY_LITERAL_DOUBLE:{
				int size = GET_2BYTE_INT(&code[pc + 1]);
				restore_pc(dvm, ee, func, pc);
				DVM_ObjectRef array = create_array_literal_double(dvm, size);
				dvm->stack.stack_pointer -= size;
				STO_WRITE(dvm, 0, array);
				dvm->stack.stack_pointer++;
				pc += 3;
				break;
			}
			case DVM_NEW_ARRAY_LITERAL_OBJECT:{
				int size = GET_2BYTE_INT(&code[pc + 1]);
				restore_pc(dvm, ee, func, pc);
				DVM_ObjectRef array = create_array_literal_object(dvm, size);
				dvm->stack.stack_pointer -= size;
				STO_WRITE(dvm, 0, array);
				dvm->stack.stack_pointer++;
				pc += 3;
				break;
			}
			case DVM_SUPER:{
				DVM_ObjectRef *this_obj = &STO(dvm, -1);
				ExecClass *this_class = this_obj->v_table->exec_class;
				this_obj->v_table = this_class->super_class->class_table;
				pc++;
				break;
			}
			case DVM_INSTANCEOF:{
				size_t index = GET_2BYTE_INT(&code[pc + 1]);
				index = ee->class_table[index];
				DVM_ObjectRef obj = STO(dvm, -1);
				if (obj.v_table->exec_class->class_index == index) {
					STI_WRITE(dvm, -1, DVM_TRUE);
				}else{
					STI_WRITE(dvm, -1, check_instanceof(dvm, &obj, index));
				}
				pc += 3;
			}
				
			case DVM_THROW:{
				DVM_ObjectRef *ex = &STO(dvm, -1);
				clear_stack_trace(dvm, ex);
				if (do_throw(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe, ex)) {
					goto EXECUTE_END;
				}
				
				break;
			}
			case DVM_RETHROW:{
				DVM_ObjectRef *ex = &STO(dvm, -1);
				if (do_throw(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe, ex)) {
					goto EXECUTE_END;
				}
				
				break;
			}
				
			case DVM_GO_FINALLY:{
				STI_WRITE(dvm, 0, (int)pc);
				dvm->stack.stack_pointer++;
				pc = GET_2BYTE_INT(&code[pc + 1]);
				break;
			}
			case DVM_FINALLY_END:{
				if (!is_object_null(dvm->current_exception)) {
					if (do_throw(dvm, &func, &code, &code_size, &pc, &base, &ee, &exe, &dvm->current_exception)) {
						goto EXECUTE_END;
					}
				}else{
					pc = STI(dvm, -1) + 3;
					dvm->stack.stack_pointer++;
				}
				break;
			}
				
				
				
				
			default:
				DBG_assert(0, "code[%d]..%d\n",pc, code[pc]);
				break;
		}
	}
EXECUTE_END:
	;
	return ret;
}





DVM_Value dvm_execute(DVM_VirtualMachine *dvm){
	dvm->current_executable = dvm->top_level;
	dvm->current_function = NULL;
	dvm_expend_stack(dvm, dvm->top_level->executable->top_level.need_stack_size);
	return  dvm_execute_i(dvm, NULL, dvm->top_level->executable->top_level.code,  dvm->top_level->executable->top_level.code_size, 0);

}


void dvm_push_object(DVM_VirtualMachine *dvm, DVM_Value value){
	STO_WRITE(dvm, 0, value.object);
	dvm->stack.stack_pointer++;
}

DVM_Value dvm_pop_object(DVM_VirtualMachine *dvm){
	DVM_ObjectRef ref = STO(dvm, 0);
	DVM_Value val;
	val.object = ref;
	dvm->stack.stack_pointer--;
	return val;
}






















