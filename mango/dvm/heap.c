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
#include "DBG.h"

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

static void gc_rest_mark(DVM_Object *obj){
	obj->marked = DVM_FALSE;
}

static void gc_mark(DVM_ObjectRef *ref){
	if (ref->data == NULL) {
		return;
	}
	
	if (ref->data->marked) {
		return;
	}
	ref->data->marked = DVM_TRUE;
	
	if (ref->data->type == ARRAY_OBJECT) {
		if (ref->data->u.array.type == OBJECT_ARRAY) {
			for (size_t i = 0; i < ref->data->u.array.size; i++) {
				gc_mark(&ref->data->u.array.u.object_array[i]);
			}
		}
	}else if (ref->data->type == CLASS_OBJECT){
		ExecClass *ec = ref->v_table->exec_class;
		for (size_t i = 0; i < ec->field_count; i++) {
			if (is_reference_type(ec->field_type[i])) {
				gc_mark(&ref->data->u.class_object.field[i].object);
			}
		}
	
	}else if (ref->data->type == DELEGATE_OBJECT){
		gc_mark(&ref->data->u.delegate.obj);
	}

}

static void gc_mark_ref_in_native_method(DVM_Context *ctx){
	if (ctx == NULL) {
		return;
	}
	for (RefNativeFunc * ref = ctx->ref_in_native_method; ref; ref = ref->next) {
		gc_mark(&ref->object);
	}
}



static void gc_mark_objects(DVM_VirtualMachine *dvm){
	for (DVM_Object *pos = dvm->heap.header; pos; pos = pos->next) {
		gc_rest_mark(pos);
	}
	
	
	for (ExecutableEntry *e_pos = dvm->executable_entry; e_pos; e_pos = e_pos->next) {
		for (size_t i = 0 ; i < e_pos->static_v.vars_count; i++) {
			if (is_reference_type(e_pos->executable->global_variable[i].type)) {
				gc_mark(&e_pos->static_v.vars[i].object);
			}
		}
	}
	
	
	for (size_t i = 0; i < dvm->stack.stack_pointer; i++) {
		if (dvm->stack.pointer_flags[i]) {
			gc_mark(&dvm->stack.stack[i].object);
		}
	}
	
	gc_mark(&dvm->current_exception);
	
	for (DVM_Context *pos = dvm->current_context; pos; pos = pos->next) {
		gc_mark_ref_in_native_method(pos);
	}
	
	
	for (DVM_Context *pos = dvm->free_context; pos; pos = pos->next) {
		gc_mark_ref_in_native_method(pos);
	}
	

}



static DVM_Boolean gc_dispose_object(DVM_VirtualMachine *dvm, DVM_Object *obj){
	DVM_Boolean call_finalizer = DVM_FALSE;
	switch (obj->type) {
		case ARRAY_OBJECT:{
			switch (obj->u.array.type) {
				case INT_ARRAY:
					dvm->heap.current_heap_size -= sizeof(int) * obj->u.array.size;
					MEM_free(obj->u.array.u.int_array);
					break;
				case DOUBLE_ARRAY:
					dvm->heap.current_heap_size -= sizeof(double) * obj->u.array.size;
					MEM_free(obj->u.array.u.double_array);
					break;
				case OBJECT_ARRAY:
					dvm->heap.current_heap_size -= sizeof(DVM_ObjectRef) * obj->u.array.size;
					MEM_free(obj->u.array.u.object_array);
					break;
				case FUNCTION_INDEX_ARRAY:
					dvm->heap.current_heap_size -= sizeof(size_t) * obj->u.array.size;
					MEM_free(obj->u.array.u.function_index_array);
					break;
				default:
					break;
			}
			break;
		}
		case CLASS_OBJECT:{
			dvm->heap.current_heap_size -= sizeof(DVM_Value) * obj->u.class_object.field_count;
			MEM_free(obj->u.class_object.field);
			break;
		}
		case STRING_OBJECT:{
			if (!obj->u.string.is_literal) {
				dvm->heap.current_heap_size -= sizeof(DVM_Char) * (obj->u.string.length + 1);
				MEM_free(obj->u.string.string);
			}
			break;
		}
		case DELEGATE_OBJECT:{
			break;
		}
		case NATIVE_POINTER_OBJECT:{
			if (obj->u.native_pointer.info->finalizer) {
				obj->u.native_pointer.info->finalizer(dvm,obj);
				call_finalizer = DVM_TRUE;
			}
			break;
		}
			
			
	  default:
			DGB_assert_func(0,"obj->type..%d",obj->type);
			break;
	}
	
	dvm->heap.current_heap_size += sizeof(DVM_Object);
	MEM_free(obj);
	return call_finalizer;
	
}


static DVM_Boolean gc_sweep_object(DVM_VirtualMachine *dvm){
	DVM_Boolean call_finalizer = DVM_FALSE;
	for (DVM_Object *pos = dvm->heap.header; pos;) {
		if (!pos->marked) {
			if (pos->preview) {
				pos->preview->next = pos->next;
			}else{
				dvm->heap.header = pos->next;
			}
			
			if (pos->next) {
				pos->next->preview = pos->preview;
			}
			DVM_Object *next = pos->next;
			if (gc_dispose_object(dvm, pos)) {
				call_finalizer = DVM_TRUE;
			}
			pos = next;
		}else{
			pos = pos->next;
		}
	}
	
	return call_finalizer;

}



void dvm_garbage_collect(DVM_VirtualMachine *dvm){
	DVM_Boolean call_finalizer = DVM_FALSE;
	do {
		gc_mark_objects(dvm);
		call_finalizer = gc_sweep_object(dvm);
	} while (call_finalizer);

}
