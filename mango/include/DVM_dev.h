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
#define DVM_NULL_POINTER_EXCEPTION_CLASS ("NullPointerException")


/* nativeif.c */
size_t dvm_array_size(DVM_VirtualMachine *dvm, DVM_Object *array);
DVM_ErrorStatus dvm_array_get_int(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, int *value, DVM_ObjectRef *exception);
DVM_ErrorStatus dvm_array_get_double(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, double *value, DVM_ObjectRef *exception);
DVM_ErrorStatus dvm_array_get_object(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, DVM_ObjectRef *value, DVM_ObjectRef *exception);
DVM_ErrorStatus dvm_array_set_int(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, int value, DVM_ObjectRef *exception);
DVM_ErrorStatus dvm_array_set_double(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, double value, DVM_ObjectRef *exception);
DVM_ErrorStatus dvm_array_set_object(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, DVM_ObjectRef value, DVM_ObjectRef *exception);
void dvm_array_resize(DVM_VirtualMachine *dvm, DVM_Object *array, size_t new_size);
void dvm_array_insert(DVM_VirtualMachine *dvm, DVM_Object *array, size_t pos, DVM_Value value);
void dvm_array_remove(DVM_VirtualMachine *dvm, DVM_Object *array, size_t pos);
size_t dvm_string_length(DVM_VirtualMachine *dvm, DVM_Object *string);
DVM_Char * dvm_string_get_string(DVM_VirtualMachine *dvm, DVM_Object *string);
DVM_ErrorStatus dvm_get_character(DVM_VirtualMachine *dvm, DVM_ObjectRef string, int index, DVM_Char *ch, DVM_ObjectRef *exception_p);
DVM_Value dvm_string_substr(DVM_VirtualMachine *dvm, DVM_Object *str, int pos, size_t len);
size_t dvm_get_field_index(DVM_VirtualMachine *dvm, DVM_ObjectRef obj, char *field_name);
DVM_Value dvm_get_field_value(DVM_ObjectRef obj, size_t index);
void dvm_set_field_value(DVM_ObjectRef obj, size_t index, DVM_Value value);
void *dvm_object_get_native_pointer_value(DVM_Object *obj);
void dvm_object_set_native_pointer_value(DVM_Object *obj, void *pointer);
void dvm_set_exception(DVM_VirtualMachine *dvm, DVM_Context *ctx,DVM_NativeLibInfo *lib_info, char *package_name, char *class_name, int error_id,...);
void dvm_set_null(DVM_Value *value);
DVM_ObjectRef dvm_up_case(DVM_ObjectRef obj, size_t target_index);
DVM_Value dvm_check_exception(DVM_VirtualMachine *dvm);










#endif /* DVM_dev_h */
