//
//  generate.c
//  mango
//
//  Created by jerry.yong on 2017/6/26.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "DVM.h"
#include "DVM_code.h"
#include "mangoc.h"
#include <stdlib.h>
#include "MEM.h"
#include "DBG.h"
#include "share.h"

#define OPCODE_ALLOC_SIZE (256)
#define LABEL_ALLOC_SIZE (256)


extern OpcodeInfo dvm_opcode_info[];

typedef struct {
	size_t label_address;
}LableTable;


typedef struct {
	size_t size;
	size_t alloc_size;
	DVM_Byte *code;
	size_t label_table_szie;
	size_t label_table_alloc_size;
	LableTable *label_table;
	size_t line_number_size;
	DVM_LineNumber *line_number;
	size_t try_szie;
	DVM_Try *try;
}OpcodeBuf;


DVM_Executable *alloc_executable(PackageName *package_name){
	DVM_Executable *exe = MEM_malloc(sizeof(DVM_Executable));
	exe->package_name = mgc_package_name_to_string(package_name);
	exe->is_required = DVM_FALSE;
	exe->path = NULL;
	exe->constant_pool_count = 0;
	exe->constant_pool = NULL;
	exe->global_variable_count = 0;
	exe->global_variable = NULL;
	exe->function_count = 0;
	exe->function = NULL;
	exe->type_specifier_count = 0;
	exe->type_specifier = NULL;
	exe->class_count = 0;
	exe->class_definition = NULL;
	exe->enum_count = 0;
	exe->enum_definition = NULL;
	exe->constant_count = 0;
	exe->constant_definition = NULL;
	exe->top_level.code_size = 0;
	exe->top_level.code = NULL;
	exe->constant_initializer.code_size = 0;
	exe->constant_initializer.code = NULL;
	return exe;
	
	

}

static void init_code_buf(OpcodeBuf *code_buf){
	code_buf->alloc_size = 0;
	code_buf->size = 0;
	code_buf->code = NULL;
	code_buf->label_table_szie = 0;
	code_buf->label_table = 0;
	code_buf->label_table = NULL;
	code_buf->line_number_size = 0;
	code_buf->line_number = NULL;
	code_buf->try_szie = 0;
	code_buf->try = NULL;

}


static void fix_labels(OpcodeBuf *ob){
	for (size_t i = 0; i < ob->size; i++) {
		DVM_Byte code = ob->code[i];
		if (code == DVM_JUMP || code == DVM_JUMP_IF_TRUE || code == DVM_JUMP_IF_FALSE || code == DVM_GO_FINALLY) {
			size_t index = (ob->code[i + 1 ] << 8) + ob->code[i + 2];
			size_t address = ob->label_table[index].label_address;
			ob->code[i + 1] = address >> 8;
			ob->code[i + 2] = address & 0xff;

		}

	

		char *param = dvm_opcode_info[code].parameter;

		for (size_t j = 0; param[j] != '\0'; j++) {
			switch (param[j]) {
				case 'b':
					i++;
					break;
				case 'p':
				case 's':
					i+=2;
					break;

				default:
					break;
			}
		}
	}
	
}

static DVM_Byte* fix_opcode_buf(OpcodeBuf *ob){
	fix_labels(ob);
	DVM_Byte *ret = MEM_realloc(ob->code, ob->size);
	MEM_free(ob->label_table);
	return ret;
}

static size_t calc_stack_size(OpcodeBuf *ob){
	size_t count = 0;
	for (size_t i = 0; i < ob->size; i++) {
		DVM_Opcode code = ob->code[i];
		size_t stack_increment = dvm_opcode_info[code].stack_increment;
		if (stack_increment > 0) {
			count += stack_increment;
		}
		char *param = dvm_opcode_info[code].parameter;
		
		for (size_t j = 0; param[j] != '\0'; j++) {
			switch (param[j]) {
				case 'b':
					i++;
					break;
				case 'p':
				case 's':
					i+=2;
					break;
					
				default:
					break;
			}
		}
	}
	
	return count;
}



static void copy_opcode_buf(DVM_CodeBlock *dest, OpcodeBuf *ob){
	dest->code_size = ob->size;
	dest->code = fix_opcode_buf(ob);
	dest->line_number = ob->line_number;
	dest->line_number_size = ob->line_number_size;
	dest->try_size = ob->try_szie;
	dest->try = ob->try;
	dest->need_stack_size = calc_stack_size(ob);
}




static void add_line_number(OpcodeBuf *ob, int line_number, size_t start_pc){
	if (ob->line_number == NULL
		|| (ob->line_number[ob->line_number_size-1].line_number
			!= line_number)) {
			ob->line_number = MEM_realloc(ob->line_number,
										  sizeof(DVM_LineNumber)
										  * (ob->line_number_size + 1));
			
			ob->line_number[ob->line_number_size].line_number = line_number;
			ob->line_number[ob->line_number_size].start_pc = start_pc;
			ob->line_number[ob->line_number_size].pc_count
			= ob->size - start_pc;
			ob->line_number_size++;
			
		} else {
			ob->line_number[ob->line_number_size-1].pc_count
			+= ob->size - start_pc;
		}
}



static void generate_code(OpcodeBuf *ob, int line_number, DVM_Opcode code, ...){
	va_list ap;
	va_start(ap, code);
	char *parameter = dvm_opcode_info[code].parameter;
	if (ob->alloc_size < ob->size + 1 + strlen(parameter) * 2) {
		ob->alloc_size += OPCODE_ALLOC_SIZE;
		ob->code = MEM_realloc(ob->code,  sizeof(DVM_Byte) * ob->alloc_size);
	}
	size_t start_pc = ob->size;
	ob->code[ob->size] = (DVM_Byte)code;
	
		ob->size++;
		
		
		for (char *c = parameter; *c != '\0'; c++) {
					unsigned int value =  va_arg(ap, unsigned int);
			switch (*c) {
				case 'b'://bype 1bype
					ob->code[ob->size] = (DVM_Byte)value;
					ob->size++;
					break;
				case 's'://short 2bype
				case 'p'://pointer 2 bype
					
					ob->code[ob->size] = (DVM_Byte)(value >> 8);
					ob->code[ob->size + 1] = (DVM_Byte)(value & 0xff);
					ob->size += 2;
					break;
				default:
					DBG_assert(0, "param..%c",*c);
					break;
			}
		}
		va_end(ap);
		add_line_number(ob, line_number, start_pc);


	


}
static void generate_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob);


static DVM_TypeSpecifier *mgc_conver_type_specifier(TypeSpecifier *type);
static DVM_LocalVariable *conver_parameter_list(ParameterList *src, size_t *count){
	size_t i = 0;
	ParameterList *pos = src;
	for (; pos; pos = pos->next,i++)
		;
	DVM_LocalVariable *vars = MEM_malloc(sizeof(DVM_LocalVariable) * i);
	for (pos = src,i = 0; pos; pos = pos->next,i++) {
		vars[i].name = MEM_strdup(pos->name);
		vars[i].type = mgc_conver_type_specifier(pos->type);
	}
	if (count) {
		*count = i;
	}
	
	return vars;
	
}



static void generate_boolean_expression(DVM_Executable *exe, Expression *expr, OpcodeBuf *ob){
	if (expr->u.boolean_value) {
		generate_code(ob, expr->line_number, DVM_PUSH_INT_1BYTE, 1);
	}else{
		generate_code(ob, expr->line_number, DVM_PUSH_INT_1BYTE, 0);
	}
}

static size_t add_constant_pool(DVM_Executable *exe, DVM_ConstantPool *cp){
	exe->constant_pool = MEM_realloc(exe->constant_pool, sizeof(DVM_ConstantPool) *(exe->constant_pool_count + 1));
	exe->constant_pool[exe->constant_pool_count] = *cp;
	return exe->constant_pool_count++;
}

static void conver_type_specifier_no_alloc(TypeSpecifier *src, DVM_TypeSpecifier *dest){
	dest->base_type = src->base_type;
	if (src->base_type ==  DVM_CLASS_TYPE) {
		dest->u.class_t.index = src->u.class_ref.class_index;
	}else{
		dest->u.class_t.index = -1;
	}
	
	size_t derive_count = 0;
	for (TypeDerive *pos = src->derive; pos; pos= pos->next) {
		derive_count++;
	}
	dest->derive_count = 0;
	if (derive_count) {
		dest->derive = malloc(sizeof(DVM_TypeSpecifier) * derive_count);
		size_t index = 0;
		for (TypeDerive *derive_pos = src->derive; derive_pos; derive_pos = derive_pos->next,index++) {
			if (derive_pos->tag == FUNCTION_DERIVE) {
				dest->derive[index].tag = DVM_FUNCTION_DERIVE;
				size_t parameter_count = 0;
				dest->derive[index].u.function_d.parameter = conver_parameter_list(derive_pos->u.function_d.parameter_list, &parameter_count);
				dest->derive[index].u.function_d.parameter_count = parameter_count;
			}else{
				DBG_assert(derive_pos->tag == ARRAY_DERIVE,"derive_pos->tag..%d",derive_pos->tag);
				dest->derive[index].tag = DVM_ARRAY_DERIVE;
			}
		}
		
	}
	
	
	
}

static DVM_TypeSpecifier *mgc_conver_type_specifier(TypeSpecifier *type){
	DVM_TypeSpecifier *dvm_type = MEM_malloc(sizeof(DVM_TypeSpecifier));
	conver_type_specifier_no_alloc(type, dvm_type);
	return dvm_type;
}


static void generate_int_expression(DVM_Executable *exe, int line_number,int value, OpcodeBuf *ob){
	if (value >= 0 && value < 256) {
		generate_code(ob, line_number, DVM_PUSH_INT_1BYTE, value);
	}else if (value >= 0 && value < 65536){
		generate_code(ob, line_number, DVM_PUSH_INT_2BYTE, value);
	}else{
		DVM_ConstantPool  cp;
		cp.tag = DVM_CONSTANT_INT;
		cp.u.c_int = value;
		size_t index = add_constant_pool(exe, &cp);
		generate_code(ob, line_number, DVM_PUSH_INT, (int)index);
	}
}

static void generate_double_expression(DVM_Executable *exe, Expression *expr, OpcodeBuf *ob){
	if (expr->u.double_value == 0.0) {
		generate_code(ob, expr->line_number, DVM_PUSH_DOUBLE_0);
	}else if (expr->u.double_value == 1.0){
		generate_code(ob, expr->line_number, DVM_PUSH_DOUBLE_1);
	}else{
		DVM_ConstantPool cp;
		cp.tag = DVM_CONSTANT_DOUBLE;
		cp.u.c_double = expr->u.double_value;
		size_t index = add_constant_pool(exe, &cp);
		generate_code(ob, expr->line_number, DVM_PUSH_DOUBLE, (int)index);
	}
}

static void generate_string_expression(DVM_Executable *exe, Expression *expr, OpcodeBuf *ob){
	DVM_ConstantPool cp;
	cp.tag = DVM_CONSTANT_STRING;
	cp.u.c_string = expr->u.string_value;
	size_t index = add_constant_pool(exe, &cp);
	generate_code(ob, expr->line_number, DVM_PUSH_STRING, (int)index);
}

static int get_opcode_type_offset(TypeSpecifier *type){
	switch (type->base_type) {
		case DVM_INT_TYPE:
		case DVM_BOOLEAN_TYPE:
		case DVM_ENUM_TYPE:
			return 0;
		case DVM_DOUBLE_TYPE:
			return 1;
		case DVM_CLASS_TYPE:
		case DVM_STRING_TYPE:
		case DVM_NATIVE_POINTER_TYPE:
		case DVM_DELEGAET_TYPE:
			return 2;
		case DVM_NULL_TYPE:
		case DVM_BASE_TYPE:
		case DVM_VOID_TYPE:
		case DVM_UNSPECIFIED_IDENTIFIER_TYPE:
		default:
			DBG_assert(0, "base_type..%d",type->base_type);
	}
	return 0;
}

static int
get_binary_expression_offset(Expression *left, Expression *right,
							 DVM_Opcode code)
{
	int offset;
	
	if ((left->kind == NULL_EXPRESSION && right->kind != NULL_EXPRESSION)
		|| (left->kind != NULL_EXPRESSION && right->kind == NULL_EXPRESSION)) {
		offset = 2; /* object type */
		
	} else if ((code == DVM_EQ_INT || code == DVM_NE_INT)
			   && mgc_is_string(left->type)) {
		offset = 3; /* string type */
		
	} else {
		offset = get_opcode_type_offset(left->type);
	}
	
	return offset;
}


static void generate_variable_identifier_expression(Declaration *decl, OpcodeBuf *ob, int line_number){
	
	if (decl->is_loacl) {
		generate_code(ob, line_number, DVM_PUSH_STACK_INT + get_opcode_type_offset(decl->type), decl->variable_index);
	}else{
		generate_code(ob, line_number, DVM_PUSH_STATIC_INT + get_opcode_type_offset(decl->type), decl->variable_index);
	}
}


static void generate_identifer_expression(DVM_Executable *exe, Block *cuurent_block, Expression *expr, OpcodeBuf *ob){
	switch (expr->u.identifer_express.kind) {
		case VARIABLE_IDENTIFER:
			generate_variable_identifier_expression(expr->u.identifer_express.u.declaration, ob, expr->line_number);
			break;
		case FUNCTION_IDENTIFER:
			generate_code(ob, expr->line_number, DVM_PUSH_FUNCTION, expr->u.identifer_express.u.function.function_index);
			break;
		case CONSTANT_IDENTIFER:
			generate_code(ob, expr->line_number, DVM_PUSH_CONSTANT_INT + get_opcode_type_offset(expr->type), expr->u.identifer_express.u.constant.constant_index);
			break;
			
		default:
			break;
	}


}

static void genereate_pop_to_identifier(Declaration *decl,int line_number, OpcodeBuf *ob){
	
	if (decl->is_loacl) {
		generate_code(ob, line_number, DVM_POP_STACK_INT + get_opcode_type_offset(decl->type), decl->variable_index);
	}else{
		generate_code(ob, line_number, DVM_POP_STATIC_INT + get_opcode_type_offset(decl->type), decl->variable_index);
	}

}




static void generate_pop_to_lvalue(DVM_Executable *exe, Block *block, Expression *expr, OpcodeBuf *ob){
	if (expr->kind == IDENTIFIER_EXPRESSION) {
		genereate_pop_to_identifier(expr->u.identifer_express.u.declaration,expr->line_number, ob);
	}else if (expr->kind == INDEX_EXPRESSION){
		generate_expression(exe, block, expr->u.index_expression.array, ob);
		generate_expression(exe, block, expr->u.index_expression.index, ob);
		generate_code(ob, expr->line_number, DVM_POP_ARRAY_INT + get_opcode_type_offset(expr->type));
		
	}else{
		DBG_assert(expr->kind == MEMBER_EXPRESSION, "expr->kind..%d", expr->kind);
		MemberDeclaration *member = expr->u.member_expression.declaration;
		if (member->kind == METHOD_MEMBER) {
			mgc_compile_error(expr->line_number, ACCESS_TO_METHOD_ERR, STRING_MESSAGE_ARGUMENT, "name", member->u.method.function_definition->name, MESSAGE_ARGUMENT_END);
		}
		generate_expression(exe, block, expr->u.member_expression.expression, ob);
		generate_code(ob, expr->line_number, DVM_POP_FIELD_INT + get_opcode_type_offset(member->u.field.type), member->u.field.field_index);
		
	}


}



static void generate_assign_expression(DVM_Executable *exe, Block *block, Expression *expr, OpcodeBuf *ob, DVM_Boolean statement_top){
	if (expr->u.assignment_expression.operator == NORMAL_ASSIGN) {
		generate_expression(exe, block, expr->u.assignment_expression.operand, ob);
	}else{
		generate_expression(exe, block, expr->u.assignment_expression.left, ob);
		generate_expression(exe, block, expr->u.assignment_expression.operand, ob);
		switch (expr->u.assignment_expression.operator) {
			case ADD_ASSIGN:
				generate_code(ob, expr->line_number, DVM_ADD_INT + get_opcode_type_offset(expr->type));
			break;
			case SUB_ASSIGN:
				generate_code(ob, expr->line_number, DVM_SUB_INT + get_opcode_type_offset(expr->type));
				break;
			case MUL_ASSIGN:
				generate_code(ob, expr->line_number, DVM_MUL_INT + get_opcode_type_offset(expr->type));
				break;
			case DIV_ASSIGN:
				generate_code(ob, expr->line_number, DVM_DIV_INT + get_opcode_type_offset(expr->type));
				break;
			case MOD_ASSIGN:
				generate_code(ob, expr->line_number, DVM_MOD_INT + get_opcode_type_offset(expr->type));
				break;
			default:
				DBG_assert(0, "expr->u.assignment_expression.operator..%d",expr->u.assignment_expression.operator);
				break;
		}
	
	}
	
	if (!statement_top) {
		generate_code(ob, expr->line_number, DVM_DUPLICATE);
	}
	
	generate_pop_to_lvalue(exe, block, expr->u.assignment_expression.left, ob);
	
}


static void generate_binary_expression(DVM_Executable *exe, Block *block, Expression *expr, OpcodeBuf *ob, DVM_Opcode code){
	generate_expression(exe, block, expr->u.binary_expression.left, ob);
	generate_expression(exe, block, expr->u.binary_expression.right, ob);
	int offset = get_opcode_type_offset(expr->type);
	generate_code(ob, expr->line_number, code + offset);
}

static void generate_bit_binary_expression(DVM_Executable *exe, Block *block, Expression *expr, OpcodeBuf *ob, DVM_Opcode code){
	generate_expression(exe, block, expr->u.binary_expression.left, ob);
	generate_expression(exe, block, expr->u.binary_expression.right, ob);
	generate_code(ob, expr->line_number, code);
}

static size_t get_label(OpcodeBuf *ob){
	if (ob->label_table_szie >= ob->label_table_alloc_size) {
		ob->label_table = MEM_realloc(ob->label_table, sizeof(LableTable) * (ob->label_table_alloc_size + LABEL_ALLOC_SIZE));
		ob->label_table_alloc_size += LABEL_ALLOC_SIZE;
	}
	
	return ob->label_table_szie++;
}

static void set_label(OpcodeBuf *ob, size_t index){
	ob->label_table[index].label_address = ob->size;
}


static void generate_logic_and_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	generate_expression(exe, current_block, expr->u.binary_expression.left, ob);
	generate_code(ob, expr->line_number, DVM_DUPLICATE);
	size_t label = get_label(ob);
	generate_code(ob, expr->line_number, DVM_JUMP_IF_FALSE, label);
	generate_expression(exe, current_block, expr->u.binary_expression.right, ob);
	generate_code(ob, expr->line_number, DVM_LOGIC_AND);
	set_label(ob, label);
}

static void generate_logic_or_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	generate_expression(exe, current_block, expr->u.binary_expression.left, ob);
	generate_code(ob, expr->line_number, DVM_DUPLICATE);
	size_t label = get_label(ob);
	generate_code(ob, expr->line_number, DVM_JUMP_IF_TRUE, label);
	generate_expression(exe, current_block, expr, ob);
	generate_code(ob, expr->line_number, DVM_LOGIC_OR);
	set_label(ob, label);
}


static void generate_argument(DVM_Executable *exe, Block *current_block, ArgumentList *arg_list, OpcodeBuf *ob){
	for (ArgumentList *arg_pos = arg_list; arg_pos; arg_pos = arg_pos->next) {
		generate_expression(exe, current_block, arg_pos->expression, ob);
	}

}

static size_t get_method_index(MemberExpression *member){
	size_t index = 0;
	if (mgc_is_array(member->expression->type) || mgc_is_string(member->expression->type)) {
		index = member->method_index;
	}else{
		index = member->declaration->u.method.method_index;
	}
	return index;
}

static void generate_method_call_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	generate_argument(exe, current_block, expr->u.function_call_expression.argument, ob);
	MemberExpression *member = &expr->u.function_call_expression.function->u.member_expression;
	generate_expression(exe, current_block, member->expression, ob);

	size_t index =  get_method_index(member);
	generate_code(ob, expr->line_number, DVM_PUSH_METHOD, index);
	generate_code(ob, expr->line_number, DVM_INVOKE);
}



static void generate_function_call_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	FunctionCallExpress *call_expr = &expr->u.function_call_expression;
	if (call_expr->function->kind == MEMBER_EXPRESSION) {
		generate_method_call_expression(exe, current_block, expr, ob);
		return;
	}
	
	generate_argument(exe, current_block, call_expr->argument, ob);
	generate_expression(exe, current_block, call_expr->function, ob);
	if (mgc_is_delegate(call_expr->function->type)) {
		generate_code(ob, expr->line_number, DVM_INVOKE_DELEGATE);
	}else{
		generate_code(ob, expr->line_number, DVM_INVOKE);
	}
}

static void generate_member_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	MemberDeclaration *decl = expr->u.member_expression.declaration;
	if (mgc_is_array(expr->u.member_expression.expression->type)
		|| mgc_is_string(expr->u.member_expression.expression->type)
		|| decl->kind == METHOD_MEMBER) {
		generate_expression(exe, current_block, expr->u.member_expression.expression, ob);
		size_t index = get_method_index(&expr->u.member_expression);
		generate_code(ob, expr->line_number, DVM_PUSH_METHOD_DELEGATE,index);
		
	}else{
		DBG_assert(decl->kind == FIELD_MEMBER, "decl->kind..%d",decl->kind);
		generate_expression(exe, current_block, expr->u.member_expression.expression, ob);
		generate_code(ob, expr->line_number, DVM_PUSH_FIELD_INT + get_opcode_type_offset(decl->u.field.type),decl->u.field.field_index);
	}

}

static FunctionDefinition * get_current_function(Block *current_block){
	for (Block *pos = current_block; pos; pos = pos->out_block) {
		if (pos->type == FUNCTION_BLOCK) {
			return pos->parent.function_info.function;
		}
	}
	return NULL;
}


static size_t parameter_count(ParameterList *param_list){
	size_t count = 0;
	for (ParameterList *pos = param_list; pos; pos = pos->next) {
		count++;
	}
	return count;

}



static void generate_this_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	FunctionDefinition *fd = get_current_function(current_block);
	size_t param_count = parameter_count(fd->parameter_list);
	generate_code(ob, expr->line_number, DVM_PUSH_STACK_OBJECT, param_count);
	

}

static void generate_super_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	FunctionDefinition *fd = get_current_function(current_block);
	size_t index = parameter_count(fd->parameter_list);
	generate_code(ob, expr->line_number, DVM_PUSH_STACK_OBJECT,index);
	generate_code(ob, expr->line_number, DVM_SUPER);

}


static void generate_new_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	generate_code(ob, expr->line_number, DVM_NEW,expr->u.new_e.class_index);
	generate_argument(exe, current_block, expr->u.new_e.argument, ob);
	generate_code(ob, expr->line_number, DVM_DUPLICATE_OFFSET,parameter_count(expr->u.new_e.method_delclaration->u.method.function_definition->parameter_list));
	generate_code(ob, expr->line_number, DVM_PUSH_METHOD,expr->u.new_e.method_delclaration->u.method.method_index);
	generate_code(ob, expr->line_number, DVM_INVOKE);
	generate_code(ob, expr->line_number, DVM_POP);
}


static void generate_array_literal_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	size_t size = 0;
	for (ExpressionList *pos = expr->u.array_literal; pos; pos = pos->next) {
		generate_expression(exe, current_block, pos->expression, ob);
		size++;
	}
	generate_code(ob, expr->line_number, DVM_NEW_ARRAY_LTTERAL_INT + get_opcode_type_offset(expr->u.array_literal->expression->type), size);
	
}

static void generate_index_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	generate_expression(exe, current_block, expr->u.index_expression.array, ob);
	generate_expression(exe, current_block, expr->u.index_expression.index, ob);
	
	if (mgc_is_string(expr->u.index_expression.array->type)) {
		generate_code(ob, expr->line_number, DVM_PUSH_CHARACTER_IN_STRING);
	}else{
		generate_code(ob, expr->line_number, DVM_PUSH_ARRAY_INT + get_opcode_type_offset(expr->type));
	}

}

static void genereate_inc_dec_expression(DVM_Executable *exe, Block *current_block, Expression *expr, ExpressionKind kind, OpcodeBuf *ob, DVM_Boolean top){
	generate_expression(exe, current_block, expr->u.inc_dec.operand, ob);
	if (kind == INCREMENT_EXPRESSION) {
		generate_code(ob, expr->line_number, DVM_INCREMENT);
	}else{
		DBG_assert(kind == DECREMENT_EXPRESSION, "kind..%d", kind);
		generate_code(ob, expr->line_number, DVM_DECREMENT);
	}
	
	if (!top) {
		generate_code(ob, expr->line_number, DVM_DUPLICATE);
	}
	
	generate_pop_to_lvalue(exe, current_block, expr->u.inc_dec.operand, ob);

}

static void generate_instanceof_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	generate_expression(exe, current_block, expr->u.instanceof.expression, ob);
	generate_code(ob, expr->line_number, DVM_INSTANCEOF, expr->u.instanceof.type->u.class_ref.class_index);
}

static void generate_down_cast_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	generate_expression(exe, current_block, expr->u.down_cast.operand, ob);
	generate_code(ob, expr->line_number, DVM_DOWN_CAST, expr->u.down_cast.type->u.class_ref.class_index);
}


static void generate_up_cast_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	generate_expression(exe, current_block, expr, ob);
	generate_code(ob, expr->line_number, DVM_UP_CAST, expr->u.up_cast.interface_index);
}

static void generate_cast_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	if (expr->u.cast.type == FUNCTION_TO_DELEGATE_CAST) {
		if (expr->u.cast.operand->kind == IDENTIFIER_EXPRESSION) {
			generate_code(ob, expr->line_number, DVM_PUSH_DELEGATE, expr->u.cast.operand->u.identifer_express.u.function.function_index);
		}else{
			DBG_assert(expr->u.cast.operand->kind == MEMBER_EXPRESSION, "kind..%d",expr->u.cast.operand->kind);
			generate_expression(exe, current_block, expr, ob);
		}
	}else{
		generate_expression(exe, current_block, expr->u.cast.operand, ob);
		switch (expr->u.cast.type) {
			case BOOLEAN_TO_STRING_CAST:
				generate_code(ob, expr->line_number, DVM_CAST_BOOLEAN_TO_STRING);
				break;
			case INT_TO_DOUBLE_CAST:
				generate_code(ob, expr->line_number, DVM_CAST_INT_TO_DOUBLE);
				break;
			case INT_TO_STRING_CAST:
				generate_code(ob, expr->line_number, DVM_CAST_INT_TO_STRING);
				break;
			case DOUBLE_TO_INT_CAST:
				generate_code(ob, expr->line_number, DVM_CAST_DOUBLE_TO_INT);
				break;
			case DOUBLE_TO_STRING_CAST:
				generate_code(ob, expr->line_number, DVM_CAST_DOUBLE_TO_STRING);
				break;
			case ENUM_TO_STRING_CAST:
				generate_code(ob, expr->line_number, DVM_CAST_ENUM_TO_STRING);
				
			default:
		break;
		}
	}
}

static size_t add_type_specifier(TypeSpecifier *type, DVM_Executable *exe){
	exe->type_specifier = MEM_realloc(exe->type_specifier, sizeof(DVM_TypeSpecifier) * (exe->type_specifier_count + 1));
	DVM_TypeSpecifier *dvm_type = mgc_conver_type_specifier(type);
	exe->type_specifier[exe->type_specifier_count] = *dvm_type;
	return  exe->type_specifier_count++;
}


static void generate_array_creation_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	size_t index = add_type_specifier(expr->type, exe);
	size_t dim_count = 0;
	for (ArrayDimension *pos = expr->u.array_creation.dimension; pos; pos = pos->next) {
		if (!pos->expression) {
			continue;
		}
		generate_expression(exe, current_block, pos->expression, ob);
		dim_count++;
	}
	
	generate_code(ob, expr->line_number, DVM_NEW_ARRAY, dim_count, index);
}





static void generate_expression(DVM_Executable *exe, Block *current_block, Expression *expr, OpcodeBuf *ob){
	switch (expr->kind) {
		case BOOLEAN_EXPRESSION:
			generate_boolean_expression(exe, expr, ob);
			break;
		case INT_EXPRESSION:
			generate_int_expression(exe, expr->line_number, expr->u.int_value, ob);
			break;
		case DOUBLE_EXPRESSION:
			generate_double_expression(exe, expr, ob);
			break;
		case STRING_EXPRESSION:
			generate_string_expression(exe, expr, ob);
			break;
		case IDENTIFIER_EXPRESSION:
			generate_identifer_expression(exe, current_block, expr, ob);
			break;
		case COMMA_EXPRESSION:
			generate_expression(exe, current_block, expr->u.comma_expression.left, ob);
			generate_expression(exe, current_block, expr->u.comma_expression.right, ob);
			break;
		case ASSIGN_EXPRESSION:
			generate_assign_expression(exe, current_block, expr, ob, DVM_FALSE);
			break;
		case ADD_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_ADD_INT);
			break;
		case SUB_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_SUB_INT);
			break;
		case MUL_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_MUL_INT);
			break;
		case DIV_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_DIV_INT);
			break;
		case MOD_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_MOD_INT);
			break;
		case EQ_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_EQ_INT);
			break;
		case NE_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_NE_INT);
			break;
		case GE_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_GE_INT);
			break;
		case GT_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_GT_INT);
			break;
		case LE_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_LE_INT);
			break;
		case LT_EXPRESSION:
			generate_binary_expression(exe, current_block, expr, ob, DVM_LT_INT);
			break;
		case BIT_AND_EXPRESSION:
			generate_bit_binary_expression(exe, current_block, expr, ob, DVM_BIT_AND);
			break;
		case BIT_OR_EXPRESSION:
			generate_bit_binary_expression(exe, current_block, expr, ob, DVM_BIT_OR);
			break;
		case BIT_XOR_EXPRESSION:
			generate_bit_binary_expression(exe, current_block, expr, ob, DVM_BIT_XOR);
			break;
		case LOGICAL_AND_EXPRESSION:
			generate_logic_and_expression(exe, current_block, expr, ob);
			break;
		case LOGICAL_OR_EXPRESSION:
			generate_logic_or_expression(exe, current_block, expr, ob);
			break;
		case MINUS_EXPRESSION:
			generate_expression(exe, current_block, expr, ob);
			generate_code(ob, expr->line_number, DVM_MINUS_INT + get_opcode_type_offset(expr->type));
			break;
		case LOGICAL_NOT_EXPRESSION:
			generate_expression(exe, current_block, expr, ob);
			generate_code(ob, expr->line_number, DVM_LOGIC_NOT);
			break;
		case BIT_NOT_EXPRESSION:
			generate_expression(exe, current_block, expr, ob);
			generate_code(ob, expr->line_number, DVM_BIT_NOT);
			break;
		case FUNCTION_CALL_EXPRESSION:
			generate_function_call_expression(exe, current_block, expr, ob);
			break;
		case MEMBER_EXPRESSION:
			generate_member_expression(exe, current_block, expr, ob);
			break;
		case NULL_EXPRESSION:
			generate_code(ob, expr->line_number, DVM_PUSH_NULL);
			break;
		case THIS_EXPRESSION:
			generate_this_expression(exe, current_block, expr, ob);
			break;
		case SUPER_EXPRESSION:
			generate_super_expression(exe, current_block, expr, ob);
			break;
		case NEW_EXPRESSION:
			generate_new_expression(exe, current_block, expr, ob);
			break;
		case ARRAY_LITERAL_EXPRESSION:
			generate_array_literal_expression(exe, current_block, expr, ob);
			break;
		case INDEX_EXPRESSION:
			generate_index_expression(exe, current_block, expr, ob);
			break;
		case INCREMENT_EXPRESSION:
		case DECREMENT_EXPRESSION:
			genereate_inc_dec_expression(exe, current_block, expr, expr->kind, ob, DVM_FALSE);
			break;
		case INSTANCEOF_EXPRESSION:
			generate_instanceof_expression(exe, current_block, expr, ob);
			break;
		case DOWN_CAST_EXPRESSION:
			generate_down_cast_expression(exe, current_block, expr, ob);
			break;
		case CAST_EXPRESSION:
			generate_cast_expression(exe, current_block, expr, ob);
			break;
		case UP_CAST_EXPRESSION:
			generate_up_cast_expression(exe, current_block, expr, ob);
			break;
		case ARRAY_CREATION_EXPRESSION:
			generate_array_creation_expression(exe, current_block, expr, ob);
			break;
		case ENUMERATOR_EXPRESSION:
			generate_int_expression(exe, expr->line_number, expr->u.enumerator.enumerator->value, ob);
			break;
		case EXPRESSION_KIND_COUNT_PLUS_1:
		default:
			DBG_assert(0, "expr->kind..%d",expr->kind);
			break;
	}
	
	

}
static void generate_statement_list(DVM_Executable *exe, Block *current_block, StatementList *statement_list, OpcodeBuf *ob);

static void generate_expression_statement(DVM_Executable *exe, Block *block, Expression *expr, OpcodeBuf *ob){
	if (expr->kind == ASSIGN_EXPRESSION) {
		generate_assign_expression(exe, block, expr, ob, DVM_TRUE);
	}else if (expr->kind == INCREMENT_EXPRESSION
			  || expr->kind == DECREMENT_EXPRESSION){
		genereate_inc_dec_expression(exe, block, expr, expr->kind, ob, DVM_TRUE);
		
	}else{
		generate_expression(exe, block, expr, ob);
		generate_code(ob, expr->line_number, DVM_POP);
	}
	
}

static void genereate_if_statement(DVM_Executable *exe, Block *block, Statement *if_s, OpcodeBuf *ob){
	size_t false_label = get_label(ob);
	size_t end_label = get_label(ob);
	generate_expression(exe, block, if_s->u.if_s.condition, ob);
	generate_code(ob, if_s->line_number, DVM_JUMP_IF_FALSE,false_label);
	generate_statement_list(exe, if_s->u.if_s.then_block, if_s->u.if_s.then_block->statement_list, ob);
	generate_code(ob, if_s->line_number, DVM_JUMP, end_label);
	set_label(ob, false_label);
	for (Elsif *pos = if_s->u.if_s.elsif_list; pos; pos = pos->next) {
		false_label = get_label(ob);
		generate_expression(exe, block, pos->condition, ob);
		generate_code(ob, if_s->line_number, DVM_JUMP_IF_FALSE,false_label);
		generate_statement_list(exe, pos->then_block, pos->then_block->statement_list, ob);
		generate_code(ob, if_s->line_number, DVM_JUMP,end_label);
		set_label(ob, false_label);
	}
	
	if (if_s->u.if_s.else_block) {
		generate_statement_list(exe, if_s->u.if_s.else_block, if_s->u.if_s.else_block->statement_list, ob);
	}
	set_label(ob, end_label);
}


static void generate_switch_statement(DVM_Executable *exe, Block *block, Statement *switch_s, OpcodeBuf *ob){
	size_t end_label = get_label(ob);
	generate_expression(exe, block, switch_s->u.switch_s.expression, ob);
	for (CaseList *pos = switch_s->u.switch_s.case_list; pos; pos = pos->next) {
		size_t case_start_label = get_label(ob);
		size_t next_case_label = get_label(ob);
		for (ExpressionList *expr_pos = pos->expression_list; expr_pos; expr_pos = expr_pos->next) {
			generate_code(ob, switch_s->line_number, DVM_DUPLICATE);
			generate_expression(exe, block, expr_pos->expression, ob);
			int offset = get_binary_expression_offset(switch_s->u.switch_s.expression, expr_pos->expression, DVM_EQ_INT);
			generate_code(ob, switch_s->line_number, DVM_EQ_INT + offset);
			generate_code(ob, switch_s->line_number, DVM_JUMP_IF_TRUE, case_start_label);
		}
		generate_code(ob, switch_s->line_number, DVM_JUMP, next_case_label);
		set_label(ob, case_start_label);
		generate_statement_list(exe, pos->block, pos->block->statement_list, ob);
		generate_code(ob, switch_s->line_number, DVM_JUMP, end_label);
		set_label(ob, next_case_label);
	}
	
	if (switch_s->u.switch_s.default_block) {
		generate_statement_list(exe, switch_s->u.switch_s.default_block, switch_s->u.switch_s.default_block->statement_list, ob);
	}
	set_label(ob, end_label);
	return;
}


static void generate_while_statement(DVM_Executable *exe, Block *block, Statement *while_s, OpcodeBuf *ob){
	size_t break_label = get_label(ob);
	size_t continue_label = get_label(ob);
	size_t loop_label = get_label(ob);
	while_s->u.while_s.block->parent.statement_info.break_label = break_label;
	while_s->u.while_s.block->parent.statement_info.continue_label = continue_label;
	set_label(ob, loop_label);
	generate_expression(exe, block, while_s->u.while_s.condition, ob);
	generate_code(ob, while_s->line_number, DVM_JUMP_IF_FALSE, break_label);
	generate_statement_list(exe, while_s->u.while_s.block, while_s->u.while_s.block->statement_list, ob);
	set_label(ob, continue_label);
	generate_code(ob, while_s->line_number, DVM_JUMP);
	set_label(ob, break_label);
}


static void genereate_do_while_statement(DVM_Executable *exe, Block *block, Statement *do_while_s, OpcodeBuf *ob){
	size_t break_label = get_label(ob);
	size_t continue_label = get_label(ob);
	size_t loop_label = get_label(ob);
	do_while_s->u.do_while_s.block->parent.statement_info.break_label = break_label;
	do_while_s->u.do_while_s.block->parent.statement_info.continue_label = continue_label;
	set_label(ob, loop_label);
	generate_statement_list(exe, do_while_s->u.do_while_s.block, do_while_s->u.do_while_s.block->statement_list, ob);
	set_label(ob, continue_label);
	generate_expression(exe, block, do_while_s->u.do_while_s.condition, ob);
	generate_code(ob, do_while_s->line_number, DVM_JUMP_IF_TRUE, loop_label);
	set_label(ob, break_label);
}



static void genereate_for_statement(DVM_Executable *exe, Block *block, Statement *for_s, OpcodeBuf *ob){
	if (for_s->u.for_s.init) {
		generate_expression(exe, block, for_s->u.for_s.init, ob);
	}
	
	size_t break_label = get_label(ob);
	size_t continue_label = get_label(ob);
	for_s->u.for_s.block->parent.statement_info.break_label = break_label;
	for_s->u.for_s.block->parent.statement_info.continue_label = continue_label;
	size_t loop_label = get_label(ob);
	set_label(ob, loop_label);
	if (for_s->u.for_s.condition) {
		generate_expression(exe, block, for_s->u.for_s.condition, ob);
		generate_code(ob, for_s->line_number, DVM_JUMP_IF_FALSE,break_label);
	}
	
	generate_statement_list(exe, for_s->u.for_s.block, for_s->u.for_s.block->statement_list, ob);
	set_label(ob, continue_label);
	if (for_s->u.for_s.post) {
		generate_expression(exe, block, for_s->u.for_s.post, ob);

	}
	generate_code(ob, for_s->line_number, DVM_JUMP,loop_label);
	set_label(ob, break_label);
	
}

static void generate_return_statement(DVM_Executable *exe, Block *block, Statement *return_s, OpcodeBuf *ob){
	for (Block *pos = block; block; block = block->out_block) {
		if (pos->type == FUNCTION_BLOCK) {
			break;
		}
		if (pos->type == TRY_CLAUSE_BLOCK || pos->type == CATCH_CLAUSE_BLOCK) {
			generate_code(ob, return_s->line_number, DVM_GO_FINALLY,mgc_get_current_compiler()->current_finally_label);
		}
	}
	
	generate_expression(exe, block, return_s->u.return_s.return_value, ob);
	generate_code(ob, return_s->line_number, DVM_RETURN);

}

static void generate_break_statement(DVM_Executable *exe, Block *block, Statement *break_s, OpcodeBuf *ob){
	DVM_Boolean in_finally = DVM_FALSE;
	Block *loop_block = NULL;
	for (loop_block = block; loop_block; loop_block = loop_block->out_block) {
		if (loop_block->type == TRY_CLAUSE_BLOCK || loop_block->type == CATCH_CLAUSE_BLOCK) {
			in_finally = DVM_TRUE;
			continue;
		}
		
		char *break_label = break_s->u.break_s.label;
		
		if (loop_block->type == FOR_STATEMENT_BLOCK) {
			if (break_s == NULL) {
				break;
			}
			char *for_label= loop_block->parent.statement_info.statement->u.for_s.label;
			if (for_label != NULL && strcat(for_label, break_label)) {
				break;
			}
		}else if (loop_block->type == WHILE_STATEMENT_BLOCK){
			if (break_s == NULL) {
				break;
			}
			
			char *while_label= loop_block->parent.statement_info.statement->u.while_s.label;
			if (while_label != NULL && strcat(while_label, break_label)) {
				break;
			}
		
		}else if (loop_block->type == DO_WHILE_STATEMENT_BLOCK){
			if (break_s == NULL) {
				break;
			}
			
			char *do_while_label= loop_block->parent.statement_info.statement->u.do_while_s.label;
			if (do_while_label != NULL && strcat(do_while_label, break_label)) {
				break;
			}
		}
	}
	
	if (loop_block == NULL) {
		mgc_compile_error(break_s->line_number, LABEL_NOT_FOUND_ERR, STRING_MESSAGE_ARGUMENT, "label", break_s->u.break_s.label, MESSAGE_ARGUMENT_END);
	}
	
	if (in_finally) {
		generate_code(ob, break_s->line_number, DVM_GO_FINALLY,mgc_get_current_compiler()->current_finally_label);
	}
	
	generate_code(ob, break_s->line_number, DVM_JUMP, loop_block->parent.statement_info.break_label);


}




static void generate_continue_statement(DVM_Executable *exe, Block *block, Statement *continue_s, OpcodeBuf *ob){
	DVM_Boolean in_finally = DVM_FALSE;
	Block *loop_block = NULL;
	for (loop_block = block; loop_block; loop_block = loop_block->out_block) {
		if (loop_block->type == TRY_CLAUSE_BLOCK || loop_block->type == CATCH_CLAUSE_BLOCK) {
			in_finally = DVM_TRUE;
			continue;
		}
		
		char *continue_label = continue_s->u.break_s.label;
		
		if (loop_block->type == FOR_STATEMENT_BLOCK) {
			if (continue_s == NULL) {
				break;
			}
			char *for_label= loop_block->parent.statement_info.statement->u.for_s.label;
			if (for_label != NULL && strcat(for_label, continue_label)) {
				break;
			}
		}else if (loop_block->type == WHILE_STATEMENT_BLOCK){
			if (continue_s == NULL) {
				break;
			}
			
			char *while_label= loop_block->parent.statement_info.statement->u.while_s.label;
			if (while_label != NULL && strcat(while_label, continue_label)) {
				break;
			}
			
		}else if (loop_block->type == DO_WHILE_STATEMENT_BLOCK){
			if (continue_s == NULL) {
				break;
			}
			
			char *do_while_label= loop_block->parent.statement_info.statement->u.do_while_s.label;
			if (do_while_label != NULL && strcat(do_while_label, continue_label)) {
				break;
			}
		}
	}
	
	if (loop_block == NULL) {
		mgc_compile_error(continue_s->line_number, LABEL_NOT_FOUND_ERR, STRING_MESSAGE_ARGUMENT, "label", continue_s->u.break_s.label, MESSAGE_ARGUMENT_END);
	}
	
	if (in_finally) {
		generate_code(ob, continue_s->line_number, DVM_GO_FINALLY,mgc_get_current_compiler()->current_finally_label);
	}
	
	generate_code(ob, continue_s->line_number, DVM_JUMP, loop_block->parent.statement_info.continue_label);
	
	
}


static void generate_try_statement(DVM_Executable *exe, Block *block, Statement *try_s, OpcodeBuf *ob){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	DVM_Try dvm_try;
	size_t backup_finally_label = compiler->current_finally_label;
	compiler->current_finally_label = get_label(ob);
	size_t after_finally_label = get_label(ob);
	dvm_try.try_start_pc = ob->size;
	generate_statement_list(exe, try_s->u.try_s.try_block, try_s->u.try_s.try_block->statement_list, ob);
	generate_code(ob, try_s->line_number, DVM_GO_FINALLY,compiler->current_finally_label);
	generate_code(ob, try_s->line_number, DVM_JUMP,after_finally_label);
	dvm_try.try_end_pc = ob->size - 1;
	size_t catch_count = 0;
	for (CatchClause *catch_pos = try_s->u.try_s.catch_clause; catch_pos; catch_pos = catch_pos->next) {
		catch_count ++;
	}
	dvm_try.catch_clause = MEM_malloc(sizeof(DVM_CatchClause) * catch_count);
	size_t catch_index = 0;
	for (CatchClause *catch_pos = try_s->u.try_s.catch_clause; catch_pos; catch_pos = catch_pos->next,catch_index++) {
		dvm_try.catch_clause[catch_index].start_pc = ob->size;
		dvm_try.catch_clause[catch_index].class_index = catch_pos->variable_declaration->type->u.class_ref.class_index;
		genereate_pop_to_identifier(catch_pos->variable_declaration, catch_pos->line_number, ob);
		generate_statement_list(exe, catch_pos->block, catch_pos->block->statement_list, ob);
		generate_code(ob, try_s->line_number, DVM_GO_FINALLY,compiler->current_finally_label);
		generate_code(ob, try_s->line_number, DVM_JUMP,after_finally_label);
		dvm_try.catch_clause[catch_index].end_pc = ob->size - 1;
	}
	
	set_label(ob, compiler->current_finally_label);
	dvm_try.finally_start_pc = ob->size;
	if (try_s->u.try_s.finally_block) {
		generate_statement_list(exe, try_s->u.try_s.finally_block, try_s->u.try_s.finally_block->statement_list, ob);
	}
	dvm_try.finally_end_pc = ob->size - 1;
	set_label(ob, after_finally_label);
	
	ob->try = MEM_realloc(ob->try, (ob->try_szie + 1) * sizeof(dvm_try));
	ob->try[ob->try_szie] = dvm_try;
	ob->try_szie++;
	
	compiler->current_finally_label = backup_finally_label;
	
	
	
	

}


static void generate_throw_statement(DVM_Executable *exe, Block *block, Statement *throw_s, OpcodeBuf *ob){
	if (throw_s->u.throw_s.expression) {
		generate_expression(exe, block, throw_s->u.throw_s.expression, ob);
		generate_code(ob, throw_s->line_number, DVM_THROW);
	}else{
		generate_variable_identifier_expression(throw_s->u.throw_s.variable_declaration, ob, throw_s->line_number);
		generate_code(ob, throw_s->line_number, DVM_RETHROW);
	}
}

static void generate_decla_initializer(DVM_Executable *exe, Block *block, Statement *statement, OpcodeBuf *ob){
	Declaration *decla = statement->u.declaration_s;
	if (!decla->initializer) {
		return;
	}
	generate_expression(exe, block, decla->initializer, ob);
	genereate_pop_to_identifier(decla, statement->line_number, ob);
}


static void generate_statement_list(DVM_Executable *exe, Block *current_block,
									StatementList *statement_list, OpcodeBuf *ob){
	for (StatementList *statement_pos = statement_list; statement_pos; statement_pos = statement_pos->next) {
		Statement *statement = statement_pos->statement;
		switch (statement->type) {
			case EXPRESSION_STATEMENT:
				generate_expression_statement(exe, current_block, statement->u.expression_s, ob);
				break;
			case IF_STATEMENT:
				genereate_if_statement(exe, current_block, statement, ob);
				break;
			case SWITCH_STATEMENT:
				generate_switch_statement(exe, current_block, statement, ob);
				break;
			case FOR_STATEMENT:
				genereate_for_statement(exe, current_block, statement, ob);
				break;
			case FOREACH_STATEMENT:
				break;
			case WHILE_STATEMENT:
				generate_while_statement(exe, current_block, statement, ob);
				break;
			case DO_WHILE_STATEMENT:
				genereate_do_while_statement(exe, current_block, statement, ob);
				break;
			case RETURN_STATEMENT:
				generate_return_statement(exe, current_block, statement, ob);
				break;
			case BREAK_STATEMENT:
				generate_break_statement(exe, current_block, statement, ob);
				break;
			case CONTINUE_STATEMENT:
				generate_continue_statement(exe, current_block, statement, ob);
				break;
			case TRY_STATEMENT:
				generate_try_statement(exe, current_block, statement, ob);
				break;
			case THROW_STATEMENT:
				generate_throw_statement(exe, current_block, statement, ob);
			case DECLARATION_STATEMENT:{
				generate_decla_initializer(exe, current_block, statement, ob);
				break;
			}
			default:
				break;
		}
		
	}
	
	
}
static DVM_LocalVariable* mgc_copy_local_variable(FunctionDefinition *fd,size_t parameter_count);


static void add_function(DVM_Executable *exe, FunctionDefinition *fd, DVM_Function *dest, DVM_Boolean in_this_exe){
	dest->type = mgc_conver_type_specifier(fd->type);
	dest->parameter = conver_parameter_list(fd->parameter_list, &dest->parameter_count);
	
	if (in_this_exe && fd->block) {
		dest->is_implemented = DVM_TRUE;
		dest->local_variable_count = fd->local_variable_count - dest->parameter_count;
		dest->locak_variable = mgc_copy_local_variable(fd, dest->parameter_count);
		
		OpcodeBuf ob;
		init_code_buf(&ob);
		
		generate_statement_list(exe, fd->block, fd->block->statement_list, &ob);
		copy_opcode_buf(&dest->code_block, &ob);
		
		
	}else{
		dest->is_implemented = DVM_FALSE;
		dest->local_variable_count = 0;
		dest->locak_variable = NULL;
	}
	
	dest->is_method = fd->class_definition ? DVM_TRUE : DVM_FALSE;
	
	
	
}



static size_t index_of_function_define(MGC_Compiler *compiler, FunctionDefinition *fd){
	char *func_name;
	if (fd->class_definition) {
		func_name = dvm_create_method_function_name(fd->class_definition->name, fd->name);
	}else{
		func_name = fd->name;
	}
	char *src_package_name = mgc_package_name_to_string(fd->package_name);
	for (size_t i = 0; i < compiler->dvm_function_count; i++) {
		if (mgc_equal_string(src_package_name, compiler->dvm_function[i].package_name)
			&& !strcmp(func_name, compiler->dvm_function[i].name)) {
			MEM_free(src_package_name);
			if (fd->class_definition) {
				MEM_free(func_name);
			}
			return i;
		}
	}
	return -1;
}

static DVM_LocalVariable* mgc_copy_local_variable(FunctionDefinition *fd,size_t parameter_count){
	size_t local_var_count = fd->local_variable_count - parameter_count;
	DVM_LocalVariable *vars = MEM_malloc(sizeof(DVM_LocalVariable) * local_var_count);
	for (size_t i = 0; i < local_var_count; i++) {
		vars[i].name = MEM_strdup(fd->local_variable[i+parameter_count]->name);
		vars[i].type = mgc_conver_type_specifier(fd->local_variable[i + parameter_count]->type);
	}
	return vars;
}



static void add_functions(MGC_Compiler *compiler, DVM_Executable *exe){
	DVM_Boolean *in_this_exe = MEM_malloc(sizeof(DVM_Boolean) * compiler->dvm_function_count);
	size_t i = 0;
	for (i = 0; i < compiler->dvm_function_count; i++) {
		in_this_exe[i] = DVM_FALSE;
	}
	
	for (FunctionDefinition *fd = compiler->function_list; fd; fd = fd->next) {
		if (fd->class_definition && fd->block == NULL) {
			continue;
		}
		
		size_t index = index_of_function_define(compiler, fd);
		in_this_exe[index] = DVM_TRUE;
		add_function(exe, fd, &compiler->dvm_function[index], DVM_TRUE);
	}
	
	for (i = 0; i < compiler->dvm_function_count; i++) {
		if (in_this_exe[i]) {
			continue;
		}
		FunctionDefinition *fd = mgc_search_function(compiler->dvm_function[i].name);
		add_function(exe, fd, &compiler->dvm_function[i], DVM_FALSE);
	}
	
	MEM_free(in_this_exe);
	
	
}

static void add_global_variable(MGC_Compiler *compiler, DVM_Executable *exe){
	size_t count = 0;
	for (DeclarationList *pos = compiler->declaration_list; pos; pos = pos->next) {
		count++;
	}
	exe->global_variable_count = count;
	exe->global_variable = MEM_malloc(sizeof(DVM_Variable) * count);
	size_t i = 0;
	for (DeclarationList *pos = compiler->declaration_list ; pos; pos = pos->next,i++) {
		exe->global_variable[i].name = MEM_strdup(pos->declaration->name);
		exe->global_variable[i].type = mgc_conver_type_specifier(pos->declaration->type);
	}

}

static DVM_Class *search_class(MGC_Compiler *compiler, ClassDefinition *cd){
	char *src_parckage_name = mgc_package_name_to_string(cd->package_name);
	for (size_t i = 0; i < compiler->dvm_class_count; i++) {
		if (mgc_equal_string(compiler->dvm_class[i].package_name, src_parckage_name)
			 && !strcmp(compiler->dvm_class[i].name, cd->name)) {
			MEM_free(src_parckage_name);
			return &compiler->dvm_class[i];
		}
	}
	MEM_free(src_parckage_name);
	return NULL;
}



static void generate_field_initailizer(DVM_Executable *exe, ClassDefinition *cd, DVM_Class *dvm_class){
	OpcodeBuf ob;
	init_code_buf(&ob);
	for (ClassDefinition *cd_pos = cd; cd_pos; cd_pos = cd_pos->super_class) {
		for (MemberDeclaration *member_pos = cd_pos->member; member_pos; member_pos = member_pos->next) {
			if (member_pos->kind == METHOD_MEMBER) {
				continue;
			}
			
			if (member_pos->u.field.initializer) {
				generate_expression(exe, NULL, member_pos->u.field.initializer, &ob);
				generate_code(&ob, member_pos->u.field.initializer->line_number, DVM_DUPLICATE_OFFSET, 1);
				generate_code(&ob, member_pos->line_number, DVM_POP_FIELD_INT + get_opcode_type_offset(member_pos->u.field.type), member_pos->u.field.field_index);
				
			}
		}
	}
	
	copy_opcode_buf(&dvm_class->field_initializer, &ob);

}

static void add_class(DVM_Executable *exe, ClassDefinition *cd, DVM_Class *dest){

	dest->is_abstract = cd->is_abstract;
	dest->access_modifier = cd->access_modifier;
	dest->class_or_interface = cd->class_or_interface;
	
	if (cd->super_class) {
		DVM_ClassIdentifier *class_identifier = MEM_malloc(sizeof(DVM_ClassIdentifier));
		class_identifier->name = MEM_strdup(cd->super_class->name);
		class_identifier->package_name = mgc_package_name_to_string(cd->package_name);
		dest->super_class = class_identifier;
	}else{
		dest->super_class = NULL;
	}
	
	
	size_t interface_count = 0;
	ExtendsList *interface_pos = cd->interface_list;
	for (; interface_pos; interface_pos = interface_pos->next) {
		interface_count++;
	}
	
	dest->interface_count = interface_count;
	dest->interface_ = MEM_malloc(sizeof(DVM_ClassIdentifier) * interface_count);
	size_t i = 0;
	for (interface_pos = cd->interface_list; interface_pos; interface_pos = interface_pos->next,i++) {
		dest->interface_[i].name = MEM_strdup(interface_pos->class_definition->name);
		dest->interface_[i].package_name = mgc_package_name_to_string(interface_pos->class_definition->package_name);
	}
	
	
	size_t field_count = 0;
	size_t method_count = 0;
	MemberDeclaration *member_pos = cd->member;
	for (; member_pos; member_pos = member_pos->next) {
		if (member_pos->kind == METHOD_MEMBER) {
			method_count++;
		}else if (member_pos->kind == FIELD_MEMBER){
			field_count++;
		}
	}
	
	dest->field_count = field_count;
	dest->field = MEM_malloc(sizeof(DVM_Field) * field_count);
	
	dest->method_count = method_count;
	dest->method = MEM_malloc(sizeof(DVM_Method) * method_count);
	
	size_t field_index = 0;
	size_t method_index = 0;
	
	for (member_pos = cd->member; member_pos; member_pos = member_pos->next) {
		if (member_pos->kind == METHOD_MEMBER) {
			dest->method[method_index].access_modifier = member_pos->access_modifier;
			dest->method[method_index].is_abstract = member_pos->u.method.is_abstract;
			dest->method[method_index].is_virtual = member_pos->u.method.is_virtual;
			dest->method[method_index].is_override = member_pos->u.method.is_override;
			dest->method[method_index].name = MEM_strdup(member_pos->u.method.function_definition->name);
			method_index++;
		}else if (member_pos->kind == FIELD_MEMBER){
			dest->field[field_index].access_modifier = member_pos->access_modifier;
			dest->field[field_index].name = MEM_strdup(member_pos->u.field.name);
			dest->field[field_index].type = mgc_conver_type_specifier(member_pos->u.field.type);
			field_index++;
		}
	}
	
	

}


static void add_classes(MGC_Compiler *compiler, DVM_Executable *exe){
	for (ClassDefinition *cd_pos = compiler->class_definition_list; cd_pos; cd_pos = cd_pos->next) {
		DVM_Class *class = search_class(compiler, cd_pos);
		class->is_implemented = DVM_TRUE;
		generate_field_initailizer(exe, cd_pos, class);
	}
	
	for (size_t i = 0; i < compiler->dvm_class_count; i++) {
		ClassDefinition *cd = mgc_search_class(compiler->dvm_class[i].name);
		add_class(exe, cd, &compiler->dvm_class[i]);
		
	}
	
}




static void generate_top_level(MGC_Compiler *compiler, DVM_Executable *exe){
	OpcodeBuf ob;
	init_code_buf(&ob);
	generate_statement_list(exe, NULL, compiler->statement_list, &ob);
	
	
	copy_opcode_buf(&exe->top_level, &ob);
	
	
}

static void generate_constant_initailizer(MGC_Compiler *compiler, DVM_Executable *exe){
	OpcodeBuf ob;
	init_code_buf(&ob);
	for (ConstantDefinition *const_pos = compiler->constant_definition_list; const_pos; const_pos = const_pos->next) {
		if (const_pos->initializer) {
			generate_expression(exe, NULL, const_pos->initializer, &ob);
			generate_code(&ob, const_pos->line_number, DVM_POP_CONSTANT_INT + get_opcode_type_offset(const_pos->type), const_pos->index);
		}
	}
	
	copy_opcode_buf(&exe->constant_initializer,&ob);

};
	
	

DVM_Executable *mgc_generate(MGC_Compiler *compiler){
	DVM_Executable *exe =  alloc_executable(compiler->package_name);
	exe->function = compiler->dvm_function;
	exe->function_count = compiler->dvm_function_count;
	exe->class_definition = compiler->dvm_class;
	exe->class_count = compiler->dvm_class_count;
	exe->enum_definition = compiler->dvm_enum;
	exe->enum_count = compiler->dvm_enum_count;
	exe->constant_definition = compiler->dvm_constant;
	exe->constant_count = compiler->dvm_constant_count;
	
	add_global_variable(compiler, exe);
	add_classes(compiler, exe);
	add_functions(compiler, exe);
	generate_top_level(compiler, exe);
	generate_constant_initailizer(compiler,exe);
	
	
	
	return exe;
}

