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
	return NULL;
}
