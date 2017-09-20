//
//  DVM_dev.h
//  mango
//
//  Created by yongpengliang on 2017/6/11.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef DVM_dev_h
#define DVM_dev_h
#include "DVM.h"

typedef enum {
	DVM_SUCCESS = 1,
	DVM_ERROR

}DVM_ErrorStatus;

typedef struct DVM_Context_tag DVM_Context;

typedef DVM_Value DVM_NativeFunctionProc(DVM_VirtualMachine *vm, DVM_Context *ctx, size_t argc, DVM_Value *argv);
typedef DVM_Value DVM_NativePointerFinalizeProc(DVM_VirtualMachine *vm, DVM_Object *obj);
typedef struct {
	char *name;
	DVM_NativePointerFinalizeProc *finalizer;
}DVM_NativePointerInfo;

typedef enum {
	DVM_INT_MESSAGE_ARGUMENT = 1,
	DVM_DOUBLE_MESSAGE_ARGUMENT,
	DVM_STRING_MESSAGE_ARGUMENT,
	DVM_CHARACTER_MESSAGE_ARGUMENT,
	DVM_POINTER_MESSAGE_ARGUMENT,
	DVM_MESSAGE_ARGUMENT_END
}DVM_MessageArgumentType;

typedef struct {
	char *format;
}DVM_ErrorDefine;

typedef struct {
	DVM_ErrorDefine  *message_format;
}DVM_NativeLibInfo;


#define DVM_MANGO_DEFAULE_PACKAGE_P1 "mango"
#define DVM_NANGO_DEFAULT_PACKAGE_P2 "lang"
#define DVM_NANGO_DEFAULT_PACKAGE  (DVM_MANGO_DEFAULE_PACKAGE_P1 "." DVM_NANGO_DEFAULT_PACKAGE_P2)
#define DVM_NULL_POINTER_EXCEPTION_NAME ("NullPointerException")


/* nativeif.c */
size_t dvm_array_size(DVM_VirtualMachine *dvm, DVM_Object *array);


#endif /* DVM_dev_h */
