//
//  util.c
//  mango
//
//  Created by jerry.yong on 2017/9/11.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "dvm_pri.h"
#include "DBG.h"



extern DVM_ObjectRef dvm_null_object_ref;

void dvm_initial_value(DVM_TypeSpecifier *type, DVM_Value *value){
	if (type->derive_count > 0) {
		value->object = dvm_null_object_ref;
	}else{
		switch (type->base_type) {
			case DVM_VOID_TYPE:
			case DVM_BOOLEAN_TYPE:
			case DVM_INT_TYPE:
			case DVM_ENUM_TYPE:
				value->int_value = 0;
				break;
			case DVM_DOUBLE_TYPE:
				value->double_value = 0.0;
				break;
			case DVM_CLASS_TYPE:
			case DVM_DELEGAET_TYPE:
			case DVM_NATIVE_POINTER_TYPE:
			case DVM_STRING_TYPE:
				value->object = dvm_null_object_ref;
				break;
			case DVM_NULL_TYPE:
			case DVM_BASE_TYPE:
			case DVM_UNSPECIFIED_IDENTIFIER_TYPE:
			default:
				DBG_assert(0, "type->base_type..%d",type->base_type);
		}
	}
}
