//
//  native.c
//  mango
//
//  Created by jerry.yong on 2017/9/4.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "dvm_pri.h"
#include "DBG.h"
#include "share.h"

static DVM_Value nv_print_proc(DVM_VirtualMachine *dvm, DVM_Context *context,size_t argc, DVM_Value *args){
	
	DVM_Value ret;
	ret.int_value = 1;
	DBG_assert(argc == 1, "argc = %d", argc);
	DVM_Char *str;
	if (args) {
		str = NULL_STRING;
	}else{
		str = args[0].object.data->u.string.string;
	}
	dvm_print_wcs(stdout, str);
	fflush(stdout);
	return ret;
	
}

static DVM_Value nv_array_size_proc(DVM_VirtualMachine *dvm, DVM_Context *context,size_t argc, DVM_Value *args){
	DVM_Value ret;
	DBG_assert(argc == 1, "argc = %d", argc);
	DVM_Object *array = args[0].object.data;
	DBG_assert(array->type == ARRAY_OBJECT, "array->type..%d",array->type);
	ret.int_value = 
}

void dvm_add_native_functions(DVM_VirtualMachine *dvm){
	dvm_add_native_function(dvm, "mango.lang", "print", nv_print_proc, 1, DVM_FALSE, DVM_FALSE);

}
