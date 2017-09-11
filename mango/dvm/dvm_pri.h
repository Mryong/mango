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

#define STACK_ALLOC_SIZE (4069)
#define HEAP_THRESHOLD_SIZE (1024*256)
#define ARRAY_ALLOC_SIZE (256)
#define NULL_STRING (L"null")
#define TRUE_STRING (L"true")
#define FALSE_STRING (L"false")
#define BUILT_IN_METHOD_PACKAGE_NAME ("$built_in")

#define NO_LINE_NUMBER_PC (-1)
#define FUNCTION_NOT_FOUND (-1)
#define ENUM_NOT_FOUND (-1)
#define CONSTANT_NOT_FOUND (-1)
#define FIELD_NOT_FOUND (-1)
#define CALL_FORM_NATIVE (-1)

#define GET_2BYTE_INT(p) (((p)[0] << 8) + (p)[1])   
#define SET_2BYTE_INT(p, value) (p)[0] = (((value)>>8) & 0xff), ((p)[0] = (value) & 0xff)



typedef struct DVM_Array_tag DVM_Array;

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
	DVM_NativeFunctionProc *proc;
	size_t argc;
	DVM_Boolean is_method;
	DVM_Boolean return_pointer;
}NativeFunction;

typedef struct ExecutableEntry_tag ExecutableEntry;

typedef struct {
	ExecutableEntry *exe;
	size_t index;
}MangoFunction;

typedef struct {
	char	*package_name;
	char	*name;
	FunctionKind kind;
	DVM_Boolean is_implemented;
	union{
		NativeFunction native_f;
		MangoFunction mango_f;
	}u;

}Function;


typedef struct {
	Function *caller;
	size_t	caller_address;
	int		base;
}CallInfo;

#define revalue_up_align(val)  ((val) ? (((val) - 1) / sizeof(DVM_Value) + 1) : 0)
#define CALL_INFO_ALIGN_ZISE revalue_up_align(sizeof(CallInfo))

typedef union {
	CallInfo s;
	DVM_Value u[CALL_INFO_ALIGN_ZISE];
}CallInfoUnion;

typedef struct {
	size_t	alloc_size;
	size_t	stack_pointer;
	DVM_Value	*stack;
	DVM_Boolean	*pointer_flags;

}Stcak;

typedef enum {
	STRING_OBJECT = 1,
	ARRAY_OBJECT,
	CLASS_OBJECT,
	NATIVE_POINTER_OBJECT,
	DELEGATE_OBJECT,
	OBJECT_TYPE_COUNT_PLUS_1
}ObjectType;


struct DVM_String_tag {
	DVM_Boolean is_literal;
	size_t		length;
	DVM_Char	*string;
};

typedef enum {
	INT_ARRAY = 1,
	DOUBLE_ARRAY,
	OBJECT_ARRAY,
	FUNCTION_INDEX_ARRAY

}ArrayType;

struct DVM_Array_tag{
	ArrayType	type;
	size_t		size;
	size_t		alloc_size;
	union{
		int		*int_array;
		double	*double_array;
		DVM_ObjectRef *object_array;
		size_t	function_index_array;
	}u;
};

typedef struct {
	size_t		field_count;
	DVM_Value	*field;
}DVM_ClassObject;


typedef struct{
	void	*pointer;
	DVM_NativeLibInfo	*info;
} NativePointer;

typedef struct {
	DVM_ObjectRef	*obj;
	size_t			index;
}Delegate;

struct DVM_Object_tag{
	ObjectType	type;
	unsigned int	marked:1;
	union{
		DVM_String string;
		DVM_Array array;
		DVM_ClassObject	class_object;
		Delegate		delegate;
	}u;
	struct DVM_Object_tag	*preview;
	struct DVM_Object_tag	*next;
};


typedef struct {
	size_t	current_heap_size;
	size_t	current_threshold;
	DVM_Object	*header;

}Heap;

typedef struct {
	size_t		vars_count;
	DVM_Value	*vars;

}Static;

typedef struct ExecClass_tag{
	DVM_Class				*dvm_calss;
	ExecutableEntry			*executable;
	char					*package_name;
	char					*name;
	DVM_Boolean				is_implemented;
	size_t					class_index;
	struct ExecClass_tag	*super_class;
	DVM_VTable				*class_table;
	size_t					interface_count;
	struct ExecClass_tag	**interface;
	DVM_VTable				**interface_table;
	size_t					field_count;
	DVM_TypeSpecifier		**field_type;
}ExecClass;


typedef struct {
	char	*name;
	size_t	index;

}VTableItem;

struct DVM_VTable_tag{
	ExecClass	*exec_class;
	size_t		table_size;
	VTableItem	*table;
};

struct ExecutableEntry_tag{
	DVM_Executable *executable;
	size_t			*function_table;
	size_t			*class_table;
	size_t			*enum_table;
	size_t			*constant_table;
	Static			static_v;
	struct ExecutableEntry_tag	*next;
};

typedef struct {
	char	*package_name;
	char	*name;
	DVM_Boolean	is_defined;
	DVM_Enum	*dvm_enum;

}Enum;

typedef struct {
	char	*package_name;
	char	*name;
	DVM_Boolean	is_defined;
	DVM_Value	value;

}Constant;

struct DVM_VirtualMachine_tag{
	Stcak	stack;
	Heap	heap;
	ExecutableEntry	*current_executable;
	Function		*current_function;
	DVM_ObjectRef	current_exception;
	size_t			pc;
	Function		**function;
	size_t			function_count;
	ExecClass		**class_;
	size_t			class_count;
	Enum			**enum_;
	size_t			enum_count;
	Constant		**constant;
	size_t			constant_count;
	DVM_ExecutableList	*executable_list;
	ExecutableEntry		*executable_entry;
	ExecutableEntry		*top_level;
	DVM_VTable			*array_v_table;
	DVM_VTable			*string_v_table;
	DVM_Context			*current_context;
	DVM_Context			*free_context;
};


typedef struct RefNativeFunc_tag{
	DVM_ObjectRef	object;
	struct	RefNativeFunc_tag *next;
}RefNativeFunc;


/* native.c */
void dvm_add_native_functions(DVM_VirtualMachine *dvm);

/* load.c */
void dvm_add_native_function(DVM_VirtualMachine *dvm, char *package_name, char *name, DVM_NativeFunctionProc *proc,
							 size_t argc, DVM_Boolean is_method, DVM_Boolean return_pointer);

/* util.c */
void dvm_initial_value(DVM_TypeSpecifier *type, DVM_Value *value);


/* execute.c */
void dvm_expend_stack(DVM_VirtualMachine *dvm, size_t need_stack_size);
DVM_Value *dvm_execute_i(DVM_VirtualMachine *dvm, Function *func, DVM_Byte *code, size_t code_size, size_t base);












#endif /* dvm_pri_h */
