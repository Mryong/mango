//
//  heap.c
//  mango
//
//  Created by jerry.yong on 2017/9/13.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "dvm_pri.h"
#include "MEM.h"
#include "share.h"

extern DVM_ObjectRef dvm_null_object_ref;

void dvm_check_gc(DVM_VirtualMachine *dvm){
	if (dvm->heap.current_heap_size >= dvm->heap.current_threshold) {
		dvm_garbage_collect(dvm);
		dvm->heap.current_threshold = dvm->heap.current_heap_size + HEAP_THRESHOLD_SIZE;
	}
}

static DVM_ObjectRef alloc_object(DVM_VirtualMachine *dvm, ObjectType type){
	DVM_ObjectRef ret;
	ret.v_table = NULL;
	ret.data = MEM_malloc(sizeof(DVM_Object));
	ret.data->marked = DVM_FALSE;
	ret.data->type = type;
	if (dvm->heap.header) {
		dvm->heap.header->preview = ret.data;
	}
	ret.data->next = dvm->heap.header;
	ret.data->preview = NULL;
	dvm->heap.current_heap_size += sizeof(DVM_Object);
	return ret;
}

static void add_ref_in_native_method(DVM_Context *ctx, DVM_ObjectRef *obj){
	RefNativeFunc *new_ref = MEM_malloc(sizeof(RefNativeFunc));
	new_ref->object = *obj;
	new_ref->next = ctx->ref_in_native_method;
	ctx->ref_in_native_method = new_ref;
}

void dvm_add_ref_in_native_method(DVM_Context *ctx, DVM_Value value){
	add_ref_in_native_method(ctx, &value.object);
}


DVM_ObjectRef dvm_create_dvm_string_i(DVM_VirtualMachine *dvm, DVM_Char *str){
	DVM_ObjectRef ref = alloc_object(dvm, STRING_OBJECT);
	ref.v_table = dvm->string_v_table;
	ref.data->u.string.string  = str;
	size_t len = dvm_wcslen(str);
	ref.data->u.string.length = len;
	ref.data->u.string.is_literal = DVM_TRUE;
	dvm->heap.current_heap_size += sizeof(DVM_Char) * (len + 1);
	return ref;
}

static DVM_ObjectRef alloc_array(DVM_VirtualMachine *dvm, ArrayType type, size_t size){
	DVM_ObjectRef ref = alloc_object(dvm, ARRAY_OBJECT);
	ref.v_table = dvm->array_v_table;
	ref.data->u.array.type = type;
	ref.data->u.array.size = size;
	ref.data->u.array.alloc_size = size;
	return ref;
}

DVM_ObjectRef dvm_create_array_int_i(DVM_VirtualMachine *dvm, size_t size){
	DVM_ObjectRef ref = alloc_array(dvm, INT_ARRAY, size);
	ref.data->u.array.u.int_array = MEM_malloc(sizeof(int) * size);
	dvm->heap.current_heap_size += sizeof(int) * size;
	for (size_t i = 0; i < size; i++) {
		ref.data->u.array.u.int_array[i] = 0;
	}
	return ref;
}

DVM_ObjectRef dvm_create_array_int(DVM_VirtualMachine *dvm, DVM_Context *ctx, size_t size){
	DVM_ObjectRef ref = dvm_create_array_int_i(dvm, size);
	add_ref_in_native_method(ctx, &ref);
	return ref;
}



DVM_ObjectRef dvm_create_array_double_i(DVM_VirtualMachine *dvm, size_t size){
	DVM_ObjectRef ref = alloc_array(dvm, DOUBLE_ARRAY, size);
	ref.data->u.array.u.double_array = MEM_malloc(sizeof(double) * size);
	dvm->heap.current_heap_size += sizeof(double) * size;
	for (size_t i = 0; i < size; i++) {
		ref.data->u.array.u.double_array[i] = 0.0;
	}
	return ref;
}

DVM_ObjectRef dvm_create_array_double(DVM_VirtualMachine *dvm, DVM_Context *ctx, size_t size){
	DVM_ObjectRef ref =  dvm_create_array_double_i(dvm, size);
	add_ref_in_native_method(ctx, &ref);
	return ref;
}




DVM_ObjectRef dvm_create_array_object_i(DVM_VirtualMachine *dvm, size_t size){
	DVM_ObjectRef ref = alloc_array(dvm, OBJECT_ARRAY, size);
	ref.data->u.array.u.object_array = MEM_malloc(sizeof(DVM_ObjectRef) * size);
	dvm->heap.current_heap_size += sizeof(DVM_ObjectRef) * size;
	for (size_t i = 0; i < size; i++) {
		ref.data->u.array.u.object_array[i] = dvm_null_object_ref;
	}
	return ref;
}

DVM_ObjectRef dvm_create_array_object(DVM_VirtualMachine *dvm, DVM_Context *ctx, size_t size){
	DVM_ObjectRef ref =  dvm_create_array_object_i(dvm, size);
	add_ref_in_native_method(ctx, &ref);
	return ref;
}

static void initialize_fields(DVM_VirtualMachine *dvm, ExecClass *ec, DVM_ObjectRef obj){
	DVM_Value val;
	val.object = obj;
	dvm_push_object(dvm, val);
	dvm->current_executable = ec->executable;
	dvm->current_function = NULL;
	dvm->pc = 0;
	
	dvm_expend_stack(dvm,ec->dvm_calss->field_initializer.code_size);
	dvm_execute_i(dvm, NULL, ec->dvm_calss->field_initializer.code, ec->dvm_calss->field_initializer.code_size, 0);
	dvm_pop_object(dvm);
	
}

DVM_ObjectRef dvm_create_class_object_i(DVM_VirtualMachine *dvm, size_t index){
	DVM_ObjectRef ref = alloc_object(dvm, CLASS_OBJECT);
	ExecClass *ec = dvm->class_[index];
	ref.v_table = ec->class_table;
	ref.data->u.class_object.field_count = ec->field_count;
	ref.data->u.class_object.field = MEM_malloc(sizeof(DVM_Value) * ec->field_count);
	for (size_t i  = 0; i < ec->field_count; i++) {
		dvm_initial_value(ec->field_type[i], &ref.data->u.class_object.field[i]);
	}
	initialize_fields(dvm, ec, ref);
	return ref;
}

DVM_ObjectRef dvm_crate_class_object(DVM_VirtualMachine *dvm, DVM_Context *ctx, size_t index){
	DVM_ObjectRef ref = dvm_create_class_object_i(dvm, index);
	add_ref_in_native_method(ctx, &ref);
	return ref;
}


DVM_ObjectRef dvm_create_native_pointer_i(DVM_VirtualMachine *dvm, void *poiner, DVM_NativePointerInfo *info){
	DVM_ObjectRef ref = alloc_object(dvm, DELEGATE_OBJECT);
	ref.v_table = NULL;
	ref.data->u.native_pointer.pointer = poiner;
	ref.data->u.native_pointer.info = info;
	return ref;

}

DVM_ObjectRef dvm_create_native_pointer(DVM_VirtualMachine *dvm, DVM_Context *ctx, void *poiner, DVM_NativePointerInfo *info){
	DVM_ObjectRef ref = dvm_create_native_pointer_i(dvm, poiner, info);
	add_ref_in_native_method(ctx, &ref);
	return ref;
}

DVM_ObjectRef dvm_create_delegate(DVM_VirtualMachine *dvm,DVM_ObjectRef obj ,size_t index) {
	DVM_ObjectRef ref = alloc_object(dvm, DELEGATE_OBJECT);
	ref.data->u.delegate.obj = obj;
	ref.data->u.delegate.index = index;
	ref.v_table = NULL;
	return ref;

}


static DVM_Boolean is_reference_type(DVM_TypeSpecifier *type){
	DVM_BaseType base = type->base_type;
	if ((type->derive_count > 0 && type->derive->tag == DVM_ARRAY_DERIVE) ||
		(type->derive_count == 0 && (base == DVM_CLASS_TYPE || base == DVM_DELEGAET_TYPE || base == DVM_NATIVE_POINTER_TYPE || base == DVM_STRING_TYPE))) {
		return DVM_TRUE;
	}
	return DVM_FALSE;

}








void dvm_garbage_collect(DVM_VirtualMachine *dvm){

}
