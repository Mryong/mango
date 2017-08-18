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

typedef struct DVM_TypeSpecifier_tag DVM_TypeSpecifier;

typedef struct {
	char				*name;
	DVM_TypeSpecifier	*type;
}DVM_LocalVariable;


typedef enum {
	DVM_FUNCTION_DERIVE,
	DVM_ARRAY_DERIVE
}DVM_DeriveTag;


typedef struct {
	size_t					parameter_count;
	DVM_LocalVariable	*parameter;
}DVM_FunctionDerive;

typedef struct {
	int dummy;
}DVM_ArrayDerive;


typedef struct {
	DVM_DeriveTag	tag;
	union{
		DVM_FunctionDerive function_d;
	}u;

}DVM_TypeDerive;

struct DVM_TypeSpecifier_tag{
	DVM_BaseType		base_type;
	union{
		struct{
			size_t index;
		}class_t;
		struct{
			size_t dummmy;
		}delegate_t;
		struct{
			size_t index;
		}enum_t;
	}u;
	size_t derive_count;
	DVM_TypeDerive *derive;
};

typedef enum {
	DVM_PUSH_INT_1BUTE = 1
}DVM_Opcode;


typedef struct {
	size_t		line_number;
	size_t		start_pc;
	size_t		pc_count;
}DVM_LineNumber;


typedef struct {
	size_t		class_index;
	size_t		start_pc;
	size_t		pc_count;
}DVM_CatchClause;

typedef struct {
	size_t					try_start_pc;
	size_t					try_end_pc;
	size_t					catch_count;
	DVM_CatchClause		*catch_clause;
	size_t					finally_start_pc;
	size_t					finally_end_pc;
}DVM_Try;



typedef struct {
	size_t				code_size;
	DVM_Byte		*code;
	size_t				line_number_size;
	size_t				try_size;
	DVM_Try			*try;
	size_t				need_stack_size;
}DVM_CodeBlock;


typedef struct {
	DVM_TypeSpecifier	*type;
	char				*package_name;
	char				*name;
	size_t					parameter_count;
	DVM_LocalVariable	*parameter;
	DVM_Boolean			is_implemented;
	DVM_Boolean			is_method;
	size_t					local_variable_count;
	DVM_LocalVariable	*locak_variable;
	DVM_CodeBlock		code_block;
}DVM_Function;


typedef enum {
	DVM_PRIVATE_MODIFIER ,
	DVM_FILE_MODIFIER,
	DVM_PUBLIC_MODIFIER
}DVM_AccessModifier;


typedef enum {
	DVM_CLASS_DEFINITION,
	DVM_INTERFACE_DEFINITION
}DVM_ClassOrInterface;


typedef struct {
	DVM_AccessModifier	access_modifier;
	char				*name;
	DVM_TypeSpecifier	*type;

}DVM_Field;

typedef struct {
	DVM_AccessModifier	access_modifier;
	DVM_Boolean			is_abstract;
	DVM_Boolean			is_virtual;
	DVM_Boolean			is_override;
	char				*name;
}DVM_Method;



typedef struct {
	char	*package_name;
	char	*name;
}DVM_ClassIdentifier;

typedef struct {
	DVM_Boolean				is_abstract;
	DVM_AccessModifier		access_modifier;
	DVM_ClassOrInterface	class_or_interface;
	char					*package_name;
	char					*name;
	DVM_Boolean				is_implemented;
	DVM_ClassIdentifier		*super_class;
	size_t						interface_count;
	DVM_ClassIdentifier		*interface_;
	size_t						field_count;
	DVM_Field				*field;
	size_t						method_count;
	DVM_Method				*method;
	DVM_CodeBlock			field_initializer;

}DVM_Class;


typedef enum {
	DVM_CONSTANT_INT,
	DVM_CONSTANT_DOUBLE,
	DVM_CONSTANT_STRING
}DVM_ConstantPoolTag;

typedef struct {
	DVM_ConstantPoolTag tag;
	union{
		int			c_int;
		double		c_double;
		DVM_Char	*c_string;
	}u;
} DVM_ConstantPool;


typedef struct {
	char				*name;
	DVM_TypeSpecifier	*type;
}DVM_Variable;

typedef struct {
	char	*package_name;
	char	*name;
	DVM_Boolean	is_defined;
	size_t			enumerator_count;
	char		**enumerator;
}DVM_Enum;

typedef struct {
	char	*package_name;
	char	*name;
	DVM_TypeSpecifier	*type;
	DVM_Boolean		is_defined;
}DVM_Constant;






struct DVM_Executable_tag{
	char				*package_name;
	DVM_Boolean			is_required;
	char				*path;
	size_t					constant_pool_count;
	DVM_ConstantPool	*constant_pool;
	size_t					global_variable_count;
	DVM_Variable		*global_variable;
	size_t					function_count;
	DVM_Function		*function;
	size_t					type_specifier_count;
	DVM_TypeSpecifier	*type_specifier;
	size_t					class_count;
	DVM_Class			*class_definition;
	size_t					enum_count;
	DVM_Enum			*enum_definition;
	size_t					constant_count;
	DVM_Constant		*constant_definition;
	DVM_CodeBlock		top_level;
	DVM_CodeBlock		constant_initializer;
};


typedef struct DVM_ExecutableItem_tag{
	DVM_Executable	*executable;
	struct DVM_ExecutableItem_tag *next;
}DVM_ExecutableItem;


struct DVM_ExecutableList_tag{
	DVM_Executable		*top_level;
	DVM_ExecutableItem	*list;
};


#endif /* DVM_code_h */
