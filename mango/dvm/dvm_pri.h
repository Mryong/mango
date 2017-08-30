//
//  dvm_pri.h
//  mango
//
//  Created by jerry.yong on 2017/8/29.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef dvm_pri_h
#define dvm_pri_h

#include "DVM.h"
#include "DVM_code.h"
#include "DVM_dev.h"



typedef enum {
	BAD_MULTBYTE_CHARACTER_ERR = 1

}RumtimeError;

typedef struct {
	DVM_Char *string;
}VString;

typedef enum {
	NATIVE_FUNCTION,
	MANGO_FUNCTION
}FunctionKind;

typedef struct {
	
}NativeFunction;


#endif /* dvm_pri_h */
