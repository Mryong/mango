%{
	#define YYDEBUG 1
	#define YYERROR_VERBOSE
	#include <Foundation/Foundation.h>

int yyerror(char const *str);
int yylex(void);

%}

%union{
	char	*identifier;
	void*	identifier_list;
	void*   expression;
	void*   statement;
	void*   statement_list;
	void*   block
	void*   dic_entry;
	void* 	dic_entry_list;
	void*   type_specifier;
	void* 	type_specifier_list;
	void*   one_case;
	void*   case_list;
	void	*else_if;
	void	*else_if_list;
	void	*property_modifier_list;
	void	*property_definition_key;
	void	*class_definition;
	void	*declare_struct;
	void	*member_definition;



}

%token <identifier> IDENTIFIER label_opt identifier_opt
%token <expression> DOUBLE_LITERAL
%token <expression> STRING_LITERAL
%token <expression> INTETER_LITERAL
%token <expression> SELF
%token <expression> SUPER
%token <expression> NIL
%token <expression> YES_
%token <expression> NO_

%token COLON SEMICOLON COMMA  LA RA LP RP LB RB LC RC  QUESTION DOT ASSIGN AT POWER
	AND OR NOT EQ NE LT LE GT GE MUL DIV MOD INCREMENT DECREMENT
	CLASS STRUCT DECLARE
	YES_ NO_ NIL 
	RETURN IF ELSE FOR WHILE DO SWITCH CASE DEFAULT BREAK CONTINUE 
	PROPERTY WEAK STRONG COPY ASSIGN_MEM NONATOMIC ATOMIC MINUS PLUS ADDRESS ASTERISK
	BOOL_ NS_INTEGER NS_U_INTEGER  CG_FLOAT  DOUBLE NS_STRING NS_NUMBER NS_ARRAY NS_MUTABLE_ARRAY NS_DICTIONARY NS_MUTABLE_DICTIONARY ID
	CG_RECT CG_SIZE CG_POINT CG_AFFINE_TRANSFORM NS_RANGE

%type <expression> expression assign_expression ternary_operator_expression logic_or_expression logic_and_expression  
equality_expression relational_expression additive_expression multiplication_expression unary_expression postfix_expression
primary_expression dic block_body



%type <dic_entry> dic_entry
%type <dic_entry_list> dic_entry_list
%type <statement> statement if_statement switch_statement for_statement foreach_statement while_statement do_while_statement
break_statement continue_statement return_statement declaration_statement block_statement
%type <statement_list> statement_list
%type <type_specifier> type_specifier non_block_type_specifier base_type_specifier oc_type_specifier struct_type_specifier
oc_type_specifier custom_type_specifier block_type_specifier
%type <block> block default_opt
%type <type_specifier_list> type_specifier_list



%%

translation_util: /*empty*/
			| definition_list
			;


definition_list: definition
			| definition_list definition
			;

definition:  class_definition
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
			| type_specifier_list COMMA type_specifier
			;



non_block_type_specifier: base_type_specifier
			| struct_type_specifier
			| oc_type_specifier
			| custom_type_specifier
			;



block_type_specifier: IDENTIFIER LP POWER  RP LP type_specifier_list RP
			|  type_specifier LP POWER  RP LP type_specifier_list RP
			;


base_type_specifier: BOOL_
			|  NS_INTEGER
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

oc_type_specifier: NS_STRING
			| NS_NUMBER
			| NS_ARRAY
			| NS_MUTABLE_ARRAY
			| NS_DICTIONARY
			| NS_MUTABLE_DICTIONARY
			| ID
			;


custom_type_specifier: IDENTIFIER
			;


method_definition: instance_method_definition
			| class_method_definition
			;

instance_method_definition: MINUS LP type_specifier RP method_name block_statement
			;

class_method_definition: PLUS LP type_specifier RP method_name  block_statement
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
			| YES_
			| NO_
			| INTETER_LITERAL
			| DOUBLE_LITERAL
			| STRING_LITERAL
			| NIL
			| AT INTETER_LITERAL
			| AT DOUBLE_LITERAL
			| AT STRING_LITERAL
			| AT YES_
			| AT NO_
			| SELF
			| SUPER
			| AT LP expression RP
			| AT LB expression_list RB
			| AT LB  RB
			| dic
			| block_body
			;




block_body:  POWER type_specifier LP  RP block_statement
			| POWER type_specifier LP function_param_list RP block_statement
			| POWER  LP  RP block_statement
			| POWER  LP function_param_list RP block_statement
			;



function_param_list: function_param
			| function_param_list COMMA function_param
			;

function_param: type_specifier IDENTIFIER
			;


declaration_statement: type_specifier IDENTIFIER SEMICOLON
			| type_specifier IDENTIFIER ASSIGN expression SEMICOLON 
			;
			


if_statement: IF LP expression RP block_statement
			| IF LP expression RP block_statement ELSE block_statement
			| IF LP expression RP else_if_statement_list
			| IF LP expression RP else_if_statement_list ELSE block_statement
			;

else_if_statement_list: else_if_statement
			| else_if_statement_list else_if_statement
			;

else_if_statement: ELSE IF block_statement
			;

switch_statement: SWITCH LP expression RP LC case_list default_opt RC
			;

case_list: one_case
			| case_list one_case
			;

one_case: CASE expression COLON block_statement
			;

default_opt: /* empty */
			| DEFAULT COLON block_statement
			;

expression_opt: /* empty */
			| expression
			;

identifier_opt: /* empty */
			| IDENTIFIER
			;

label_opt: /* empty */
			| IDENTIFIER COLON
			;

for_statement: label_opt FOR LP expression_opt SEMICOLON expression_opt SEMICOLON expression_opt RP block_statement
			| FOR LP declaration_statement expression_opt SEMICOLON expression_opt RP block_statement
			;

while_statement: label_opt WHILE LP expression RP block_statement
			;

do_while_statement:label_opt DO block_statement WHILE LP expression RP SEMICOLON
			;

foreach_statement: label_opt FOR  LP type_specifier IDENTIFIER COLON expression RP block_statement
			;


continue_statement: CONTINUE identifier_opt
			;


break_statement: BREAK identifier_opt
			;


return_statement: RETURN expression_opt SEMICOLON {
				return NULL;
			}
			;


block_statement: LC RC
			| LC  statement_list RC
			;


statement_list: statement 
			| statement_list statement
			;


statement:  expression SEMICOLON
			| if_statement
			| switch_statement
			| for_statement
			| foreach_statement
			| while_statement
			| do_while_statement
			| break_statement
			| continue_statement
			| return_statement
			| declaration_statement
			;

%%
