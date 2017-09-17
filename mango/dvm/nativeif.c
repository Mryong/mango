//
//  nativeif.c
//  mango
//
//  Created by jerry.yong on 2017/9/4.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "dvm_pri.h"

size_t dvm_array_size(DVM_VirtualMachine *dvm, DVM_Object *array){
	return array->u.array.size;
}

DVM_ErrorStatus dvm_array_get_int(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, int *value, DVM_ObjectRef *exception){
	return 0;
}
