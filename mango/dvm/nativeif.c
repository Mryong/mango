//
//  nativeif.c
//  mango
//
//  Created by jerry.yong on 2017/9/4.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "dvm_pri.h"
#include <string.h>
#include "DBG.h"
#include "MEM.h"
#include "share.h"

size_t dvm_array_size(DVM_VirtualMachine *dvm, DVM_Object *array){
	return array->u.array.size;
}

static DVM_ErrorStatus check_array(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, DVM_Executable *exe, Function *func, size_t pc, DVM_ObjectRef *exception_p){
	if (array.data == NULL) {
		*exception_p = dvm_create_exception(dvm, DVM_NULL_POINTER_EXCEPTION_CLASS, NULL_POINTER_ERR, DVM_MESSAGE_ARGUMENT_END);
		return DVM_ERROR;
	}
	if (index < 0 || index >= array.data->u.array.size) {
		*exception_p = dvm_create_exception(dvm, ARRAY_INDEX_EXCEPTION_CLASS, INDEX_OUT_OF_BOUNDS_ERR,
											DVM_INT_MESSAGE_ARGUMENT,"index",index,
											DVM_INT_MESSAGE_ARGUMENT,"size",array.data->u.array.size,
											DVM_MESSAGE_ARGUMENT_END);
		return DVM_ERROR;
	}
	
	return DVM_SUCCESS;
	
}




DVM_ErrorStatus dvm_array_get_int(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, int *value, DVM_ObjectRef *exception){
	DVM_ErrorStatus status = check_array(dvm, array, index, dvm->current_executable->executable, dvm->current_function, dvm->pc, exception);
	if (status != DVM_SUCCESS) {
		return status;
	}
	*value = array.data->u.array.u.int_array[index];
	return DVM_SUCCESS;
}


DVM_ErrorStatus dvm_array_get_double(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, double *value, DVM_ObjectRef *exception){
	DVM_ErrorStatus status = check_array(dvm, array, index, dvm->current_executable->executable, dvm->current_function, dvm->pc, exception);
	if (status != DVM_SUCCESS) {
		return status;
	}
	*value = array.data->u.array.u.double_array[index];
	return DVM_SUCCESS;
}


DVM_ErrorStatus dvm_array_get_object(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, DVM_ObjectRef *value, DVM_ObjectRef *exception){
	DVM_ErrorStatus status = check_array(dvm, array, index, dvm->current_executable->executable, dvm->current_function, dvm->pc, exception);
	if (status != DVM_SUCCESS) {
		return status;
	}
	*value = array.data->u.array.u.object_array[index];
	return DVM_SUCCESS;
}



DVM_ErrorStatus dvm_array_set_int(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, int value, DVM_ObjectRef *exception){
	DVM_ErrorStatus status = check_array(dvm, array, index, dvm->current_executable->executable, dvm->current_function, dvm->pc, exception);
	if (status != DVM_SUCCESS) {
		return status;
	}
	array.data->u.array.u.int_array[index] = value;
	return DVM_SUCCESS;
}


DVM_ErrorStatus dvm_array_set_double(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, double value, DVM_ObjectRef *exception){
	DVM_ErrorStatus status = check_array(dvm, array, index, dvm->current_executable->executable, dvm->current_function, dvm->pc, exception);
	if (status != DVM_SUCCESS) {
		return status;
	}
	array.data->u.array.u.double_array[index] = value;
	return DVM_SUCCESS;
}


DVM_ErrorStatus dvm_array_set_object(DVM_VirtualMachine *dvm, DVM_ObjectRef array, int index, DVM_ObjectRef value, DVM_ObjectRef *exception){
	DVM_ErrorStatus status = check_array(dvm, array, index, dvm->current_executable->executable, dvm->current_function, dvm->pc, exception);
	if (status != DVM_SUCCESS) {
		return status;
	}
	array.data->u.array.u.object_array[index] = value;
	return DVM_SUCCESS;
}




static void resize_array_alloc_size(DVM_VirtualMachine *dvm, DVM_Object *array, size_t new_size){
	size_t old_alloc_size = array->u.array.alloc_size;
	size_t new_alloc_size = 0;
	DVM_Boolean need_alloc = DVM_FALSE;
	if (old_alloc_size < new_size) {
		new_alloc_size = old_alloc_size * 2;
		if (new_alloc_size < new_size) {
			new_alloc_size = new_size + ARRAY_ALLOC_SIZE;
		}else if (new_alloc_size > new_size + ARRAY_ALLOC_SIZE){
			new_alloc_size = old_alloc_size + ARRAY_ALLOC_SIZE;
		}
		new_alloc_size = DVM_TRUE;
	}else if (new_size + ARRAY_ALLOC_SIZE < old_alloc_size){
		new_alloc_size = old_alloc_size - ARRAY_ALLOC_SIZE;
		new_alloc_size = DVM_TRUE;
	}else{
		new_alloc_size = DVM_FALSE;
	}
	
	if (need_alloc) {
		dvm_check_gc(dvm);
		switch (array->u.array.type) {
			case INT_ARRAY:
				array->u.array.u.int_array = MEM_malloc(sizeof(int) * new_alloc_size);
				dvm->heap.current_heap_size += (int)(new_alloc_size - old_alloc_size) * sizeof(int);
				break;
			case DOUBLE_ARRAY:
				array->u.array.u.double_array = MEM_malloc(sizeof(double) * new_alloc_size);
				dvm->heap.current_heap_size += (int)(new_alloc_size - old_alloc_size) * sizeof(double);
				break;
			case OBJECT_ARRAY:
				array->u.array.u.object_array = MEM_malloc(sizeof(DVM_ObjectRef) * new_alloc_size);
				dvm->heap.current_heap_size += (int)(new_alloc_size - old_alloc_size) * sizeof(DVM_ObjectRef);
				break;
			case FUNCTION_INDEX_ARRAY:
			default:
				DBG_assert(0,"array->u.array->type.. %d",array->u.array.type);
				break;
		}
		
	}
	
	array->u.array.alloc_size = new_alloc_size;

}

void dvm_array_resize(DVM_VirtualMachine *dvm, DVM_Object *array, size_t new_size){
	resize_array_alloc_size(dvm, array, new_size);
	switch (array->u.array.type) {
		case INT_ARRAY:
			for (size_t i = array->u.array.size; i < new_size; i++) {
				array->u.array.u.int_array[i] = 0;
			}
			break;
		case DOUBLE_ARRAY:
			for (size_t i = array->u.array.size; i < new_size; i++) {
				array->u.array.u.double_array[i] = 0.0;
			}
			break;
		case OBJECT_ARRAY:
			for (size_t i = array->u.array.size; i < new_size; i++) {
				array->u.array.u.object_array[i] = dvm_null_object_ref;
			}
			break;
		case FUNCTION_INDEX_ARRAY:
		default:
			DBG_assert(0,"array->u.array->type.. %d",array->u.array.type);
			break;
	}
	array->u.array.size = new_size;
}


static void extracted(DVM_Object *array, DVM_VirtualMachine *dvm) {
	resize_array_alloc_size(dvm, array, array->u.array.size + 1);
}

void dvm_array_insert(DVM_VirtualMachine *dvm, DVM_Object *array, size_t pos, DVM_Value value){
	extracted(array, dvm);
	switch (array->u.array.type) {
		case INT_ARRAY:
			memmove(&array->u.array.u.int_array[pos + 1], &array->u.array.u.int_array[pos], sizeof(int) * (array->u.array.size - pos));
			array->u.array.u.int_array[pos] = value.int_value;
			break;
		case DOUBLE_ARRAY:
			memmove(&array->u.array.u.double_array[pos + 1], &array->u.array.u.double_array[pos], sizeof(double) * (array->u.array.size - pos));
			array->u.array.u.double_array[pos] = value.double_value;
			break;
		case OBJECT_ARRAY:
			memmove(&array->u.array.u.object_array[pos + 1], &array->u.array.u.object_array[pos], sizeof(DVM_ObjectRef) * (array->u.array.size - pos));
			array->u.array.u.object_array[pos] = value.object;
			break;
		case FUNCTION_INDEX_ARRAY:
		default:
			DBG_assert(0,"array->u.array->type.. %d",array->u.array.type);
			break;
	}
	array->u.array.size++;
}

void dvm_array_remove(DVM_VirtualMachine *dvm, DVM_Object *array, size_t pos){
	switch (array->u.array.type) {
		case INT_ARRAY:
			memmove(&array->u.array.u.int_array[pos], &array->u.array.u.int_array[pos + 1], sizeof(int) * (array->u.array.size - pos - 1));
			break;
		case DOUBLE_ARRAY:
			memmove(&array->u.array.u.double_array[pos], &array->u.array.u.double_array[pos + 1], sizeof(double) * (array->u.array.size - pos - 1));
			break;
		case OBJECT_ARRAY:
			memmove(&array->u.array.u.object_array[pos], &array->u.array.u.object_array[pos + 1], sizeof(DVM_ObjectRef) * (array->u.array.size - pos - 1));
			break;
		case FUNCTION_INDEX_ARRAY:
		default:
			DBG_assert(0,"array->u.array->type.. %d",array->u.array.type);
			break;
	}
	resize_array_alloc_size(dvm, array, array->u.array.size - 1);
	array->u.array.size--;
	
}


size_t dvm_string_length(DVM_VirtualMachine *dvm, DVM_Object *string){
	return string->u.string.length;
}

DVM_Char * dvm_string_get_string(DVM_VirtualMachine *dvm, DVM_Object *string){
	return string->u.string.string;
}

static DVM_ErrorStatus check_string_index(DVM_VirtualMachine *dvm, DVM_ObjectRef string, int index, DVM_Executable *exe, Function *func, size_t pc, DVM_ObjectRef *exception_p){
	if (string.data == NULL) {
		*exception_p = dvm_create_exception(dvm, DVM_NULL_POINTER_EXCEPTION_CLASS, NULL_POINTER_ERR, DVM_MESSAGE_ARGUMENT_END);
		return DVM_ERROR;
	}
	if (index < 0 || index >= string.data->u.string.length) {
		*exception_p = dvm_create_exception(dvm, STRING_INDEX_EXCEPTION_CLASS, INDEX_OUT_OF_BOUNDS_ERR,
											DVM_INT_MESSAGE_ARGUMENT, "index", index,
											DVM_INT_MESSAGE_ARGUMENT, "size", string.data->u.string.length,
											DVM_MESSAGE_ARGUMENT_END);
		return DVM_ERROR;
	}
	return DVM_SUCCESS;
	
}

DVM_ErrorStatus dvm_get_character(DVM_VirtualMachine *dvm, DVM_ObjectRef string, int index, DVM_Char *ch, DVM_ObjectRef *exception_p){
	DVM_ErrorStatus status = check_string_index(dvm, string, index, dvm->current_executable->executable, dvm->current_function, dvm->pc, exception_p);
	if (status != DVM_SUCCESS) {
		return status;
	}
	
	*ch = string.data->u.string.string[index];
	return DVM_SUCCESS;
}

DVM_Value dvm_string_substr(DVM_VirtualMachine *dvm, DVM_Object *str, int pos, size_t len){
	 DVM_Char *new_str = MEM_malloc(sizeof(DVM_Char) *(len + 1));
	dvm_wcsncpy(new_str, str->u.string.string + len, len);
	new_str[len] = L'\0';
	DVM_Value ret;
	ret.object = dvm_create_dvm_string_i(dvm, new_str);
	return ret;
}


static size_t get_field_index(ExecClass *ec, char *filed_name, size_t *count){
	if (ec->super_class) {
		size_t index = get_field_index(ec->super_class, filed_name,count);
		if (index != FIELD_NOT_FOUND) {
			return index;
		}
	}
	
	for (size_t i =0 ; i < ec->field_count; i++) {
		if (!strcmp(filed_name, ec->dvm_calss->field[i].name)) {
			return *count + i;
		}
	}
	
	*count = *count + ec->dvm_calss->field_count;
	return FIELD_NOT_FOUND;
}


size_t dvm_get_field_index(DVM_VirtualMachine *dvm, DVM_ObjectRef obj, char *field_name){
	size_t count = 0;
	return get_field_index(obj.v_table->exec_class, field_name, &count);

}


DVM_Value dvm_get_field_value(DVM_ObjectRef obj, size_t index){
	return obj.data->u.class_object.field[index];
}

void dvm_set_field_value(DVM_ObjectRef obj, size_t index, DVM_Value value){
	obj.data->u.class_object.field[index] = value;
}

void *dvm_object_get_native_pointer_value(DVM_Object *obj){
	DBG_assert(obj->type == NATIVE_POINTER_OBJECT, "obj->type..%d\n",obj->type);
	return obj->u.native_pointer.pointer;
}


void dvm_object_set_native_pointer_value(DVM_Object *obj, void *pointer){
	DBG_assert(obj->type == NATIVE_POINTER_OBJECT, "obj->type..%d\n",obj->type);
	obj->u.native_pointer.pointer = pointer;
}

void dvm_set_exception(DVM_VirtualMachine *dvm, DVM_Context *ctx,DVM_NativeLibInfo *lib_info, char *package_name, char *class_name, int error_id,...){
	size_t class_index = dvm_search_class(dvm, package_name, class_name);
	DVM_ObjectRef obj = dvm_crate_class_object(dvm, ctx, class_index);
	size_t message_filed_index = dvm_get_field_index(dvm, obj, "message");
	VString message;
	va_list ap;
	va_start(ap, error_id);
	dvm_format_message(lib_info->message_format, error_id, &message, ap);
	va_end(ap);
	obj.data->u.class_object.field[message_filed_index].object = dvm_create_dvm_string_i(dvm, message.string);
	
	size_t stack_trace_index = dvm_get_field_index(dvm, obj, "stack_trace");
	obj.data->u.class_object.field[stack_trace_index].object = dvm_create_array_object_i(dvm, 0);
	
	dvm->current_exception = obj;
	
}

void dvm_set_null(DVM_Value *value){
	value->object.data = NULL;
	value->object.v_table = NULL;
}

DVM_ObjectRef dvm_up_case(DVM_ObjectRef obj, size_t target_index){
	DVM_ObjectRef ref;
	ref = obj;
	ref.v_table = obj.v_table->exec_class->interface_table[target_index];
	return ref;
}

DVM_Value dvm_check_exception(DVM_VirtualMachine *dvm){
	DVM_Value ret;
	ret.object = dvm->current_exception;
	return ret;
	
}








