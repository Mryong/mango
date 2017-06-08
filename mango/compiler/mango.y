%{
#include <stdio.h>
#include "mangoc.h"
#define YYDEBUG 1
#define YYERROR_VERBOSE




// typedef enum {
// 	PARSE_ERR,
// 	CHARACTER_INVALID_ERR,
// 	BAD_MULTIPLE_ERR,
// 	UNEXPECTED_WIDE_STRING_IN_COMPILE_ERR,
// 	ARRAY_ELEMENT_CAN_NOT_BE_FINAL_ERR,
// 	COMPLEX_ASSIGNMENT_OPERATOR_TO_FINAL_ERR,
// 	PARAMETER_MULTIPLE_DEFINE_ERR,
// 	VARIABLE_MULTIPLE_DEFINE_ERR,
// 	IDENTIFIER_NOT_FOUND_ERR,
// 	FUNCTION_IDENTIFIER_ERR,
// 	DERIVE_TYPE_CAST_ERR,
// 	CAST_MISMATCH_ERR,
// 	MATH_TYPE_MISMATCH_ERR,
// 	COMPARE_TYPE_MISMATCH_ERR,
// 	LOGICAL_TYPE_MISMATCH_ERR,
// 	MINUS_TYPE_MISMATCH_ERR,
// 	LOGICAL_NOT_TYPE_MISMATCH_ERR,
// 	INC_DEC_TYPE_MISMATCH_ERR,
// 	FUNCTION_NOT_IDENTIFIER_ERR,
// 	FUNCTION_NOT_FOUND_ERR,
// 	ARGUMENT_COUNT_MISMATCH_ERR,
// 	NOT_VALUE_ERR,
// 	LABEL_NOT_FOUND_ERR,
// 	ARRAY_LITERAL_EMPTY_ERR,
// 	INDEX_LEFT_OPERAND_NOT_ARRAY_ERR,
// 	INDEX_NOT_INT_ERR,
// 	ARRAY_SIZE_NOT_INT_ERR,
// 	DIVISION_BY_ZERO_IN_COMPILE_ERR,
// 	PACKAGE_NAME_TOO_LONG_ERR,
// 	REQUIRE_FILE_NOT_FOUND_ERR,
// 	REQUIRE_FILE_ERR,
// 	REQUIRE_DUPLICATE_ERR,
// 	RENAME_HAS_NO_PACKAGED_NAME_ERR,
// 	ABSTRACT_MULTIPLE_SPECIFIED_ERR,
// 	ACCESS_MODIFIER_MULTIPLE_SPECIFIED_ERR,
// 	OVERRIDE_MODIFIER_MULTIPLE_SPECIFIED_ERR,
// 	VIRTUAL_MODIFIER_MULTIPLE_SPECIFIED_ERR,
// 	MEMBER_EXPRESSION_TYPE_ERR,
// 	MEMBER_NOT_FOUND_ERR,
// 	PRIVATE_MEMBER_ACCESS_ERR,
// 	ABSTRACT_METHOD_HAS_BODY_ERR,
// 	CONCRETE_METHOD_HAS_NO_BODY_ERR,
// 	MULTIPLE_INHERITANCE_ERR,
// 	INHERIT_CONCRETE_CLASS_ERR,
// 	NEW_ABSTRACT_CLASS_ERR,
// 	RETURN_IN_VOID_FUNCTION_ERR,
// 	CLASS_NOT_FOUND_ERR,
// 	CONSTRUCTOR_IS_FIELD_ERR,
// 	NOT_CONSTRUCTOR_ERR,
// 	FIELD_CAN_NOT_CALL_ERR,
// 	ACCESS_TO_METHOD_ERR,
// 	NON_VIRTUAL_METHOD_OVERRIDED_ERR,
// 	NEED_OVERRIDED_ERR,
// 	ABSTRACT_METHOD_IN_CONCRETE_CLASS_ERR,
// 	HASNOT_SUPER_CLASS_ERR,
// 	SUPER_NOT_IN_MEMBER_EXPRESSION_ERR,
// 	FIELD_OF_SUPER_REFERENCED_ERR,
// 	FIELD_VOERRIDED_ERR,
// 	FIELD_NAME_DUPLICATE_ERR,
// 	ARRAY_METHOD_NOT_FOUND_ERR,
// 	STRING_METHOD_NOT_FOUND_ERR,
// 	INSTANCEOF_OPERAND_NOT_REFERENCE_ERR,
// 	INSTANCEOF_TYPE_NOT_REFERENCE_ERR,
// 	INSTANCEOF_FOR_NOT_CLASS_ERR,
// 	INSTANCEOF_MUST_RETURN_TRUE_ERR,
// 	INSTANCEOF_MUST_RETURN_FALSE_ERR,
// 	INSTANCEOF_INTERFACE_ERR,
// 	DOWN_CAST_OPERAND_IS_NOT_CLASS_ERR,
// 	DOWN_CAST_TARGET_IS_NOT_CLASS_ERR,
// 	DOWN_CAST_DO_NOTHING_ERR,
// 	DOWN_CAST_TO_SUPER_CLASS_ERR,
// 	DOWN_CAST_TO_BAD_CLASS_ERR,
// 	DOWN_CAST_INTERFACE_CLASS_ERR,
// 	CATCH_TYPE_IS_NOT_CLASS_ERR,
// 	CATCH_TYPE_IS_NOT_EXCEPTION_ERR,
// 	THROW_TYPE_IS_NOT_CLASS_ERR,
// 	THROW_TYPE_IS_NOT_EXCEPTION_ERR,
// 	RETHOROW_OUT_OF_CATCH_ERR,
// 	GOTO_STATEMENT_IN_FINALLY_ERR,
// 	THROWS_TYPE_NOT_FOUND_ERR,
// 	THROWS_TYPE_IS_NOT_EXCEPTION_ERR,
// 	EXCEPTION_HAS_TO_BE_THROWN_ERR,
// 	REQUIRE_ITSELF_ERR,
// 	IF_CONDITION_NOT_BOOLEAN_ERR,
// 	WHILE_CONDITION_NOT_BOOLEAN_ERR,
// 	FOR_CONDITION_NOT_BOOLEAN_ERR,
// 	DO_WHILE_CONDITION_NOT_BOOLEAN_ERR,
// 	CAST_TYPE_MISMATCH_ERR,
// 	FINAL_VARIABLE_ASSIGNMENT_ERR,
// 	FINAL_FIELD_ASSIGNMENT_ERR,
// 	FINAL_VARIABLE_WITHOUT_INITIALIZER_ERR,
// 	OVERRIDE_METHOD_ACCESSIBILITY_ERR,
// 	BAD_PARAMETER_COUNT_ERR,
// 	BAD_PARAMETER_TYPE_ERR,
// 	BAD_RETURN_TYPE_ERR,
// 	BAD_EXCEPTION_ERR,
// 	CONSTRUCTOR_CALLED_ERR,
// 	TYPE_NAME_NOT_FOUND_ERR,
// 	ENUMERATOR_NOT_FOUND_ERR,
// 	INTERFACE_INHERIT_ERR,
// 	PACKAGE_MEMBER_ACCESS_ERR,
// 	PACKAGE_CLASS_ACCESS_ERR,
// 	THIS_OUT_OF_CLASS_ERR,
// 	SUPER_OUT_OF_CLASS_ERR,
// 	BIT_NOT_TYPE_MISMATCH_ERR,
// 	BIT_BINARY_OPERATOR_TYPE_MISMATCH_ERR,
// 	EOF_IN_C_COMMENT_ERR,
// 	EOF_IN_STRING_LITERAL_ERR,
// 	COMPLILE_ERROR_COUNT_PLUS_1
// }CompileError;



// typedef enum {
// 	BOOLEAN_EXPRESSION = 1,
//     INT_EXPRESSION,
// 	DOUBLE_EXPRESSION,
// 	STRING_EXPRESSION,
// 	IDENTIFIER_EXPRESSION,
// 	COMMA_EXPRESSION,
// 	ASSIGN_EXPRESSION,
// 	ADD_EXPRESSION,
// 	SUB_EXPRESSION,
// 	MUL_EXPRESSION,
// 	DIV_EXPRESSION,
// 	MOD_EXPRESSION,
// 	BIT_ADD_EXPRESSION,
// 	BIT_OR_EXPRESSION,
// 	BIT_XOR_EXPRESSION,
// 	EQ_EXPRESSION,
// 	NE_EXPRESSION,
// 	GT_EXPRESSION,
// 	GE_EXPRESSION,
// 	LT_EXPRESSION,
// 	LE_EXPRESSION,
// 	LOGICAL_AND_EXPRESSION,
// 	LOGICAL_OR_EXPRESSION,
// 	MINUS_EXPRESSION,
// 	BIT_NOT_EXPRESSION,
// 	LOGICAL_NOT_EXPRESSION,
// 	FUNCTION_CALL_EXPRESSION,
// 	MENBER_EXPRESSION,
// 	NULL_EXPRESSION,
// 	THIS_EXPRESSION,
// 	SUPER_EXPRESSION,
// 	ARRAY_LITERAL_EXPRESSION,
// 	INDEX_EXPRESSION,
// 	INCREMENT_EXPRESSION,
// 	DECREMENT_EXPRESSION,
// 	INSTANCEOF_EXPRESSION,
// 	DOWN_CAST_EXPRESSION,
// 	CASE_EXPRESSION,
// 	UP_CASE_EXPRESSION,
// 	NEW_EXPRESSION,
// 	ARRAY_CREATION_EXPRESSION,
// 	ENUMATOR_EXPRESSION,
// 	EXPRESSION_KIND_COUNT_PLUS_1

// }ExpressionKind;

// typedef struct Expression_tag Expression;
// typedef struct TypeSpecifier_tag TypeSpecifier;
// typedef struct ClassDefinition_tag ClassDefinition;
// typedef struct DelegateDefinition_tag DelegateDefinition;
// typedef struct EnumDefinition_tag EnumDefinition;
// typedef struct FunctionDefinition_tag FunctionDefinition;
// typedef struct ConstantDefinition_tag ConstantDefinition;
// typedef struct MemberDeclaration_tag MemberDeclaration;

// typedef struct Statement_tag Statement;


// typedef struct PackageName_tag{
// 	char	*name;
// 	struct PackageName_tag	*next;
// }PackageName;

// typedef struct RequireList_tag{
// 	PackageName	*package_name;
// 	int line_number;
// 	struct RequireList_tag	*next;
// }RequireList;


// typedef struct RenameList_tag{
// 	PackageName	*package_name;
// 	char	*original_name;
// 	char	*renamed_name;
// 	int	line_number;
// 	struct	RequireList_tag	*next;
// }RenameList;


// typedef struct ArgumentList_tag {
// 	Expression	*expression;
// 	struct ArgumentList_tag	*next;
// }ArgumentList;

// typedef struct ParameterList_tag{
// 	TypeSpecifier	*type;
// 	char	*name;
// 	int	line_number;
// 	struct PackageName_tag	*next;
// }ParameterList;

// typedef enum {
// 	FUNCTION_DERIVE,
// 	ARRAY_DERIVE
// }DeriveTag;


// typedef struct{
// 	char			*identifer;
// 	ClassDefinition	*class_definition;
// 	int				line_number;
// }ExceptionRef;

// typedef struct ExceptionList_tag{
// 	ExceptionRef				*exception;
// 	struct ExceptionList_tag	*next;
// }ExceptionList;

// typedef struct {
// 	ParameterList	*parameter_list;
// 	ExceptionList	*throws;
// }FunctionDerive;

// typedef struct {
// 	int dummy;
// }ArrayDerive;

// typedef struct TypeDerive_tag{
// 	DeriveTag	tag;
// 	union{
// 		FunctionDerive	function_d;
// 		ArrayDerive		array_d;
// 	}u;
// 	struct TypeDerive_tag	*next;

// }TypeDerive;





// struct TypeSpecifier_tag{
// 	DVM_BaseType	base_type;
// 	char *identifier;
// 	union{
// 		struct{
// 			ClassDefinition	*class_definition;
// 			int class_index;
// 		}class_ref;
		
// 		struct{
// 			DelegateDefinition	*delegate_definition;
// 			int	delegate_index;
// 		}delegate_ref;
		
// 		struct{
// 			EnumDefinition *enum_definition;
// 			int enum_index;
// 		}enum_ref;
// 	}u;
// 	int	line_number;
// 	TypeDerive *derive;
// };



// typedef struct {
// 	TypeSpecifier	*type;
// 	char			*name;
// 	Expression		*initializer;
// 	DVM_Boolean		*is_final;
// 	int				variable_index;
// 	DVM_Boolean		is_loacl;
// }Declaration;

// typedef struct DeclarationList_tag{
// 	Declaration	*declaration;
// 	struct DeclarationList_tag	*next;
// }DeclarationList;

// typedef struct {
// 	FunctionDefinition *function_definition;
// 	int	function_index;
// }FunctionIdentifier;



// typedef struct {
// 	ConstantDefinition	*constant_definition;
// 	int	constant_index;
// }ConstantIdentifier;

// typedef enum {
// 	VARIABLE_IDENTIFER,
// 	FUNCTION_IDENTIFER,
// 	CONSTANT_IDENTIFER
// }IdentifierKind;


// typedef struct {
// 	char	*name;
// 	IdentifierKind	kind;
// 	union{
// 		Declaration			*declaration;
// 		FunctionIdentifier	*function;
// 		ConstantIdentifier	*constant;
// 	}u;
// }IdentifierExpression;


// typedef struct {
// 	Expression	*left;
// 	Expression	*right;
// }CommaExpression;


// typedef enum {
// 	NORMAL_ASSIGN,
// 	ADD_ASSIGN,
// 	SUB_ASSIGN,
// 	MUL_ASSIGN,
// 	DIV_ASSIGN,
// 	MOD_ASSIGN
// }AssignmentOperator;


// typedef struct{
// 	AssignmentOperator	operator;
// 	Expression	*left;
// 	Expression	*operand;
// }AssignmentExpression;


// typedef struct {
// 	Expression	*left;
// 	Expression	*right;
// }BinaryExpression;


// typedef struct {
// 	Expression	*function;
// 	ArgumentList	*argument;
// }FunctionCallExpress;

// typedef struct ExpressionList_tag{
// 	Expression	*expression;
// 	struct ExceptionList_tag	*next;
// }ExpressionList;

// typedef struct {
// 	Expression	*array;
// 	Expression	*index;
// }IndexExpression;

// typedef struct {
// 	char	*member_name;
// 	Expression	*expression;
// 	MemberDeclaration	*declaration;
// 	int method_index;
// }MemberExpression;

// typedef struct {
// 	Expression	*operand;
// }IncrementOrDecrement;

// typedef struct {
// 	Expression	*expression;
// 	TypeSpecifier	*type;
// }InstanceofExpression;

// typedef struct {
// 	Expression	*operand;
// 	TypeSpecifier	*type;
// }DownCastExpression;

// typedef struct {
// 	Expression	*operand;
// 	ClassDefinition	*interface_definition;
// 	int	interface_index;
// }UpCastExpression;


// typedef enum {
// 	INT_TO_DOUBLE_CAST,
// 	DOUBLE_TO_INT_CAST,
// 	BOOLEAN_TO_STRING_CAST,
// 	INT_TO_STRING_CAST,
// 	DOUBLE_TO_STRING_CAST,
// 	ENUM_TO_STRING_CAST,
// 	FUNCTION_TO_DELEGATE_CAST
// }CastType;

// typedef struct {
// 	CastType	type;
// 	Expression	*operand;
// }CastExpression;


// typedef struct {
// 	char	*class_name;
// 	ClassDefinition	*class_definition;
// 	int		*class_index;
// 	char	*method_name;
// 	MemberDeclaration	*method_delclaration;
// 	ArgumentList	*argument;
// }NewExpression;


// typedef struct ArrayDimension_tag{
// 	Expression	*expression;
// 	struct	ArgumentList_tag	*next;
// }ArrayDimension;


// typedef struct {
// 	TypeSpecifier	*type;
// 	ArrayDimension	*dimension;
// }ArrayCreation;

// typedef struct Enumerator_tag {
// 	char	*name;
// 	int		value;
// 	struct	Enumerator_tag	*next;
// }Enumerator;


// typedef struct {
// 	EnumDefinition	*enum_definition;
// 	Enumerator		*enumerator;
// }EnumeratorExpression;


// struct Expression_tag{
// 	TypeSpecifier	*type;
// 	ExpressionKind kind;
// 	int line_number;
// 	union {
// 		DVM_Boolean				boolean_value;
// 		int						int_value;
// 		double					double_value;
// 		DVM_Char				*string_value;
// 		IdentifierExpression	*identifer_express;
// 		CommaExpression			*comma_expression;
// 		AssignmentExpression	*assignment_expression;
// 		BinaryExpression		*binary_expression;
// 		Expression				*minus_expression;
// 		Expression				*logic_not;
// 		Expression				*bit_not;
// 		FunctionCallExpress		*function_call_expression;
// 		MemberExpression		*member_expression;
// 		ExceptionList			*array_literal;
// 		IndexExpression			*index_expression;
// 		IncrementOrDecrement	*inc_dec;
// 		InstanceofExpression	*instanceof;
// 		DownCastExpression		*down_cast;
// 		CastExpression			*cast;
// 		UpCastExpression		*up_cast;
// 		NewExpression			*new_e;
// 		ArrayCreation			*array_creation;
// 		EnumeratorExpression	*enumerator;
// 	}u;
	
// };


// typedef struct StatementList_tag{
// 	Statement	*statement;
// 	struct	StatementList_tag	*next;
// }StatementList;


// typedef enum {
// 	UNDEFINED_BLOCK = 1,
// 	FUNCTION_BLOCK,
// 	WHILE_STATEMENT_BLOCK,
// 	FOR_STATEMENT_BLOCK,
// 	DO_WHILE_STATEMENT_BLOCK,
// 	TRY_CLAUSE_BLOCK,
// 	CATCH_CLAUSE_BLOCK,
// 	FINALLY_CLAUSE_BLOCK
// }BlockType;

// typedef struct {
// 	Statement	*statement;
// 	int			continue_label;
// 	int			break_label;
// }StatementBlockInfo;


// typedef struct {
// 	FunctionDefinition *function;
// 	int					end_label;
// }FunctionBlockInfo;

// typedef struct Block_tag{
// 	BlockType			*type;
// 	StatementList		*statement_list;
// 	DeclarationList		*declaration_list;
// 	struct Block_tag	*out_block;
// 	union {
// 		FunctionBlockInfo	*function;
// 		StatementBlockInfo	*statement;
// 	}parent;
	

// }Block;


// typedef struct Elsif_tag{
// 	Expression	*condition;
// 	Block	then_block;
// 	struct Elsif_tag	*next;
// }Elsif;

// typedef struct {
// 	Expression	*condition;
// 	Block		*then_block;
// 	Block		*else_block;
// 	Elsif		*elsif_list;
// }IfStatement;

// typedef struct CaseList_tag{
// 	ExpressionList		*expression_list;
// 	Block				*block;
// 	struct CaseList_tag	*next;
// }CaseList;


// typedef struct {
// 	Expression	*expression;
// 	CaseList	*case_list;
// 	Block		*default_block;

// }SwitchStatement;

// typedef struct {
// 	char		*label;
// 	Expression	*condition;
// 	Block		*block;

// }WhileStatement;

// typedef struct {
// 	char		*label;
// 	Expression	*init;
// 	Expression	*condition;
// 	Expression	*post;
// 	Block		*block;

// }ForStatement;


// typedef struct {
// 	char		*label;
// 	Block		*block;
// 	Expression	*condition;

// }DoWhileStatement;

// typedef struct {
// 	char		*label;
// 	char		*variable;
// 	Expression	*collection;
// 	Block		*block;
// }ForeachStatement;


// typedef struct {
// 	Expression	*return_value;
// }ReturnStatement;



// typedef struct {
// 	char	*label;
// }BreakStatement;

// typedef struct {
// 	char	*label;
// }ContinueStatement;

// typedef struct CatchClause_tag{
// 	char			*variable_name;
// 	Declaration		*variable_declaration;
// 	TypeSpecifier	*type;
// 	Block			*block;
// 	int				line_number;
// 	struct CatchClause_tag	*next;
	
// }CatchClause;

// typedef struct {
// 	Block		*try_block;
// 	CatchClause	*catch_clause;
// 	Block		*finally_block;
// }TryStatement;

// typedef struct {
// 	Expression	*expression;
// 	Declaration	*variable_declaration;
// }ThrowsStatement;


// typedef enum {
// 	EXPRESSION_STATEMENT = 1,
// 	IF_STATEMENT,
// 	SWITCH_STATEMENT,
// 	WHILE_STATEMENT,
// 	FOR_STATEMENT,
// 	DO_WHILE_STATEMENT,
// 	FOREACH_STATEMENT,
// 	RETURN_STATEMENT,
// 	BREAK_STATEMENT,
// 	CONTINUE_STATEMENT,
// 	TRY_STATEMENT,
// 	THROW_STATEMENT,
// 	DECLARATION_STATEMENT,
// 	STATEMENT_TYPE_CONT_PLUS_1
// }StatementType;

// struct Statement_tag {
// 	StatementType		*type;
// 	int					line_number;
// 	union {
// 		Expression		*expression_s;
// 		IfStatement		*if_s;
// 		SwitchStatement	*switch_s;
// 		WhileStatement	*while_s;
// 		ForStatement	*for_s;
// 		DoWhileStatement	*do_while_s;
// 		ForeachStatement	*foreach_s;
// 		BreakStatement		*break_s;
// 		ContinueStatement	*continue_s;
// 		ReturnStatement		*return_s;
// 		TryStatement		*try_s;
// 		ThrowsStatement		*throw_s;
// 		Declaration			*declaration_s;
// 	}u;
// };

// struct	FunctionDefinition_tag {
// 	TypeSpecifier		*type;
// 	PackageName			*package_name;
// 	char				*name;
// 	ParameterList	*parameter_list;
// 	Block				*block;
// 	int					*local_variable_count;
// 	Declaration			**local_variable;
// 	ClassDefinition		*class_definition;
// 	ExceptionList		*throws;
// 	int					end_line_number;
// 	struct FunctionDefinition_tag	*next;
// };



// typedef enum {
// 	ABSTRACT_MODIFIER,
// 	PUBLIC_MODIFIER,
// 	PRIVATE_MODIFIER,
// 	OVERRIDE_MODIFIER,
// 	VITRAL_MODIFIER,
// 	NOT_SPECIFIED_MODIFIER

// }ClassOrMemberModifierKind;


// typedef struct {
// 	ClassOrMemberModifierKind	id_abstract;
// 	ClassOrMemberModifierKind	access_modifier;
// 	ClassOrMemberModifierKind	is_virtual;
// 	ClassOrMemberModifierKind	is_override;
// }ClassOrMemberModifierList;


// typedef struct ExtendsList_tag{
// 	char	*identifer;
// 	ClassDefinition	*class_definition;
// 	struct	Expression_tag	*next;
// }ExtendsList;

// typedef enum {
// 	METHOD_MEMBER,
// 	FIELD_MEMBER
// }MemberKind;

// typedef struct {
// 	DVM_Boolean			is_constructor;
// 	DVM_Boolean			is_abstract;
// 	DVM_Boolean			is_virtual;
// 	DVM_Boolean			*is_override;
// 	FunctionDefinition	*function_definition;
// 	int					method_index;
	
// }MethodMember;


// typedef struct {
// 	char			*name;
// 	TypeSpecifier	*type;
// 	DVM_Boolean		*is_final;
// 	Expression		*initializer;
// 	int				field_index;
// }FieldMember;

// struct MemberDeclaration_tag{
// 	MemberKind		*kind;
// 	DVM_AccessModifier	access_modifier;
// 	union {
// 		FieldMember		*field;
// 		MethodMember	*method;
// 	}u;
// 	int line_number;
// 	struct MemberDeclaration_tag	*next;
// };

// struct ClassDefinition_tag {
// 	DVM_Boolean		*is_abstract;
// 	DVM_AccessModifier	access_modifier;
// 	DVM_ClassOrInterface class_or_interface;
// 	PackageName	*package_name;
// 	char		*name;
// 	ExceptionList	*extents;
// 	ClassDefinition	*super_class;
// 	ExceptionList	*interface_list;
// 	MemberDeclaration	*member;
// 	int line_number;
// 	struct	ClassDefinition_tag	*next;
// };

// typedef struct CompilerList_tag{
// 	MGC_Compiler	*compiler;
// 	struct	CompilerList_tag	*next;
// }CompilerList;


// typedef enum {
// 	MGH_SOURCE,
// 	MGM_SOURCE
// }SourceSuffix;

// typedef enum {
// 	FILE_INPUT_MODE,
// 	STRING_INPUT_MODE
// }InputMode;

// typedef struct {
// 	InputMode	*mode;
// 	union {
// 		struct {
// 			FILE	*fp;
// 		}file;
// 		struct {
// 			char	**lines;
// 		}string;
// 	}u;
// }SourceInput;


// struct DelegateDefinition_tag {
// 	char			*name;
// 	TypeSpecifier	*type;
// 	ParameterList	*parameter_list;
// 	ExceptionList	*throws;
// 	DelegateDefinition	*next;
// };

// struct EnumDefinition_tag{
// 	PackageName		*package_name;
// 	char			*name;
// 	Enumerator		*enumerator;
// 	int				index;
// 	EnumDefinition	*next;
// };

// struct ConstantDefinition_tag {
// 	PackageName			*package_name;
// 	TypeSpecifier		*type;
// 	char				*name;
// 	Expression			*initializer;
// 	int					index;
// 	int					line_number;
// 	ConstantDefinition	*next;
// };


// typedef enum {
// 	EUC_ENCODING = 1,
// 	UTF_8_ENCODING
// }Encoding;

// typedef struct {
// 	char	*string;
// }VString;

// typedef struct {
// 	DVM_Char	*string;
// }VWString;

// typedef struct {
// 	char	*package_name;
// 	SourceSuffix	suffix;
// 	char	**source_string;
// }BuiltinScript;


// struct MGC_Compiler_tag{
// 	InputMode	input_mode;
// 	int			current_line_number;
// };






// /* mango.l */
// void mgc_set_source_string(char **source);


// /* create.c */
// DeclarationList *mgc_chain_declaration(DeclarationList *list,Declaration *decl);
// Declaration	*mgc_create_declaration(DVM_Boolean is_final,TypeSpecifier type, char *identifier);
// PackageName *mgc_create_package_name(char *identifier);
// PackageName	*mgc_chain_package_name(PackageName *list, char *identifier);

// RequireList *mgc_create_require_list(PackageName *package_name);
// RequireList *mgc_chain_require_list(RequireList *list,RequireList *add);

// RenameList *mgc_create_rename_list(PackageName package_name, char *identifer);
// RenameList *mgc_chain_rename_list(RenameList *list, RenameList add);

// void mgc_set_require_and_rename_list(RequireList *require_list, RenameList *rename_list);

// FunctionDefinition *mgc_create_function_definition(TypeSpecifier *type, char *identifier,
// 												   ParameterList *parameter_list, ExceptionList *exception_list,
// 												   Block *block);
// void mgc_function_define(TypeSpecifier *type, char *identifier,
// 						 ParameterList *parameter_list, ExceptionList *exception_list,
// 						 Block *block);

// ParameterList *mgc_create_parameter(TypeSpecifier *type, char *identifier);
// ParameterList *mgc_chain_parameter(ParameterList *list, TypeSpecifier *type, char *identifier);

// ArgumentList *mgc_create_argument(Expression *expression);
// ArgumentList *mgc_chain_argument(ArgumentList *list,Expression *expression);

// ExpressionList *mgc_create_expression_list(Expression *expression);
// ExpressionList *mgc_chain_expression_list(ExpressionList *list, Expression *expression);


// StatementList *mgc_create_statement_list(Statement *statement);
// StatementList *mgc_chain_statement_list(StatementList *statement_list, Statement *statement);



// TypeSpecifier *mgc_create_type_specifier(DVM_BaseType base_type);
// TypeSpecifier *mgc_create_identifier_type_specifier(char *identifier);
// TypeSpecifier *mgc_create_array_type_specifier(TypeSpecifier *base);


// Expression *mgc_alloc_expression(ExpressionKind kind);
// Expression *mgc_create_comma_expression(Expression *left, Expression *right);
// Expression *mgc_create_assign_expression(Expression *left, AssignmentOperator operator, Expression operand);
// Expression *mgc_create_binary_expression(ExpressionKind operator, Expression *left, Expression *right);
// Expression *mgc_create_minus_expression(Expression *operand);
// Expression *mgc_create_logic_not_expression(Expression *operand);
// Expression *mgc_create_bit_not_expression(Expression *operand);
// Expression *mgc_create_index_expression(Expression *array,Expression *index);
// Expression *mgc_create_incdec_expression(Expression *operand,ExpressionKind inc_or_dec);
// Expression *mgc_create_instanceof_expression(Expression *operand, TypeSpecifier *type);
// Expression *mgc_create_identifier_expression(char *identifier);
// Expression *mgc_create_function_call_expression(Expression *function, ArgumentList *argument);
// Expression *mgc_create_down_cast_expression(Expression *operand,TypeSpecifier *type);
// Expression *mgc_create_member_expression(Expression *expression, char *member_name);
// Expression *mgc_create_boolean_expression(DVM_Boolean value);
// Expression *mgc_create_null_expression(void);
// Expression *mgc_create_new_expression(char *class_name, char *method_name, ArgumentList *argument);
// Expression *mgc_create_array_literal_expression(ExpressionList *list);
// Expression *mgc_create_base_array_creation(DVM_BaseType base_type,
// 										   ArrayDimension *dim_expr_list,
// 										   ArrayDimension *dim_list);
// Expression *mgc_create_class_array_creation(TypeSpecifier *type,
// 											ArrayDimension *dim_expr_list,
// 											ArrayDimension *dim_list);
// Expression *mgc_create_this_expression(void);
// Expression *mgc_create_super_expression(void);

// ArrayDimension *mgc_create_array_dimension(Expression *expression);
// ArrayDimension *mgc_chain_array_dimension(ArrayDimension *list,
// 										  ArrayDimension *dim);

// Statement *mgc_alloc_statement(StatementType type);
// Statement *mgc_create_if_statement(Expression *condition,
// 								   Block *then_block, Elsif *elsif_list,
// 								   Block *else_block);

// Elsif *mgc_create_elsif_statement(Expression *expr, Block *block);
// Elsif *mgc_chain_elsif_statement(Elsif *list, Elsif *add);

// Statement *mgc_create_switch_statement(Expression *expr,
// 									   CaseList *case_list,
// 									   Block *default_block);

// CaseList *mgc_create_one_case(ExpressionList *expression_list, Block *block);
// CaseList *mgc_chain_case(CaseList *list, CaseList *add);

// Statement *mgc_create_while_statement(char *label,
// 									  Expression *condition,
// 									  Block *block);

// Statement *mgc_create_foreach_statement(char *label,
// 										char *variable,
// 										Expression *collection,
// 										Block *block);

// Statement *mgc_create_for_statement(char *label,
// 									Expression *init,
// 									Expression *condition,
// 									Expression *post,
// 									Block *block);

// Statement *mgc_create_do_while_statement(char *label,
// 										 Block *block,
// 										 Expression *condition);


// Block *mgc_alloc_block(void);
// Block *mgc_open_block(void);
// Block *mgc_close_block(Block *block, StatementList *statement_list);

// Statement *mgc_create_expression_statement(Expression *expression);
// Statement *mgc_create_return_statement(Expression *expression);
// Statement *mgc_create_break_statement(char *label);
// Statement *mgc_create_continue_statement(char *label);
// Statement *mgc_create_try_statement(Block *try_block,
// 									CatchClause *catch_clause,
// 									Block *finally_block);

// CatchClause *mgc_create_catch_clause(TypeSpecifier *type,
// 									 char *variable_name,
// 									 Block *block);
// CatchClause *mgc_start_catch_clause(void);
// CatchClause *mgc_end_catch_clause(CatchClause *catch_clause,
// 								  TypeSpecifier *type,
// 								  char *variable_name,
// 								  Block *block);
// CatchClause *mgc_chain_catch_clause(CatchClause *list, CatchClause *add);


// Statement *mgc_create_throw_statement(Expression *expression);
// Statement *mgc_create_declaration_statement(DVM_Boolean is_final,
// 											TypeSpecifier *type,
// 											char *identifier,
// 											Expression *initializer);

// void mgc_start_class_definition(ClassOrMemberModifierList *modifier,
// 								DVM_ClassOrInterface class_or_interface,
// 								char *identifier,
// 								ExpressionList *extends);

// void mgc_class_define(MemberDeclaration *member_list);
// ExtendsList *mgc_create_extends_list(char *identifier);
// ExtendsList *mgc_chain_extends_list(ExtendsList *list, char *add);

// ClassOrMemberModifierList mgc_create_class_or_member_modifier_list(ClassOrMemberModifierKind kind);
// ClassOrMemberModifierList mgc_chain_class_or_member_modifier_list(ClassOrMemberModifierList list,
// 																  ClassOrMemberModifierList add);

// MemberDeclaration *mgc_chain_member_declaration_list(MemberDeclaration *list,
// 													 MemberDeclaration *add);

// MemberDeclaration *mgc_create_method_member(ClassOrMemberModifierList *modifier,
// 											FunctionDefinition *function_definition,
// 											DVM_Boolean is_final);
// FunctionDefinition *mgc_method_function_definition(TypeSpecifier *type,
// 												   char *identifier,
// 												   ParameterList *parameter,
// 												   ExceptionList *thorws,
// 												   Block *block);
// FunctionDefinition *mgc_constructor_function_definition(char *identifier,
// 														ParameterList *parameter,
// 														ExceptionList *throws,
// 														Block *block);

// MemberDeclaration *mgc_create_field_member(ClassOrMemberModifierList *modifier,
// 										   DVM_Boolean *is_final,
// 										   TypeSpecifier *type,
// 										   char *name,
// 										   Expression *initializer);

// ExceptionList *mgc_create_thorws(char *identifer);
// ExceptionList *mgc_chain_exception_list(ExceptionList *list, char *identifier);

// void mgc_create_delegate_definition(TypeSpecifier *type, char identifier,
// 									ParameterList *parameter, ExceptionList *thorws);


// void mgc_create_enum_definition(char *identifier, Enumerator *enumerator);
// Enumerator *mgc_create_enumerator(char *identifier);
// Enumerator *mgc_chain_enumerator(Enumerator *enumerator, char *identifier);

// void *mgc_create_cons_definition(TypeSpecifier *type,
// 								 char *identifier,
// 								 Expression *initializer);




// void mgc_compile_error(int line_number,CompileError err,...);


// /* string.c */
// void mgc_open_string_literal(void);
// void mgc_add_string_literal(int letter);
// void mgc_rest_string_literal_buffer(void);
// DVM_Char *mgc_close_string_literal(void);
// int mgc_close_character_literal(void);
// char *mgc_create_identifier(char *str);


// /* util.c */
// MGC_Compiler *mgc_get_current_compiler(void);

// typedef enum {
// 	DVM_VOID_TYPE,
// 	DVM_BOOLEAN_TYPE,
// 	DVM_INT_TYPE,
// 	DVM_DOUBLE_TYPE,
// 	DVM_STRING_TYPE,
// 	DVM_CLASS_TYPE,
// 	DVM_DELEGAET_TYPE,
// 	DVM_ENUM_TYPE,
// 	DVM_NULL_TYPE,
// 	DVM_NATIVE_POINTER_TYPE,
// 	DVM_BASE_TYPE,
// 	DVM_UNSPECIFIED_IDENTIFIER_TYPE
// }DVM_BaseType;


%}

%union{
    char                *identifier;
    PackageName         *package_name;
    RequireList         *require_list;
    RequireList         *rename_list;
    ParameterList       *parameter_list;
    ArgumentList        *argument_list;
    Expression          *expression;
    ExpressionList      *expression_list;
    Statement           *statement;
    StatementList       *statement_list;
    Block               *block;
    Elsif               *elsif;
    CaseList            *case_list;
    CatchClause         *catch_clause;
    AssignmentOperator  *assignment_operator;
    TypeSpecifier       *type_specifier;
    DVM_BaseType        *base_type_specifier;
    ArrayDimension      *array_dimension;
    ClassOrMemberModifierList   class_or_member_modifier;
    DVM_ClassOrInterface        calss_or_interface;
    ExtendsList         *extends_list;
    MemberDeclaration   *member_declaration;
    FunctionDefinition  *function_definition;
    ExceptionList       *exception_list;
    Enumerator          *enumerator;
}

%token <expression> INT_LITERAL
%token <expression> DOUBLE_LITERAL
%token <expression> STRING_LITERAL
%token <expression> REGEXP_LITERAL
%token <identifier> IDENTIFER
%token IF ELSE ELSIF SWITHC CASE DEFAULT_T WHILE DO_T FOR FOREACH
        RETURN_T BREAK CONTINUE NULL_T 
        LP RP LC RC LB RB SEMICOLON COLON COMMA  ASSIGN_T LOGICAL_AND LOGICAL_OR
        EQ NE GT GE LT LE ADD SUB MUL DIV MOD BIT_AND BIT_OR BIT_XOR BIT_NOT
        TRUE_T FALSE_T EXCLAMATION  DOT
        ADD_ASSIGN_T SUB_ASSIGN_T MUL_ASSIGN_T DIV_ASSIGN_T MOD_ASSIGN_T
        INCREMENT DECREMENT TRY CATCH FINALLY THROW THROWS 
        VOID_T BOOLEAN_T INT_T DOUBLE_T STRING_T NATIVE_POINTER_T
        NEW REQUIRE RENAME
        CLASS_T INTERFACE_T PUBLIC_T PRIVATE_T VIRTUAL_T OVERRIED_T
        ABSTRACT_T THIS_T SUPER_T CONSTRUCTOR INSTANCEOF
        DOWN_CAST_BEGIN DOWN_CAST_END DELEGATE FINAL ENUM CONST


%type <package_name> package_name
%type <require_list> require_list require_declaration
%type <rename_list> rename_list rename_declaration
%type <parameter_list> parameter_list
%type <argument_list> argument_list
%type <expression> expression expression_opt
        assignment_expression logical_and_expression logical_or_expression
        equality_expression relational_expression
        additive_expression multiplication_expression
        unary_expression postfix_expression primary_expression
        primary_no_new_array array_literal array_creation
        initializer_opt
%type <expression_list> expression_list case_expression_list
%type <statement> statement
        if_statement swithc_statement
        while_statement for_statement do_while_statement foreach_statement
        return_statement break_statement continue_statement try_statement
        throw_statement  declaration_statement
%type <statement_list> statement_list
%type <block> block default_opt
%type <elsif> elsif elsif_list
%type <case_list> case_list one_case
%type <catch_clause> catch_clause catch_list
%type <assignment_operator> assignment_operator
%type <identifier> identifier_opt label_opt
%type <type_specifier> type_specifier identifier_type_specifier array_type_specifier
%type <base_type> base_type_specifier
%type <array_dimension> dimension_expression dimension_expression_list dimension_list
%type <class_or_member_modifier> class_or_member_modifier class_or_member_modifier_list access_modifier
%type <class_or_interface> class_or_interface
%type <extends_list> extends_list extends
%type <member_declaration> member_declaration member_declaration_list
        method_member field_member
%type <function_definition> method_function_definition constructor_function_definition 
%type <exception_list> exception_list throws_clause
%type <enumerator> enumerator_list


%%

translation_util: initial_declaration definition_or_statement
                | translation_util definition_or_statement
                ;


initial_declaration: /* empty */
                {
                        mgc_set_require_and_rename_list(NULL,NULL);
                }
                | require_list rename_list
                {
                        mgc_set_require_and_rename_list($1,$2);
                }
                | require_list
                {
                        mgc_set_require_and_rename_list($1,NULL);
                }
                | rename_list
                {
                        mgc_set_require_and_rename_list(NULL,$1);
                }
                ;

require_list: require_declaration
                | require_list require_declaration
                {
                        $$ = mgc_chain_require_list($1,$2);
                }
                ;

require_declaration: REQUIRE package_name SEMICOLON
                {
                        $$ = mgc_create_require_list($2);
                }
                ;

package_name: IDENTIFER
                {
                        $$ = mgc_create_package_name($1);
                }
                | package_name DOT IDENTIFER
                {
                        $$ = mgc_chain_package_name($1,$2);
                }
                ;

rename_list: rename_declaration
                | rename_list rename_declaration
                {
                        $$ = mgc_chain_rename_list($1,$2);
                }
                ;

rename_declaration: RENAME package_name IDENTIFER SEMICOLON
                {
                        $$ = mgc_create_rename_list($1);
                }
                ;


definition_or_statement: function_definition
                | class_definition
                | statement
                {
                        MGC_Compiler *compiler = mgc_get_current_compiler();
                        compiler->statement_list = mgc_chain_statement_list(compiler->statement_list,$1);   
                }
                | delegate_definition
                | enum_definition
                | constant_definition
                ;


base_type_specifier: VOID_T
                {
                       $$ = DVM_VOID_TYPE;
                }
                | BOOLEAN_T
                {
                        $$ = DVM_BOOLEAN_TYPE;
                }
                | INT_T
                {
                        $$ = DVM_INT_TYPE;
                }
                | DOUBLE_T
                {
                        $$ = DVM_DOUBLE_TYPE;
                }
                | STRING_T
                {
                        $$ = DVM_STRING_TYPE;
                }
                | NATIVE_POINTER_T
                {
                        $$ = DVM_NATIVE_POINTER_TYPE;
                }
                ;

identifier_type_specifier: IDENTIFER
                {
                        $$ = mgc_create_identifier_type_specifier($1);
                }
                ;

array_type_specifier: 
                base_type_specifier LB RB
                {
                        TypeSpecifier *base_type = mgc_create_type_specifier($1);
                        $$ = mgc_create_array_type_specifier(base_type);
                }
                | IDENTIFER LB RB
                {
                        TypeSpecifier *identifier_type = mgc_create_identifier_type_specifier($1);
                        $$ = mgc_create_array_type_specifier(identifier_type);
                }
                array_type_specifier LB RB
                {
                        $$ = mgc_create_array_type_specifier($1);
                }
                ;

type_specifier: base_type_specifier
                {
                        $$ = mgc_create_type_specifier($1);
                }
                | identifier_type_specifier
                | array_type_specifier
                ;

function_definition: type_specifier IDENTIFER LP parameter_list RP throws_clause block
                {
                        mgc_function_define($1,$2,$3,$4,$5);
                }
                | type_specifier IDENTIFER LP  RP throws_clause block
                {
                        mgc_function_define($1,$2,NULL,$4,$5);
                }
                | type_specifier IDENTIFER LP parameter_list RP throws_clause SEMICOLON
                {
                        mgc_function_define($1,$2,$3,$4,NULL);
                }
                | type_specifier IDENTIFER LP  RP throws_clause SEMICOLON
                {
                        mgc_function_define($1,$2,NULL,$4,NULL);
                }
                ;

parameter_list: type_specifier IDENTIFER
                {
                        $$ = mgc_create_parameter($1,$2);
                }
                | parameter_list COMMA type_specifier IDENTIFER
                {
                        $$ = mgc_chain_parameter($1,$3,$4);
                }
                ;

argument_list: assignment_expression
                {
                        $$ = mgc_create_argument($1);
                }
                | argument_list COMMA assignment_expression
                {
                        $$ = mgc_chain_argument($1,$3);
                }
                ;
        
statement_list: statement
                {
                        $$ = mgc_create_statement_list($1);
                }
                | statement_list statement
                {
                        $$ = mgc_chain_statement_list($1,$2);
                }
                ;

expression: assignment_expression
                | exception COMMA assignment_expression
                {
                        $$ = mgc_create_comma_expression($1,$2);
                }
                ;

assignment_expression: logical_or_expression
                | primary_expression assignment_operator assignment_expression
                {
                        $$ = mgc_create_assign_expression($1,$2,$3);
                }
                ;

assignment_operator: ASSIGN_T
                {
                        $$ = NORMAL_ASSIGN;
                }
                | ADD_ASSIGN_T
                {
                        $$ = ADD_ASSIGN;
                }
                | SUB_ASSIGN_T
                {
                        $$ = SUB_ASSIGN;
                }
                | MUL_ASSIGN_T
                {
                        $$ = MUL_ASSIGN;
                }
                | DIV_ASSIGN_T
                {
                        $$ = DIV_ASSIGN;
                }
                | MOD_ASSIGN_T
                {
                        $$ = MOD_ASSIGN;
                }
                ;

logical_or_expression: logical_and_expression
                | logical_or_expression LOGICAL_OR logical_and_expression
                {
                        $$ = mgc_create_binary_expression(LOGICAL_OR_EXPRESSION,$1,$3);    
                }
                ;

logical_and_expression: equality_expression
                | logical_and_expression LOGICAL_AND equality_expression
                {
                        $$ = mgc_create_binary_expression(LOGICAL_AND_EXPRESSION,$1,$3); 
                }
                ;

equality_expression: relational_expression
                | equality_expression EQ relational_expression
                {
                        $$ = mgc_create_binary_expression(EQ_EXPRESSION,$1,$3);
                }
                | equality_expression NQ relational_expression
                {
                        $$ = mgc_create_binary_expression(NE_EXPRESSION,$1,$3);
                }
                ;
        
relational_expression: additive_expression
                | relational_expression GT additive_expression
                {
                        $$ = mgc_create_binary_expression(GT_EXPRESSION,$1,$3);
                }
                | relational_expression GE additive_expression
                {
                        $$ = mgc_create_binary_expression(GE_EXPRESSION,$1,$3);
                }
                | relational_expression LT additive_expression
                {
                        $$ = mgc_create_binary_expression(LT_EXPRESSION,$1,$3);
                }
                | relational_expression LE additive_expression
                {
                        $$ = mgc_create_binary_expression(LE_EXPRESSION,$1,$3);
                }
                ;

additive_expression: multiplication_expression
                | additive_expression ADD multiplication_expression
                {
                        $$ = mgc_create_binary_expression(ADD_EXPRESSION,$1,$3);
                }
                | additive_expression SUB multiplication_expression
                {
                        $$ = mgc_create_binary_expression(SUB_EXPRESSION,$1,$3);
                }
                ;

multiplication_expression: unary_expression
                | multiplication_expression MUL unary_expression
                {
                        $$ = mgc_create_binary_expression(MUL_EXPRESSION,$1,$3);
                }
                | multiplication_expression DIV unary_expression
                {
                        $$ = mgc_create_binary_expression(DIV_EXPRESSION,$1,$3);
                }
                | multiplication_expression MOD unary_expression
                {
                        $$ = mgc_create_binary_expression(MOD_EXPRESSION,$1,$3);
                }
                | multiplication_expression BIT_AND unary_expression{
                        $$ = mgc_create_binary_expression(BIT_ADD_EXPRESSION,$1,$3);
                }
                | multiplication_expression BIT_OR unary_expression
                {
                        $$ = mgc_create_binary_expression(BIT_OR_EXPRESSION,$1,$3);
                }
                | multiplication_expression BIT_XOR unary_expression
                {
                        $$ = mgc_create_binary_expression(BIT_XOR_EXPRESSION,$1,$3);
                }
                ;

unary_expression: postfix_expression
                | SUB unary_expression
                {
                        $$ = mgc_create_minus_expression($2);
                }
                | EXCLAMATION unary_expression
                {
                        $$ = mgc_create_logic_not_expression($2);
                }
                | BIT_NOT unary_expression
                {
                        $$ = mgc_create_bit_not_expression($2);
                }
                ;

postfix_expression: primary_expression
                |  primary_expression INCREMENT
                {
                        $$ = mgc_create_incdec_expression($1,INCREMENT_EXPRESSION);
                }      
                | primary_expression DECREMENT
                {
                        $$ = mgc_create_incdec_expression($1,DECREMENT_EXPRESSION);
                }
                | primary_expression INSTANCEOF type_specifier
                {
                        $$ = mgc_create_instanceof_expression($1,$3);
                }
                ;


primary_expression: primary_no_new_array
                | array_creation
                | IDEN.TIFER
                {
                        $$ = mgc_create_identifier_expression($1);
                }
                ;

primary_no_new_array: primary_no_new_array LB expression RB
                {
                        $$ = mgc_create_index_expression($1,$3);
                }
                | IDENTIFER LB exception RB
                {
                        Expression *identifer = mgc_create_identifier_expression($1);
                        $$ = mgc_create_index_expression(identifer, $3);
                }
                | primary_expression DOT IDENTIFER
                {
                        $$ = mgc_create_member_expression($1, $3);
                }
                | primary_expression LP argument_list RP
                {
                        $$ = mgc_create_function_call_expression($1,$3);
                }
                | primary_expression LP RP
                {
                        $$ = mgc_create_function_call_expression($1,NULL);  
                }
                | LP exception RP
                {
                        $$ = $2;
                }
                | primary_expression DOWN_CAST_BEGIN type_specifier DOWN_CAST_END
                {
                        $$ = mgc_create_down_cast_expression($1,$3);
                }
                | INT_LITERAL
                | DOUBLE_LITERAL
                | STRING_LITERAL
                | REGEXP_LITERAL
                | TRUE_T
                {
                        $$ = mgc_create_boolean_expression($1);
                }
                | FALSE_T{
                        $$ = mgc_create_boolean_expression($1);
                }
                | NULL_T
                {
                        $$ = mgc_create_null_expression();
                }
                | array_literal
                | THIS_T {
                        $$ = mgc_create_this_expression();
                }
                | SUPER_T
                {
                        $$ = mgc_create_super_expression();
                }
                | NEW IDENTIFER LP RP
                {
                        $$ = mgc_create_new_expression($2, NULL, NULL);
                }
                | NEW IDENTIFER LP argument_list RP
                {
                        $$  = mgc_create_new_expression($2, NULL, $4);
                }
                | NEW IDENTIFER DOT IDENTIFER LP  RP
                {
                        $$  = mgc_create_new_expression($2, $4, NULL);
                }
                | NEW IDENTIFER DOT IDENTIFER LP argument_list RP
                {
                         $$  = mgc_create_new_expression($2, $4, $6);
                }
                ;

array_literal: LC exception_list RC
                {
                        $$ = mgc_create_array_literal_expression($2);
                }
                | LC exception_list COMMA RC
                {
                        $$ = mgc_create_array_literal_expression($2);
                }
                ;

array_creation: NEW base_type_specifier dimension_expression_list
                {
                        $$ = mgc_create_base_array_creation($2, $3, NULL);
                }
                | NEW base_type_specifier dimension_expression_list dimension_list
                {
                        $$ = mgc_create_base_array_creation($2, $3, $4);
                }
                | NEW identifier_type_specifier dimension_expression_list
                {
                        $$ = mgc_create_class_array_creation($2, $3, NULL);
                }
                | NEW identifier_type_specifier dimension_expression_list  dimension_list
                {
                        $$ = mgc_create_class_array_creation($2, $3, $4);
                }
                ;

dimension_expression_list: dimension_expression
                | dimension_expression_list dimension_list
                {
                        $$ = mgc_chain_array_dimension($1,$2);
                }
                ;

dimension_expression: LB exception RB
                {
                        $$ = mgc_create_array_dimension($2);
                }
                ;

dimension_list: LB RB
                {
                        $$ = mgc_create_array_dimension(NULL);
                }
                | dimension_list LB RB
                {
                        $$ = mgc_chain_array_dimension($1,mgc_create_array_dimension(NULL));
                }
                ;

expression_list: /* empty */
                {
                        $$ = NULL;
                }
                | assignment_expression
                {
                        $$ = mgc_create_expression_list($1);
                }
                | expression_list assignment_expression
                {
                        $$ = mgc_chain_expression_list($1,$2);
                }
                ;

statement: expression SEMICOLON
                {
                        $$ = mgc_create_expression_statement()$1;
                }
                | if_statement
                | swithc_statement
                | while_statement
                | for_statement
                | do_while_statement
                | foreach_statement
                | return_statement
                | break_statement
                | continue_statement
                | try_statement
                | throw_statement
                | declaration_statement
                ;

if_statement: IF LP exception RP block
                {
                        $$ = mgc_create_if_statement($3, $5, NULL, NULL);
                }
                | IF LP exception RP block ELSE block
                {
                        $$ = mgc_create_if_statement($3, $5, $7);
                }
                | IF LP exception RP block elsif_list
                {
                        $$ = mgc_create_if_statement($3, $5, $6);
                }
                | IF LP exception RP block elsif_list ELSE block
                {
                        $$ = mgc_create_if_statement($3, $5, $6, $8);
                }
                ;

elsif_list: elsif
                | elsif_list elsif
                {
                        $$ = mgc_chain_elsif_statement($1,$2);
                }
                ;

elsif: ELSIF LP exception RP block
                {
                        $$ = mgc_create_elsif_statement($3, $5);
                }

label_opt:  /* empty */
                {
                        $$ = NULL;
                }
                | IDENTIFER COLON
                {
                        $$ = $1;
                }
                ;

swithc_statement: SWITHC LP exception RP case_list default_opt
                {
                        $$ = mgc_create_switch_statement($3, $5, $6);
                }
                ;


case_list: one_case
                | case_list one_case
                {
                        $$ = mgc_chain_case($1,$2);
                }
                ;

one_case: CASE case_expression_list block
                {
                        $$ = mgc_create_one_case($2,$3);
                }
                ;


default_opt: /* empty */
                {
                        $$ = NULL;
                }
                | DEFAULT_T block
                {
                        $$ = $2;
                }
                ;

case_expression_list: assignment_expression 
                {
                        $$ = mgc_create_expression_list($1);
                }
                | case_expression_list COMMA assignment_expression
                {
                        $$ = mgc_chain_expression_list($1,$3);
                }
                ;

                

while_statement: label_opt WHILE LP exception RP block
                {
                        $$ = mgc_create_while_statement($1, $4, $6);
                }
                ;


for_statement: label FOR LP expression_opt SEMICOLON expression_opt SEMICOLON expression_opt block
                {
                        $$ = mgc_create_for_statement($1, $4, $6, $8, $9);
                }
                ;





%%


