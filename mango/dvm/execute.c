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

static DVM_Boolean do_throw(DVM_VirtualMachine *dvm, Function **func_p, DVM_Byte **code_p, size_t *code_p_size, size_t *pc_p,
							size_t *base_p, ExecutableEntry **ee_p, DVM_Executable **exe_p, DVM_ObjectRef *exception){
	
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
	size_t class_index = dvm_search_class(dvm, DVM_NANGO_DEFAULT_PACKAGE, class_name);
	DVM_ObjectRef obj = dvm_create_class_object_i(dvm, class_index);
	STO_WRITE(dvm, 0, obj);
	dvm->stack.stack_pointer++;
	VString message;
	dvm_format_message(dvm_error_message_format, id, &message, ap);
	va_end(ap);
	
	return obj;

}

DVM_Value *dvm_execute_i(DVM_VirtualMachine *dvm, Function *func, DVM_Byte *code, size_t code_size, size_t base){
	ExecutableEntry *ee = dvm->current_executable;
	DVM_Executable *exe = ee->executable;
	size_t pc = dvm->pc;
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
				
			}
				
				
				
			default:
				break;
		}
	}
EXECUTE_END:
	;
	return NULL;
}





DVM_Value *dvm_execute(DVM_VirtualMachine *dvm){
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






















