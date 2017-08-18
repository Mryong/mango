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




static void generate_code(OpcodeBuf *ob, int line_number, DVM_Opcode code, ...){
	va_list ap;
	va_start(ap, code);
	char *parameter = dvm_opcode_info[code].parameter;
	if (ob->alloc_size < ob->size + 1 + strlen(parameter) * 2) {
		ob->alloc_size += OPCODE_ALLOC_SIZE;
		ob->code = MEM_realloc(ob->code,  ob->alloc_size);
	}
	size_t start_pc = ob->size;
	ob->code[ob->size] = code;
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

}



static void generate_boolean_expression(DVM_Executable *exe, Expression *expr, OpcodeBuf *ob){
	if (expr->u.boolean_value) {
		
	}else{
	
	}
}


static void generate_assign_expression(DVM_Executable *exe, Block *block, Expression *expr, OpcodeBuf *ob){
	switch (expr->kind) {
		case BOOLEAN_EXPRESSION:
			
			
			break;
			
  default:
			break;
	}

}

static void genereate_expression(DVM_Executable *exe, Block *block, Expression *expr, OpcodeBuf *ob){
	

}

static void generate_expression_statement(DVM_Executable *exe, Block *block, Expression *expr, OpcodeBuf *ob){
	if (expr->kind == ASSIGN_EXPRESSION) {
		
	}else if (expr->kind == INCREMENT_EXPRESSION
			  || expr->kind == DECREMENT_EXPRESSION){
	
	}else{
		
	}

}

static void generate_statement_list(DVM_Executable *exe, Block *current_block, StatementList *statement_list, OpcodeBuf *ob){
	for (StatementList *statement_pos = statement_list; statement_pos; statement_pos = statement_pos->next) {
		 Statement *statement = statement_pos->statement;
		switch (statement->type) {
			case EXPRESSION_STATEMENT:
				
		break;
				
			default:
		break;
		}
		
	}
	

}




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

static DVM_LocalVariable* copy_local_variable(FunctionDefinition *fd,size_t parameter_count){
	size_t local_var_count = fd->local_variable_count - parameter_count;
	DVM_LocalVariable *vars = MEM_malloc(sizeof(DVM_LocalVariable) * local_var_count);
	for (size_t i = 0; i < local_var_count; i++) {
		vars[i].name = MEM_strdup(fd->local_variable[i+parameter_count]->name);
		vars[i].type = mgc_conver_type_specifier(fd->local_variable[i + parameter_count]->type);
	}
	return vars;
}

static void add_function(DVM_Executable *exe, FunctionDefinition *fd, DVM_Function *dest, DVM_Boolean in_this_exe){
	dest->type = mgc_conver_type_specifier(fd->type);
	dest->parameter = conver_parameter_list(fd->parameter_list, &dest->parameter_count);
	
	if (in_this_exe && fd->block) {
		dest->is_implemented = DVM_FALSE;
		dest->local_variable_count = fd->local_variable_count - dest->parameter_count;
		dest->locak_variable = copy_local_variable(fd, dest->parameter_count);
		
		
	}else{
		dest->is_implemented = DVM_FALSE;
		dest->local_variable_count = 0;
		dest->locak_variable = NULL;
	}
	
	dest->is_method = fd->class_definition ? DVM_TRUE : DVM_FALSE;
	
	

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



static void generate_field_initailizer(DVM_Executable *exe, ClassDefinition *cd, DVM_Class *dvm_class){}

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
	
}

static void generate_constant_initailizer(MGC_Compiler *compiler, DVM_Executable *exe){

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

