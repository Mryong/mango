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

void dvm_expend_stack(DVM_VirtualMachine *dvm, size_t need_stack_size){
	size_t rest = dvm->stack.alloc_size - dvm->stack.stack_pointer;
	if (rest <= need_stack_size) {
		size_t revalue_up = ((need_stack_size/STACK_ALLOC_SIZE) + 1) * STACK_ALLOC_SIZE;
		dvm->stack.alloc_size += revalue_up;
		dvm->stack.stack = MEM_malloc(sizeof(DVM_Value) * dvm->stack.alloc_size);
		dvm->stack.pointer_flags = MEM_malloc(sizeof(DVM_Boolean) * dvm->stack.alloc_size);
	}
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
