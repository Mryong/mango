//
//  mangoc.h
//  mango_preview
//
//  Created by jerry.yong on 2017/5/25.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef mango_h
#define mango_h

#include "DVM.h"
#include "DVM_code.h"



typedef enum {
	PARSE_ERR,
	CHARACTER_INVALID_ERR,
	BAD_MULTIPLE_ERR,
	UNEXPECTED_WIDE_STRING_IN_COMPILE_ERR,
	ARRAY_ELEMENT_CAN_NOT_BE_FINAL_ERR,
	COMPLEX_ASSIGNMENT_OPERATOR_TO_FINAL_ERR,
	PARAMETER_MULTIPLE_DEFINE_ERR,
	VARIABLE_MULTIPLE_DEFINE_ERR,
	IDENTIFIER_NOT_FOUND_ERR,
	FUNCTION_IDENTIFIER_ERR,
	DERIVE_TYPE_CAST_ERR,
	CAST_MISMATCH_ERR,
	MATH_TYPE_MISMATCH_ERR,
	COMPARE_TYPE_MISMATCH_ERR,
	LOGICAL_TYPE_MISMATCH_ERR,
	MINUS_TYPE_MISMATCH_ERR,
	LOGICAL_NOT_TYPE_MISMATCH_ERR,
	INC_DEC_TYPE_MISMATCH_ERR,
	FUNCTION_NOT_IDENTIFIER_ERR,
	FUNCTION_NOT_FOUND_ERR,
	ARGUMENT_COUNT_MISMATCH_ERR,
	NOT_VALUE_ERR,
	LABEL_NOT_FOUND_ERR,
	ARRAY_LITERAL_EMPTY_ERR,
	INDEX_LEFT_OPERAND_NOT_ARRAY_ERR,
	INDEX_NOT_INT_ERR,
	ARRAY_SIZE_NOT_INT_ERR,
	DIVISION_BY_ZERO_IN_COMPILE_ERR,
	PACKAGE_NAME_TOO_LONG_ERR,
	REQUIRE_FILE_NOT_FOUND_ERR,
	REQUIRE_FILE_ERR,
	REQUIRE_DUPLICATE_ERR,
	RENAME_HAS_NO_PACKAGED_NAME_ERR,
	ABSTRACT_MULTIPLE_SPECIFIED_ERR,
	ACCESS_MODIFIER_MULTIPLE_SPECIFIED_ERR,
	OVERRIDE_MODIFIER_MULTIPLE_SPECIFIED_ERR,
	VIRTUAL_MODIFIER_MULTIPLE_SPECIFIED_ERR,
	MEMBER_EXPRESSION_TYPE_ERR,
	MEMBER_NOT_FOUND_ERR,
	PRIVATE_MEMBER_ACCESS_ERR,
	ABSTRACT_METHOD_HAS_BODY_ERR,
	CONCRETE_METHOD_HAS_NO_BODY_ERR,
	MULTIPLE_INHERITANCE_ERR,
	INHERIT_CONCRETE_CLASS_ERR,
	NEW_ABSTRACT_CLASS_ERR,
	RETURN_IN_VOID_FUNCTION_ERR,
	CLASS_NOT_FOUND_ERR,
	CONSTRUCTOR_IS_FIELD_ERR,
	NOT_CONSTRUCTOR_ERR,
	FIELD_CAN_NOT_CALL_ERR,
	ACCESS_TO_METHOD_ERR,
	NON_VIRTUAL_METHOD_OVERRIDED_ERR,
	NEED_OVERRIDED_ERR,
	ABSTRACT_METHOD_IN_CONCRETE_CLASS_ERR,
	HASNOT_SUPER_CLASS_ERR,
	SUPER_NOT_IN_MEMBER_EXPRESSION_ERR,
	FIELD_OF_SUPER_REFERENCED_ERR,
	FIELD_VOERRIDED_ERR,
	FIELD_NAME_DUPLICATE_ERR,
	ARRAY_METHOD_NOT_FOUND_ERR,
	STRING_METHOD_NOT_FOUND_ERR,
	INSTANCEOF_OPERAND_NOT_REFERENCE_ERR,
	INSTANCEOF_TYPE_NOT_REFERENCE_ERR,
	INSTANCEOF_FOR_NOT_CLASS_ERR,
	INSTANCEOF_MUST_RETURN_TRUE_ERR,
	INSTANCEOF_MUST_RETURN_FALSE_ERR,
	INSTANCEOF_INTERFACE_ERR,
	DOWN_CAST_OPERAND_IS_NOT_CLASS_ERR,
	DOWN_CAST_TARGET_IS_NOT_CLASS_ERR,
	DOWN_CAST_DO_NOTHING_ERR,
	DOWN_CAST_TO_SUPER_CLASS_ERR,
	DOWN_CAST_TO_BAD_CLASS_ERR,
	DOWN_CAST_INTERFACE_CLASS_ERR,
	CATCH_TYPE_IS_NOT_CLASS_ERR,
	CATCH_TYPE_IS_NOT_EXCEPTION_ERR,
	THROW_TYPE_IS_NOT_CLASS_ERR,
	THROW_TYPE_IS_NOT_EXCEPTION_ERR,
	RETHOROW_OUT_OF_CATCH_ERR,
	GOTO_STATEMENT_IN_FINALLY_ERR,
	THROWS_TYPE_NOT_FOUND_ERR,
	THROWS_TYPE_IS_NOT_EXCEPTION_ERR,
	EXCEPTION_HAS_TO_BE_THROWN_ERR,
	REQUIRE_ITSELF_ERR,
	IF_CONDITION_NOT_BOOLEAN_ERR,
	WHILE_CONDITION_NOT_BOOLEAN_ERR,
	FOR_CONDITION_NOT_BOOLEAN_ERR,
	DO_WHILE_CONDITION_NOT_BOOLEAN_ERR,
	CAST_TYPE_MISMATCH_ERR,
	FINAL_VARIABLE_ASSIGNMENT_ERR,
	FINAL_FIELD_ASSIGNMENT_ERR,
	FINAL_VARIABLE_WITHOUT_INITIALIZER_ERR,
	OVERRIDE_METHOD_ACCESSIBILITY_ERR,
	BAD_PARAMETER_COUNT_ERR,
	BAD_PARAMETER_TYPE_ERR,
	BAD_RETURN_TYPE_ERR,
	BAD_EXCEPTION_ERR,
	CONSTRUCTOR_CALLED_ERR,
	TYPE_NAME_NOT_FOUND_ERR,
	ENUMERATOR_NOT_FOUND_ERR,
	INTERFACE_INHERIT_ERR,
	PACKAGE_MEMBER_ACCESS_ERR,
	PACKAGE_CLASS_ACCESS_ERR,
	THIS_OUT_OF_CLASS_ERR,
	SUPER_OUT_OF_CLASS_ERR,
	BIT_NOT_TYPE_MISMATCH_ERR,
	BIT_BINARY_OPERATOR_TYPE_MISMATCH_ERR,
	EOF_IN_C_COMMENT_ERR,
	EOF_IN_STRING_LITERAL_ERR,
	COMPLILE_ERROR_COUNT_PLUS_1
}CompileError;



typedef enum {
	BOOLEAN_EXPRESSION = 1,
    INT_EXPRESSION,
	DOUBLE_EXPRESSION,
	STRING_EXPRESSION,
	IDENTIFIER_EXPRESSION,
	COMMA_EXPRESSION,
	ASSIGN_EXPRESSION,
	ADD_EXPRESSION,
	SUB_EXPRESSION,
	MUL_EXPRESSION,
	DIV_EXPRESSION,
	MOD_EXPRESSION,
	BIT_ADD_EXPRESSION,
	BIT_OR_EXPRESSION,
	BIT_XOR_EXPRESSION,
	EQ_EXPRESSION,
	NE_EXPRESSION,
	GT_EXPRESSION,
	GE_EXPRESSION,
	LT_EXPRESSION,
	LE_EXPRESSION,
	LOGICAL_AND_EXPRESSION,
	LOGICAL_OR_EXPRESSION,
	MINUS_EXPRESSION,
	BIT_NOT_EXPRESSION,
	LOGICAL_NOT_EXPRESSION,
	FUNCTION_CALL_EXPRESSION,
	MENBER_EXPRESSION,
	NULL_EXPRESSION,
	THIS_EXPRESSION,
	SUPER_EXPRESSION,
	ARRAY_LITERAL_EXPRESSION,
	INDEX_EXPRESSION,
	INCREMENT_EXPRESSION,
	DECREMENT_EXPRESSION,
	INSTANCEOF_EXPRESSION,
	DOWN_CAST_EXPRESSION,
	CASE_EXPRESSION,
	UP_CASE_EXPRESSION,
	NEW_EXPRESSION,
	ARRAY_CREATION_EXPRESSION,
	ENUMATOR_EXPRESSION,
	EXPRESSION_KIND_COUNT_PLUS_1

}ExpressionKind;

typedef struct Expression_tag Expression;
typedef struct TypeSpecifier_tag TypeSpecifier;
typedef struct ClassDefinition_tag ClassDefinition;
typedef struct DelegateDefinition_tag DelegateDefinition;
typedef struct EnumDefinition_tag EnumDefinition;
typedef struct FunctionDefinition_tag FunctionDefinition;
typedef struct ConstantDefinition_tag ConstantDefinition;
typedef struct MemberDeclaration_tag MemberDeclaration;

typedef struct Statement_tag Statement;


typedef struct PackageName_tag{
	char	*name;
	struct PackageName_tag	*next;
}PackageName;

typedef struct RequireList_tag{
	PackageName	*package_name;
	int line_number;
	struct RequireList_tag	*next;
}RequireList;


typedef struct RenameList_tag{
	PackageName	*package_name;
	char	*original_name;
	char	*renamed_name;
	int	line_number;
	struct	RequireList_tag	*next;
}RenameList;


typedef struct ArgumentList_tag {
	Expression	*expression;
	struct ArgumentList_tag	*next;
}ArgumentList;

typedef struct ParaemeterList_tag{
	TypeSpecifier	*type;
	char	*name;
	int	line_number;
	struct PackageName_tag	*next;
}ParaemeterList;

typedef enum {
	FUNCTION_DERIVE,
	ARRAY_DERIVE
}DeriveTag;


typedef struct{
	char			*identifer;
	ClassDefinition	*class_definition;
	int				line_number;
}ExceptionRef;

typedef struct ExceptionList_tag{
	ExceptionRef				*exception;
	struct ExceptionList_tag	*next;
}ExceptionList;

typedef struct {
	ParaemeterList	*parameter_list;
	ExceptionList	*throws;
}FunctionDerive;

typedef struct {
	int dummy;
}ArrayDerive;

typedef struct TypeDerive_tag{
	DeriveTag	tag;
	union{
		FunctionDerive	function_d;
		ArrayDerive		array_d;
	}u;
	struct TypeDerive_tag	*next;

}TypeDerive;





struct TypeSpecifier_tag{
	DVM_BaseType	base_type;
	char *identifier;
	union{
		struct{
			ClassDefinition	*class_definition;
			int class_index;
		}class_ref;
		
		struct{
			DelegateDefinition	*delegate_definition;
			int	delegate_index;
		}delegate_ref;
		
		struct{
			EnumDefinition *enum_definition;
			int enum_index;
		}enum_ref;
	}u;
	int	line_number;
	TypeDerive *derive;
};



typedef struct {
	TypeSpecifier	*type;
	char			*name;
	Expression		*initializer;
	DVM_Boolean		*is_final;
	int				variable_index;
	DVM_Boolean		is_loacl;
}Declaration;

typedef struct DeclarationList_tag{
	Declaration	*declaration;
	struct DeclarationList_tag	*next;
}DeclarationList;

typedef struct {
	FunctionDefinition *function_definition;
	int	function_index;
}FunctionIdentifier;



typedef struct {
	ConstantDefinition	*constant_definition;
	int	constant_index;
}ConstantIdentifier;

typedef enum {
	VARIABLE_IDENTIFER,
	FUNCTION_IDENTIFER,
	CONSTANT_IDENTIFER
}IdentifierKind;


typedef struct {
	char	*name;
	IdentifierKind	kind;
	union{
		Declaration			*declaration;
		FunctionIdentifier	*function;
		ConstantIdentifier	*constant;
	}u;
}IdentifierExpression;


typedef struct {
	Expression	*left;
	Expression	*right;
}CommaExpression;


typedef enum {
	NORMAL_ASSIGN,
	ADD_ASSIGN,
	SUB_ASSIGN,
	MUL_ASSIGN,
	DIV_ASSIGN,
	MOD_ASSIGN
}AssignmentOperator;


typedef struct{
	AssignmentOperator	operator;
	Expression	*left;
	Expression	*operand;
}AssignmentExpression;


typedef struct {
	Expression	*left;
	Expression	*right;
}BinaryExpression;


typedef struct {
	Expression	*function;
	ArgumentList	*argument;
}FunctionCallExpress;

typedef struct ExpressionList_tag{
	Expression	*expression;
	struct ExceptionList_tag	*next;
}ExpressionList;

typedef struct {
	Expression	*array;
	Expression	*index;
}IndexExpression;

typedef struct {
	char	*member_name;
	Expression	*expression;
	MemberDeclaration	*declaration;
	int method_index;
}MemberExpression;

typedef struct {
	Expression	*operand;
}IncrementOrDecrement;

typedef struct {
	Expression	*expression;
	TypeSpecifier	*type;
}InstanceofExpression;

typedef struct {
	Expression	*operand;
	TypeSpecifier	*type;
}DownCastExpression;

typedef struct {
	Expression	*operand;
	ClassDefinition	*interface_definition;
	int	interface_index;
}UpCastExpression;


typedef enum {
	INT_TO_DOUBLE_CAST,
	DOUBLE_TO_INT_CAST,
	BOOLEAN_TO_STRING_CAST,
	INT_TO_STRING_CAST,
	DOUBLE_TO_STRING_CAST,
	ENUM_TO_STRING_CAST,
	FUNCTION_TO_DELEGATE_CAST
}CastType;

typedef struct {
	CastType	type;
	Expression	*operand;
}CastExpression;


typedef struct {
	char	*class_name;
	ClassDefinition	*class_definition;
	int		*class_index;
	char	*method_name;
	MemberDeclaration	*method_delclaration;
	ArgumentList	*argument;
}NewExpression;


typedef struct ArrayDemension_tag{
	Expression	*expression;
	struct	ArgumentList_tag	*next;
}ArrayDemension;


typedef struct {
	TypeSpecifier	*type;
	ArrayDemension	*demension;
}ArrayCreation;

typedef struct Enumerator_tab {
	char	*name;
	int		value;
	struct	ExceptionList_tag	*next;
}Enumerator;


typedef struct {
	EnumDefinition	*enum_definition;
	Enumerator		*enumrator;
}EnumeratorExpression;


struct Expression_tag{
	TypeSpecifier	*type;
	ExpressionKind kind;
	int line_number;
	union {
		DVM_Boolean				boolean_value;
		int						int_value;
		double					double_value;
		DVM_Char				*string_value;
		IdentifierExpression	*identifer_express;
		CommaExpression			*comma_expression;
		AssignmentExpression	*assignment_expression;
		BinaryExpression		*binary_expression;
		Expression				*minus_expression;
		Expression				*logic_not;
		Expression				*bit_not;
		FunctionCallExpress		*function_call_expression;
		MemberExpression		*member_expression;
		ExceptionList			*array_literal;
		IndexExpression			*index_expression;
		IncrementOrDecrement	*inc_dec;
		InstanceofExpression	*instanceof;
		DownCastExpression		*down_cast;
		CastExpression			*cast;
		UpCastExpression		*up_cast;
		NewExpression			*new_e;
		ArrayCreation			*array_creation;
		EnumeratorExpression	*enumerator;
	}u;
	
};


typedef struct StatementList_tag{
	Statement	*statement;
	struct	StatementList_tag	*next;
}StatementList;



/* create.c */
Expression *mgc_alloc_expression(ExpressionKind kind);
void mgc_compile_error(int line_number,...);


/* string.c */
void mgc_open_string_literal(void);
void mgc_add_string_literal(int letter);
void mgc_rest_string_literal_buffer(void);
DVM_Char *mgc_close_string_literal(void);
int mgc_close_character_literal(void);
char *mgc_create_identifier(char *str);


#endif /* mango_h */
