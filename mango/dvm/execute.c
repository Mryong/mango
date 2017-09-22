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
				dvm_array_get_int(<#DVM_VirtualMachine *dvm#>, <#DVM_ObjectRef array#>, <#int index#>, <#int *value#>, <#DVM_ObjectRef *exception#>)
				
				
				
			}
				
				
				
			default:
    break;
		}
	}
	
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






















