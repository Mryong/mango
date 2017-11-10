%{
	#define YYDEBUG 1
	#define YYERROR_VERBOSE
	#include <Foundation/Foundation.h>

int yyerror(char const *str);
int yylex(void);

%}

%union{
	char	*identifier;
	void*   expression;
	void*   statement;
	void*   statement_list;
	void*   dic_entry;
	void* 	dic_entry_list;
}

%token <identifier> IDENTIFIER

%token <expression> DOUBLE_LITERAL
%token <expression> STRING_LITERAL
%token <expression> INTETER_LITERAL
%token <expression> SELF_T
%token <expression> SUPER_T

%token COLON SEMICOLON COMMA  LA RA LP RP LB RB LC RC  QUESTION DOT ASSIGN AT POWER
	AND OR NOT EQ NE LT LE GT GE MUL DIV MOD INCREMENT DECREMENT
	RETURN_T 
	CLASS STRUCT DECLARE
	PROPERTY WEAK STRONG COPY ASSIGN_MEM NONATOMIC ATOMIC MINUS PLUS ADDRESS ASTERISK
	NS_INTEGER NS_U_INTEGER  CG_FLOAT  DOUBLE NS_STRING NS_NUMBER NS_ARRAY NS_MUTABLE_ARRAY NS_DICTIONARY NS_MUTABLE_DICTIONARY ID
	CG_RECT CG_SIZE CG_POINT CG_AFFINE_TRANSFORM NS_RANGE

%type <expression> expression assign_expression ternary_operator_expression logic_or_expression logic_and_expression  
equality_expression relational_expression additive_expression multiplication_expression unary_expression postfix_expression
primary_expression 

%type <dic_entry> dic_entry
%type <dic_entry_list> dic_entry_list
%type <statement> statement return_statement declaration_statement
%type <statement_list> statement_list


%%

translation_util: definition
			| translation_util definition
			;

definition: /*empty*/
			| class_definition
			| declare_struct
			;

struct_name: IDENTIFIER
			| struct_type_specifier
			;

declare_struct: DECLARE STRUCT struct_name LC
				IDENTIFIER COLON IDENTIFIER COMMA
				IDENTIFIER COLON identifier_list   
				RC
			;

identifier_list: IDENTIFIER
			| identifier_list COMMA IDENTIFIER
			;




class_definition: CLASS IDENTIFIER COLON IDENTIFIER LC RC
			| CLASS IDENTIFIER COLON IDENTIFIER LC member_definition_list RC
			| CLASS IDENTIFIER COLON IDENTIFIER LA protocol_list RA LC RC
			| CLASS IDENTIFIER COLON IDENTIFIER LA protocol_list RA LC member_definition_list RC
			;

protocol_list: IDENTIFIER
			| protocol_list COMMA IDENTIFIER
			;


property_definition: PROPERTY LP property_modifier_list RP type_specifier IDENTIFIER  SEMICOLON
			| PROPERTY LP property_modifier_list RP block_var_declaration SEMICOLON
			| PROPERTY LP  RP type_specifier IDENTIFIER SEMICOLON
			;



property_modifier_list: property_modifier
			| property_modifier_list COMMA property_modifier
			;


property_modifier: property_rc_modifier
				| property_atomic_modifier
			;

property_rc_modifier: WEAK
			| STRONG
			| COPY
			| ASSIGN_MEM
			;

property_atomic_modifier: NONATOMIC
			| ATOMIC
			;


type_specifier: non_block_type_specifier
			| block_type_specifier
			;

type_specifier_list: type_specifier
			: type_specifier_list type_specifier
			;



non_block_type_specifier: base_type_specifier
			| struct_type_specifier
			| oc_type_specifier
			| custom_type_specifier
			;



block_type_specifier: type_specifier LP POWER  RP LP type_specifier_list RP
			;


base_type_specifier: NS_INTEGER
			| NS_U_INTEGER
			| CG_FLOAT
			| DOUBLE
			;

struct_type_specifier: CG_RECT 
			| CG_SIZE 
			| CG_POINT 
			| CG_AFFINE_TRANSFORM 
			| NS_RANGE
			;

oc_type_specifier: NS_STRING ASTERISK
			| NS_NUMBER ASTERISK
			| NS_ARRAY ASTERISK
			| NS_MUTABLE_ARRAY ASTERISK
			| NS_DICTIONARY ASTERISK
			| NS_MUTABLE_DICTIONARY ASTERISK
			| ID
			;


custom_type_specifier: IDENTIFIER
				| IDENTIFIER asterisk_list
			;

asterisk_list: ASTERISK
			| asterisk_list ASTERISK
			;

method_definition: instance_method_definition
			| class_method_definition
			;

instance_method_definition: MINUS LP type_specifier RP method_name LC  RC
			| MINUS LP type_specifier RP method_name LC statement_list  RC 
			;

class_method_definition: PLUS LP type_specifier RP method_name  LC  RC
			| PLUS LP type_specifier RP method_name  LC statement_list  RC
			;	

method_name: method_name_1
			| method_name_2
			;		

method_name_1: IDENTIFIER
			;

method_name_2: method_name_item
			| method_name_2 method_name_item
			;

method_name_item: IDENTIFIER COLON LP type_specifier RP IDENTIFIER 
		;

member_definition: property_definition
			| method_definition
			;
		
member_definition_list: member_definition
			| member_definition_list member_definition
			;

selector: selector_1
			| selector_2
			;

selector_1: IDENTIFIER
			;

selector_2: IDENTIFIER COLON
			| selector_2 IDENTIFIER COLON
			;

expression: assign_expression
			| expression COMMA assign_expression
			;
	
assign_expression:  ternary_operator_expression
			| primary_expression ASSIGN ternary_operator_expression
			;

ternary_operator_expression: logic_or_expression
 			| logic_or_expression  QUESTION ternary_operator_expression  COLON ternary_operator_expression
			| logic_or_expression  QUESTION COLON ternary_operator_expression
			;

logic_or_expression: logic_and_expression
			| logic_or_expression OR logic_and_expression
			;

logic_and_expression: equality_expression
			| logic_and_expression AND equality_expression
			;

equality_expression: relational_expression
			| equality_expression EQ relational_expression
			| equality_expression NE relational_expression
			;

relational_expression: additive_expression
			| relational_expression LT additive_expression
			| relational_expression LE additive_expression
			| relational_expression GT additive_expression
			| relational_expression GE additive_expression
			;

additive_expression: multiplication_expression
			| additive_expression PLUS multiplication_expression
			| additive_expression MINUS multiplication_expression
			;

multiplication_expression: unary_expression
			| multiplication_expression ASTERISK unary_expression
			| multiplication_expression DIV unary_expression
			| multiplication_expression MOD unary_expression
			;

unary_expression: postfix_expression
			| NOT unary_expression
			| MINUS unary_expression
			;

postfix_expression: primary_expression
			| primary_expression INCREMENT
			| primary_expression DECREMENT
			;

expression_list: assign_expression
			| expression_list COMMA assign_expression
			;

dic_entry: primary_expression COLON primary_expression {
				return NULL;
			}
			;

dic_entry_list: dic_entry {
				return NULL;
			} 
			| dic_entry_list COMMA dic_entry {
				return NULL;
			}
			;

dic: AT LC  dic_entry_list RC {
				return NULL;
			}
			| AT LC  RC { 
				return NULL;
			}
			;

primary_expression: IDENTIFIER
			| primary_expression DOT INCREMENT
			| primary_expression DOT selector LP RP
			| primary_expression DOT selector LP expression_list RP
			| IDENTIFIER LP RP
			| IDENTIFIER LP expression_list RP
			| INTETER_LITERAL
			| DOUBLE_LITERAL
			| STRING_LITERAL
			| AT INTETER_LITERAL
			| AT DOUBLE_LITERAL
			| AT STRING_LITERAL
			| SELF_T
			| SUPER_T
			| AT LP expression RP
			| AT LB expression_list RB
			| AT LB  RB
			| dic
			;

block_var_declaration: type_specifier LP POWER IDENTIFIER RP LP type_specifier_list RP
			| type_specifier LP POWER IDENTIFIER RP LP  RP
			;

block_body: POWER type_specifier LP  RP LC RC COMMA
			| POWER type_specifier LP function_param_list RP LC RC COMMA
			| POWER type_specifier LP  RP LC statement_list RC COMMA
			| POWER type_specifier LP function_param_list RP LC statement_list RC COMMA
			;

function_param_list: function_param
			| function_param_list COMMA function_param
			;

function_param: non_block_type_specifier IDENTIFIER
			| block_var_declaration
			;


declaration_statement: block_var_declaration SEMICOLON {
				return NULL;
			}
			| block_var_declaration ASSIGN block_body SEMICOLON {
				return NULL;
			}
			;

return_statement: RETURN_T COMMA {
				return NULL;
			}
			| RETURN_T expression COMMA {
				return NULL;
			}
			;

statement: expression SEMICOLON {
				return NULL;
			}
			| declaration_statement
			| return_statement
			;

statement_list: statement {
				return NULL;
			}
			| statement_list statement {
				return NULL;
			}
			;




	




%%
