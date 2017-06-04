//
//  DVM_code.h
//  mango_preview
//
//  Created by jerry.yong on 2017/5/31.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef DVM_code_h
#define DVM_code_h

#include <wchar.h>

typedef wchar_t DVM_Char;
typedef unsigned char DVM_Byte;

typedef enum {
	DVM_VOID_TYPE,
	DVM_BOOLEAN_TYPE,
	DVM_INT_TYPE,
	DVM_DOUBLE_TYPE,
	DVM_STRING_TYPE,
	DVM_CLASS_TYPE,
	DVM_DELEGAET_TYPE,
	DVM_ENUM_TYPE,
	DVM_NULL_TYPE,
	DVM_NATIVE_POINTER_TYPE,
	DVM_BASE_TYPE,
	DVM_UNSPECIFIED_IDENTIFIER_TYPE
}DVM_BaseType;

#endif /* DVM_code_h */
