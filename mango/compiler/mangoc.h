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
#include "MGC.h"
#include "MEM.h"

#define MESSAGE_ARGUMENT_MAX (256)
#define LINE_BUF_SIZE  (1024)


#define UNDEFINED_LABEL (-1)
#define UNDEFINED_ENUMERATOR (-1)
#define UNDEFINED_METHOD_INDEX (-1)

typedef enum {
	INT_MESSAGE_ARGUMENT = 1,
	DOUBLE_MESSAGE_ARGUMENT,
	STRING_MESSAGE_ARGUMENT,
	CHARACTER_MESSAGE_ARGUMENT,
	MESSAGE_ARGUMENT_END
}MessageArgumentType;



typedef enum {
	PARSE_ERR,
	CHARACTER_INVALID_ERR,
	FUNCTION_MULTIPLE_DEFINI_ERR,
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
	MEMBER_EXPRESSION,
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
	struct	RenameList_tag	*next;
}RenameList;


typedef struct ArgumentList_tag {
	Expression	*expression;
	struct ArgumentList_tag	*next;
}ArgumentList;

typedef struct ParameterList_tag{
	TypeSpecifier	*type;
	char	*name;
	int	line_number;
	struct ParameterList_tag	*next;
}ParameterList;

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
	ParameterList	*parameter_list;
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
	DVM_Boolean		is_final;
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
	struct ExpressionList_tag	*next;
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


typedef struct ArrayDimension_tag{
	Expression	*expression;
	struct	ArrayDimension_tag	*next;
}ArrayDimension;


typedef struct {
	TypeSpecifier	*type;
	ArrayDimension	*dimension;
}ArrayCreation;

typedef struct Enumerator_tag {
	char	*name;
	int		value;
	struct	Enumerator_tag	*next;
}Enumerator;


typedef struct {
	EnumDefinition	*enum_definition;
	Enumerator		*enumerator;
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
		IdentifierExpression	identifer_express;
		CommaExpression			comma_expression;
		AssignmentExpression	assignment_expression;
		BinaryExpression		binary_expression;
		Expression				*minus_expression;
		Expression				*logic_not;
		Expression				*bit_not;
		FunctionCallExpress		function_call_expression;
		MemberExpression		member_expression;
		ExpressionList			*array_literal;
		IndexExpression			index_expression;
		IncrementOrDecrement	inc_dec;
		InstanceofExpression	instanceof;
		DownCastExpression		down_cast;
		CastExpression			cast;
		UpCastExpression		up_cast;
		NewExpression			new_e;
		ArrayCreation			array_creation;
		EnumeratorExpression	enumerator;
	}u;
	
};


typedef struct StatementList_tag{
	Statement	*statement;
	struct	StatementList_tag	*next;
}StatementList;


typedef enum {
	UNDEFINED_BLOCK = 1,
	FUNCTION_BLOCK,
	WHILE_STATEMENT_BLOCK,
	FOR_STATEMENT_BLOCK,
	DO_WHILE_STATEMENT_BLOCK,
	TRY_CLAUSE_BLOCK,
	CATCH_CLAUSE_BLOCK,
	FINALLY_CLAUSE_BLOCK
}BlockType;

typedef struct {
	Statement	*statement;
	int			continue_label;
	int			break_label;
}StatementBlockInfo;


typedef struct {
	FunctionDefinition *function;
	int					end_label;
}FunctionBlockInfo;

typedef struct Block_tag{
	BlockType			type;
	StatementList		*statement_list;
	DeclarationList		*declaration_list;
	struct Block_tag	*out_block;
	union {
		FunctionBlockInfo	function_info;
		StatementBlockInfo	statement_info;
	}parent;
	

}Block;


typedef struct Elsif_tag{
	Expression	*condition;
	Block		*then_block;
	struct Elsif_tag	*next;
}Elsif;

typedef struct {
	Expression	*condition;
	Block		*then_block;
	Block		*else_block;
	Elsif		*elsif_list;
}IfStatement;

typedef struct CaseList_tag{
	ExpressionList		*expression_list;
	Block				*block;
	struct CaseList_tag	*next;
}CaseList;


typedef struct {
	Expression	*expression;
	CaseList	*case_list;
	Block		*default_block;

}SwitchStatement;

typedef struct {
	char		*label;
	Expression	*condition;
	Block		*block;

}WhileStatement;

typedef struct {
	char		*label;
	Expression	*init;
	Expression	*condition;
	Expression	*post;
	Block		*block;

}ForStatement;


typedef struct {
	char		*label;
	Block		*block;
	Expression	*condition;

}DoWhileStatement;

typedef struct {
	char		*label;
	char		*variable;
	Expression	*collection;
	Block		*block;
}ForeachStatement;


typedef struct {
	Expression	*return_value;
}ReturnStatement;



typedef struct {
	char	*label;
}BreakStatement;

typedef struct {
	char	*label;
}ContinueStatement;

typedef struct CatchClause_tag{
	char			*variable_name;
	Declaration		*variable_declaration;
	TypeSpecifier	*type;
	Block			*block;
	int				line_number;
	struct CatchClause_tag	*next;
	
}CatchClause;

typedef struct {
	Block		*try_block;
	CatchClause	*catch_clause;
	Block		*finally_block;
}TryStatement;

typedef struct {
	Expression	*expression;
	Declaration	*variable_declaration;
}ThrowsStatement;


typedef enum {
	EXPRESSION_STATEMENT = 1,
	IF_STATEMENT,
	SWITCH_STATEMENT,
	WHILE_STATEMENT,
	FOR_STATEMENT,
	DO_WHILE_STATEMENT,
	FOREACH_STATEMENT,
	RETURN_STATEMENT,
	BREAK_STATEMENT,
	CONTINUE_STATEMENT,
	TRY_STATEMENT,
	THROW_STATEMENT,
	DECLARATION_STATEMENT,
	STATEMENT_TYPE_CONT_PLUS_1
}StatementType;

struct Statement_tag {
	StatementType		type;
	int					line_number;
	union {
		Expression		*expression_s;
		IfStatement		if_s;
		SwitchStatement	switch_s;
		WhileStatement	while_s;
		ForStatement	for_s;
		DoWhileStatement	do_while_s;
		ForeachStatement	foreach_s;
		BreakStatement		break_s;
		ContinueStatement	continue_s;
		ReturnStatement		return_s;
		TryStatement		try_s;
		ThrowsStatement		throw_s;
		Declaration			*declaration_s;
	}u;
};

struct	FunctionDefinition_tag {
	TypeSpecifier		*type;
	PackageName			*package_name;
	char				*name;
	ParameterList		*parameter_list;
	Block				*block;
	int					*local_variable_count;
	Declaration			**local_variable;
	ClassDefinition		*class_definition;
	ExceptionList		*throws;
	int					end_line_number;
	struct FunctionDefinition_tag	*next;
};



typedef enum {
	ABSTRACT_MODIFIER,
	PUBLIC_MODIFIER,
	PRIVATE_MODIFIER,
	OVERRIDE_MODIFIER,
	VIRTUAL_MODIFIER,
	NOT_SPECIFIED_MODIFIER

}ClassOrMemberModifierKind;


typedef struct {
	ClassOrMemberModifierKind	id_abstract;
	ClassOrMemberModifierKind	access_modifier;
	ClassOrMemberModifierKind	is_virtual;
	ClassOrMemberModifierKind	is_override;
}ClassOrMemberModifierList;


typedef struct ExtendsList_tag{
	char	*identifer;
	ClassDefinition	*class_definition;
	struct	ExtendsList_tag	*next;
}ExtendsList;

typedef enum {
	METHOD_MEMBER,
	FIELD_MEMBER
}MemberKind;

typedef struct {
	DVM_Boolean			is_constructor;
	DVM_Boolean			is_abstract;
	DVM_Boolean			is_virtual;
	DVM_Boolean			is_override;
	FunctionDefinition	*function_definition;
	int					method_index;
	
}MethodMember;


typedef struct {
	char			*name;
	TypeSpecifier	*type;
	DVM_Boolean		*is_final;
	Expression		*initializer;
	int				field_index;
}FieldMember;

struct MemberDeclaration_tag{
	MemberKind			kind;
	DVM_AccessModifier	access_modifier;
	union {
		FieldMember		field;
		MethodMember	method;
	}u;
	int line_number;
	struct MemberDeclaration_tag	*next;
};

struct ClassDefinition_tag {
	DVM_Boolean		is_abstract;
	DVM_AccessModifier	access_modifier;
	DVM_ClassOrInterface class_or_interface;
	PackageName	*package_name;
	char		*name;
	ExtendsList	*extends;
	ClassDefinition	*super_class;
	ExtendsList	*interface_list;
	MemberDeclaration	*member;
	int line_number;
	struct	ClassDefinition_tag	*next;
};

typedef struct CompilerList_tag{
	MGC_Compiler	*compiler;
	struct	CompilerList_tag	*next;
}CompilerList;


typedef enum {
	MGH_SOURCE,
	MGM_SOURCE
}SourceSuffix;

typedef enum {
	FILE_INPUT_MODE,
	STRING_INPUT_MODE
}InputMode;

typedef struct {
	InputMode	mode;
	union {
		struct {
			FILE	*fp;
		}file;
		struct {
			char	**lines;
		}string;
	}u;
}SourceInput;


struct DelegateDefinition_tag {
	char			*name;
	TypeSpecifier	*type;
	ParameterList	*parameter_list;
	ExceptionList	*throws;
	DelegateDefinition	*next;
};

struct EnumDefinition_tag{
	PackageName		*package_name;
	char			*name;
	Enumerator		*enumerator;
	int				index;
	EnumDefinition	*next;
};

struct ConstantDefinition_tag {
	PackageName			*package_name;
	TypeSpecifier		*type;
	char				*name;
	Expression			*initializer;
	int					index;
	int					line_number;
	ConstantDefinition	*next;
};


typedef enum {
	EUC_ENCODING = 1,
	UTF_8_ENCODING
}Encoding;

typedef struct {
	char	*string;
}VString;

typedef struct {
	DVM_Char	*string;
}VWString;

typedef struct {
	char	*package_name;
	SourceSuffix	suffix;
	char	**source_string;
}BuiltinScript;


struct MGC_Compiler_tag{
	MEM_Storage			compile_storage;
	PackageName			*package_name;
	SourceSuffix		source_suffix;
	char				*path;
	RequireList			*require_list;
	RenameList			*rename_list;
	FunctionDefinition	*function_list;
	StatementList		*statement_list;
	DeclarationList		*declaration_list;
	ClassDefinition		*class_definition_list;
	DelegateDefinition	*delegate_definition_list;
	EnumDefinition		*enum_definition_list;
	ConstantDefinition	*constant_definition_list;
	int					current_line_number;
	Block				*current_block;
	ClassDefinition		*current_class_definition;
	TryStatement		*current_try_statement;
	CatchClause			*current_catch_clause;
	int					*current_finally_label;
	InputMode			input_mode;
	CompilerList		*required_list;
	int					array_method_count;
	FunctionDefinition	*array_method;
	int					string_method_count;
	FunctionDefinition	*string_method;
	Encoding			source_encoding;
	
	int					dvm_function_count;
	DVM_Function		*dvm_function;
	int					dvm_enum_count;
	DVM_Enum			*dvm_enum;
	int					dvm_constant_count;
	DVM_Constant		*dvm_constant;
	int					dvm_class_count;
	DVM_Class			*dvm_class;
	
	
	

};






/* mango.l */
void mgc_set_source_string(char **source);


/* create.c */
DeclarationList *mgc_chain_declaration(DeclarationList *list,Declaration *decl);
Declaration	*mgc_alloc_declaration(DVM_Boolean is_final,TypeSpecifier *type, char *identifier);
PackageName *mgc_create_package_name(char *identifier);
PackageName	*mgc_chain_package_name(PackageName *list, char *identifier);

RequireList *mgc_create_require_list(PackageName *package_name);
RequireList *mgc_chain_require_list(RequireList *list,RequireList *add);

RenameList *mgc_create_rename_list(PackageName *package_name, char *identifer);
RenameList *mgc_chain_rename_list(RenameList *list, RenameList *add);

void mgc_set_require_and_rename_list(RequireList *require_list, RenameList *rename_list);

FunctionDefinition *mgc_create_function_definition(TypeSpecifier *type, char *identifier,
												   ParameterList *parameter_list, ExceptionList *exception_list,
												   Block *block);
void mgc_function_define(TypeSpecifier *type, char *identifier,
						 ParameterList *parameter_list, ExceptionList *exception_list,
						 Block *block);

ParameterList *mgc_create_parameter(TypeSpecifier *type, char *identifier);
ParameterList *mgc_chain_parameter(ParameterList *list, TypeSpecifier *type, char *identifier);

ArgumentList *mgc_create_argument(Expression *expression);
ArgumentList *mgc_chain_argument(ArgumentList *list,Expression *expression);

ExpressionList *mgc_create_expression_list(Expression *expression);
ExpressionList *mgc_chain_expression_list(ExpressionList *list, Expression *expression);


StatementList *mgc_create_statement_list(Statement *statement);
StatementList *mgc_chain_statement_list(StatementList *statement_list, Statement *statement);



TypeSpecifier *mgc_create_type_specifier(DVM_BaseType base_type);
TypeSpecifier *mgc_create_identifier_type_specifier(char *identifier);
TypeSpecifier *mgc_create_array_type_specifier(TypeSpecifier *base);


Expression *mgc_alloc_expression(ExpressionKind kind);
Expression *mgc_create_comma_expression(Expression *left, Expression *right);
Expression *mgc_create_assign_expression(Expression *left, AssignmentOperator operator, Expression *operand);
Expression *mgc_create_binary_expression(ExpressionKind operator, Expression *left, Expression *right);
Expression *mgc_create_minus_expression(Expression *operand);
Expression *mgc_create_logic_not_expression(Expression *operand);
Expression *mgc_create_bit_not_expression(Expression *operand);
Expression *mgc_create_index_expression(Expression *array,Expression *index);
Expression *mgc_create_incdec_expression(Expression *operand,ExpressionKind inc_or_dec);
Expression *mgc_create_instanceof_expression(Expression *operand, TypeSpecifier *type);
Expression *mgc_create_identifier_expression(char *identifier);
Expression *mgc_create_function_call_expression(Expression *function, ArgumentList *argument);
Expression *mgc_create_down_cast_expression(Expression *operand,TypeSpecifier *type);
Expression *mgc_create_member_expression(Expression *expression, char *member_name);
Expression *mgc_create_boolean_expression(DVM_Boolean value);
Expression *mgc_create_null_expression(void);
Expression *mgc_create_new_expression(char *class_name, char *method_name, ArgumentList *argument);
Expression *mgc_create_array_literal_expression(ExpressionList *list);
Expression *mgc_create_base_array_creation(DVM_BaseType base_type,
										   ArrayDimension *dim_expr_list,
										   ArrayDimension *dim_list);
Expression *mgc_create_class_array_creation(TypeSpecifier *type,
											ArrayDimension *dim_expr_list,
											ArrayDimension *dim_list);
Expression *mgc_create_this_expression(void);
Expression *mgc_create_super_expression(void);

ArrayDimension *mgc_create_array_dimension(Expression *expression);
ArrayDimension *mgc_chain_array_dimension(ArrayDimension *list,
										  ArrayDimension *dim);

Statement *mgc_alloc_statement(StatementType type);
Statement *mgc_create_if_statement(Expression *condition,
								   Block *then_block, Elsif *elsif_list,
								   Block *else_block);

Elsif *mgc_create_elsif_statement(Expression *expr, Block *block);
Elsif *mgc_chain_elsif_statement(Elsif *list, Elsif *add);

Statement *mgc_create_switch_statement(Expression *expr,
									   CaseList *case_list,
									   Block *default_block);

CaseList *mgc_create_one_case(ExpressionList *expression_list, Block *block);
CaseList *mgc_chain_case(CaseList *list, CaseList *add);

Statement *mgc_create_while_statement(char *label,
									  Expression *condition,
									  Block *block);

Statement *mgc_create_foreach_statement(char *label,
										char *variable,
										Expression *collection,
										Block *block);

Statement *mgc_create_for_statement(char *label,
									Expression *init,
									Expression *condition,
									Expression *post,
									Block *block);

Statement *mgc_create_do_while_statement(char *label,
										 Block *block,
										 Expression *condition);


Block *mgc_alloc_block(void);
Block *mgc_open_block(void);
Block *mgc_close_block(Block *block, StatementList *statement_list);

Statement *mgc_create_expression_statement(Expression *expression);
Statement *mgc_create_return_statement(Expression *expression);
Statement *mgc_create_break_statement(char *label);
Statement *mgc_create_continue_statement(char *label);
Statement *mgc_create_try_statement(Block *try_block,
									CatchClause *catch_clause,
									Block *finally_block);

CatchClause *mgc_create_catch_clause(TypeSpecifier *type,
									 char *variable_name,
									 Block *block);
CatchClause *mgc_start_catch_clause(void);
CatchClause *mgc_end_catch_clause(CatchClause *catch_clause,
								  TypeSpecifier *type,
								  char *variable_name,
								  Block *block);
CatchClause *mgc_chain_catch_clause(CatchClause *list, CatchClause *add);


Statement *mgc_create_throw_statement(Expression *expression);
Statement *mgc_create_declaration_statement(DVM_Boolean is_final,
											TypeSpecifier *type,
											char *identifier,
											Expression *initializer);

void mgc_start_class_definition(ClassOrMemberModifierList *modifier,
								DVM_ClassOrInterface class_or_interface,
								char *identifier,
								ExtendsList *extends);

void mgc_class_define(MemberDeclaration *member_list);
ExtendsList *mgc_create_extends_list(char *identifier);
ExtendsList *mgc_chain_extends_list(ExtendsList *list, char *add);

ClassOrMemberModifierList mgc_create_class_or_member_modifier(ClassOrMemberModifierKind kind);
ClassOrMemberModifierList mgc_chain_class_or_member_modifier(ClassOrMemberModifierList list,
																  ClassOrMemberModifierList add);

MemberDeclaration *mgc_chain_member_declaration_list(MemberDeclaration *list,
													 MemberDeclaration *add);

MemberDeclaration *mgc_create_method_member(ClassOrMemberModifierList *modifier,
											FunctionDefinition *function_definition,
											DVM_Boolean is_constructor);
FunctionDefinition *mgc_method_function_definition(TypeSpecifier *type,
												   char *identifier,
												   ParameterList *parameter,
												   ExceptionList *thorws,
												   Block *block);
FunctionDefinition *mgc_constructor_function_definition(char *identifier,
														ParameterList *parameter,
														ExceptionList *throws,
														Block *block);

MemberDeclaration *mgc_create_field_member(ClassOrMemberModifierList *modifier,
										   DVM_Boolean *is_final,
										   TypeSpecifier *type,
										   char *name,
										   Expression *initializer);

ExceptionList *mgc_create_thorws(char *identifer);
ExceptionList *mgc_chain_exception_list(ExceptionList *list, char *identifier);

void mgc_create_delegate_definition(TypeSpecifier *type, char *identifier,
									ParameterList *parameter, ExceptionList *thorws);


void mgc_create_enum_definition(char *identifier, Enumerator *enumerator);
Enumerator *mgc_create_enumerator(char *identifier);
Enumerator *mgc_chain_enumerator(Enumerator *enumerator, char *identifier);

void mgc_create_const_definition(TypeSpecifier *type,
								 char *identifier,
								 Expression *initializer);




void mgc_compile_error(int line_number,CompileError err,...);


/* string.c */
void mgc_open_string_literal(void);
void mgc_add_string_literal(int letter);
void mgc_rest_string_literal_buffer(void);
DVM_Char *mgc_close_string_literal(void);
int mgc_close_character_literal(void);
char *mgc_create_identifier(char *str);

/* fix_tree.c */
void mgc_fix_tree(MGC_Compiler *compiler);

/* generate.c */
DVM_Executable *mgc_generate(MGC_Compiler *compiler);

/* util.c */
MGC_Compiler *mgc_get_current_compiler(void);
void mgc_set_current_compiler(MGC_Compiler *compiler);
void *mgc_malloc(size_t size);
char *mgc_package_name_to_string(PackageName *package_name);
DVM_Boolean mgc_equal_package_name(PackageName *package_name1, PackageName *package_name2);
FunctionDefinition *mgc_search_function(char *name);
Declaration *mgc_search_declaration(char *identifier, Block *block);
ConstantDefinition *mgc_search_constant(char *identifier);
ClassDefinition *mgc_search_class(char *identifier);
DelegateDefinition *mgc_search_delegate(char *identifier);
EnumDefinition *mg_search_enum(char *identifier);
MemberDeclaration *mgc_search_member(ClassDefinition *class_def,char *member_name);
TypeSpecifier *mgc_alloc_type_specifier(DVM_BaseType type);
TypeDerive *mgc_alloc_type_derive(DeriveTag derive_tag);


#endif /* mango_h */
