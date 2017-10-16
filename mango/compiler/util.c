//
//  util.c
//  mango
//
//  Created by jerry.yong on 2017/6/5.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mangoc.h"
#include "DVM.h"
#include "share.h"
#include "DBG.h"

static MGC_Compiler *st_current_Compiler;


MGC_Compiler *mgc_get_current_compiler(void){
	return st_current_Compiler;
}

void mgc_set_current_compiler(MGC_Compiler *compiler){
	st_current_Compiler = compiler;
}

void *mgc_malloc(size_t size){
	MGC_Compiler *compler = mgc_get_current_compiler();
	return  MEM_malloc(size);
}



char *mgc_package_name_to_string(PackageName *package_name){
	
	if (package_name == NULL) {
		return NULL;
	}
	size_t len = 0;
	PackageName *fragment = package_name;
	while (fragment != NULL) {
		len += (strlen(fragment->name) + 1);
		fragment = fragment->next;
	}
	
	char *ret_val = MEM_malloc(len);
	fragment = package_name;
	ret_val[0] = '\0';
	while (fragment != NULL) {
		strcat(ret_val, fragment->name);
		if (fragment->next) {
			strcat(ret_val, ".");
		}
		fragment = fragment->next;
	}
	return ret_val;
}

DVM_Boolean mgc_equal_parameter(ParameterList *param1, ParameterList *param2){
	ParameterList *pos1;
	ParameterList *pos2;
	for (pos1 = param1, pos2 = param2; pos1 && pos2; pos1 = pos1->next, pos2 = pos2->next) {
		if (!strcmp(pos1->name, pos2->name)) {
			return DVM_FALSE;
		}
		
		if (!mgc_equal_type(pos1->type, pos2->type)) {
			return DVM_FALSE;
		}
	}
	
	if (pos1 || pos2) {
		return DVM_FALSE;
	}
	return DVM_TRUE;
}

DVM_Boolean mgc_equal_string(char *str1, char *str2){
	if (str1 == NULL && str2 == NULL) {
		return DVM_TRUE;
	}
	
	if (str1 == NULL || str2 == NULL) {
		return DVM_FALSE;
	}
	
	return !strcmp(str1, str2);
}


DVM_Boolean mgc_equal_type(TypeSpecifier *type1, TypeSpecifier *type2){
	
	if (type1->base_type != type2->base_type) {
		return DVM_FALSE;
	}
	
	if (type1->base_type == DVM_CLASS_TYPE) {
		if (type1->u.class_ref.class_definition != type2->u.class_ref.class_definition) {
			return DVM_FALSE;
		}
	}
	
	if (type1->base_type == DVM_ENUM_TYPE) {
		if (type1->u.enum_ref.enum_definition != type2->u.enum_ref.enum_definition) {
			return DVM_FALSE;
		}
	}
	
	if (type1->base_type == DVM_DELEGAET_TYPE) {
		if (type1->u.delegate_ref.delegate_definition != type2->u.delegate_ref.delegate_definition) {
			return DVM_FALSE;
		}
	}
	
	TypeDerive *td1 = NULL;
	TypeDerive *td2 = NULL;
	for (td1 = type1->derive, td2 = type2->derive; td1 && td2; td1 = td1->next, td2 = td2->next) {
		if (td1->tag != td2->tag) {
			return DVM_FALSE;
		}
		
		if (td1->tag == FUNCTION_DERIVE) {
			if (!mgc_equal_parameter(td1->u.function_d.parameter_list, td2->u.function_d.parameter_list)) {
				return DVM_FALSE;
			}
		}
	}
	
	if (td1 || td2) {
		return DVM_FALSE;
	}
	
	return DVM_TRUE;
}


DVM_Boolean mgc_equal_package_name(PackageName *package_name1, PackageName *package_name2){
	
	PackageName *last1 = package_name1;
	PackageName *last2 = package_name2;
	
	while (last1 && last2) {
		if (strcmp(last1->name, last2->name) != 0) {
			return DVM_FALSE;
		}
		last1 = last1->next;
		last2 = last2->next;
	}
	
	if (last1 || last2) {
		return DVM_FALSE;
	}
	
	return DVM_TRUE;
	
	
	
	
	
}
static FunctionDefinition *search_rename_function(MGC_Compiler *compiler, RenameList *rename){
	CompilerList *compiler_pos = compiler->required_list;
	while (compiler_pos) {
		if (!mgc_equal_package_name(compiler_pos->compiler->package_name, rename->package_name)) {
			
			compiler_pos = compiler_pos->next;
			continue;
		}
		
		for (FunctionDefinition *func_pos = compiler_pos->compiler->function_list; func_pos; func_pos = func_pos->next) {
			if (!strcmp(func_pos->name, rename->original_name) && func_pos->class_definition == NULL) {
				return func_pos;
			}
		}
		
	}
	
	return NULL;
	
}


FunctionDefinition *mgc_search_function(char *name){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	
	for (FunctionDefinition	*fun_pos = compiler->function_list; fun_pos; fun_pos = fun_pos->next) {
		if (!strcmp(fun_pos->name, name) && fun_pos->class_definition == NULL) {
			return fun_pos;
		}
	}
	
	for (RenameList	*rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, name)) {
			FunctionDefinition *fd = search_rename_function(compiler, rename_pos);
			if (fd) {
				return fd;
			}
		}
	}
	
	for (CompilerList *complier_pos = compiler->required_list; complier_pos; complier_pos = complier_pos->next) {
		for (FunctionDefinition *fun_pos = complier_pos->compiler->function_list; fun_pos; fun_pos = fun_pos->next) {
			if (!strcmp(fun_pos->name, name) && fun_pos->class_definition == NULL) {
				return fun_pos;
			}
		}
	}
	
	return NULL;
}

Declaration *mgc_search_declaration(char *identifier, Block *block){
	for (Block *block_pos = block; block_pos; block_pos = block->out_block) {
		for (DeclarationList *declaration_pos = block_pos->declaration_list; declaration_pos; declaration_pos = declaration_pos->next) {
			if (!strcmp(declaration_pos->declaration->name, identifier)) {
				return declaration_pos->declaration;
			}
		}
	}
	
	for (DeclarationList *declaration_pos = mgc_get_current_compiler()->declaration_list; declaration_pos; declaration_pos = declaration_pos->next) {
		if (!strcmp(declaration_pos->declaration->name, identifier)) {
			return declaration_pos->declaration;
		}
	}
	
	return NULL;
}


static ConstantDefinition *search_renamed_constant(MGC_Compiler *compiler, RenameList *rename){
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		if (!mgc_equal_package_name(compiler_pos->compiler->package_name, rename->package_name)) {
			continue;
		}
		
		for (ConstantDefinition *const_pos = compiler_pos->compiler->constant_definition_list; compiler_pos; compiler_pos = compiler_pos->next) {
			if (!strcmp(const_pos->name, rename->original_name)) {
				return const_pos;
			}
		}
	}
	
	return NULL;
}


ConstantDefinition *mgc_search_constant(char *identifier){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	
	for (ConstantDefinition * const_pos = compiler->constant_definition_list; const_pos; const_pos = const_pos->next) {
		if (!strcmp(const_pos->name, identifier)) {
			return const_pos;
		}
	}
	
	for (RenameList *rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, identifier)) {
			ConstantDefinition *cd = search_renamed_constant(compiler, rename_pos);
			if (cd) {
				return cd;
			}
		}
		
	}
	
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		for (ConstantDefinition *const_pos = compiler_pos->compiler->constant_definition_list; const_pos; const_pos = const_pos->next) {
			if (!strcmp(const_pos->name, identifier)) {
				return const_pos;
			}
		}
	}
	return NULL;
}


static ClassDefinition *search_rename_class(MGC_Compiler *compiler,RenameList *rename){
	for (CompilerList *compile_pos = compiler->required_list; compile_pos; compile_pos = compile_pos->next) {
		if (!mgc_equal_package_name(compile_pos->compiler->package_name, rename->package_name)) {
			continue;
		}
		
		for (ClassDefinition *class_pos = compile_pos->compiler->class_definition_list; class_pos; class_pos = class_pos->next) {
			if (!strcmp(class_pos->name, rename->original_name)) {
				return class_pos;
			}
		}
	}
	
	return NULL;
	
}



ClassDefinition *mgc_search_class(char *identifier){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	for (ClassDefinition *class_pos = compiler->class_definition_list; class_pos; class_pos = class_pos->next) {
		if (!strcmp(class_pos->name, identifier)) {
			return class_pos;
		}
	}
	
	for (RenameList *rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, identifier)) {
			ClassDefinition *cd = search_rename_class(compiler, rename_pos);
			if (cd) {
				return cd;
			}
		}
	}
	
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		for (ClassDefinition *class_pos = compiler_pos->compiler->class_definition_list; class_pos; class_pos = class_pos->next) {
			if (!strcmp(class_pos->name, identifier)) {
				return class_pos;
			}
		}
	}
	
	return NULL;
	
}


static DelegateDefinition *search_rename_delegate(MGC_Compiler *compiler, RenameList *rename){
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		if (!mgc_equal_package_name(compiler_pos->compiler->package_name, rename->package_name)) {
			continue;
		}
		
		for (DelegateDefinition *delegate_pos = compiler_pos->compiler->delegate_definition_list; delegate_pos; delegate_pos = delegate_pos->next) {
			if (!strcmp(delegate_pos->name, rename->original_name)) {
				return delegate_pos;
			}
		}
	}
	return NULL;
}


DelegateDefinition *mgc_search_delegate(char *identifier){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	for (DelegateDefinition *delegate_pos = compiler->delegate_definition_list; delegate_pos; delegate_pos = delegate_pos->next) {
		if (!strcmp(delegate_pos->name, identifier)) {
			return delegate_pos;
		}
	}
	
	for (RenameList *rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, identifier)) {
			DelegateDefinition *dd = search_rename_delegate(compiler, rename_pos);
			if (dd) {
				return dd;
			}
		}
	}
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		for (DelegateDefinition *delegate_pos = compiler_pos->compiler->delegate_definition_list; delegate_pos; delegate_pos = delegate_pos->next) {
			if (!strcmp(delegate_pos->name, identifier)) {
				return delegate_pos;
			}
		}
	}
	
	return NULL;
	
}


static EnumDefinition *search_rename_enum(MGC_Compiler *compiler, RenameList *rename){
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		if (!mgc_equal_package_name(compiler_pos->compiler->package_name, rename->package_name)) {
			continue;
		}
		
		for (EnumDefinition *enum_pos = compiler_pos->compiler->enum_definition_list; enum_pos; enum_pos = enum_pos->next) {
			if (!strcmp(enum_pos->name, rename->original_name)) {
				return enum_pos;
			}
		}
		
		
	}
	
	return NULL;
}

EnumDefinition *mgc_search_enum(char *identifier){
	MGC_Compiler *compiler = mgc_get_current_compiler();
	for (EnumDefinition *enum_pos = compiler->enum_definition_list; enum_pos; enum_pos = enum_pos->next) {
		if (!strcmp(enum_pos->name, identifier)) {
			return enum_pos;
		}
	}
	
	for (RenameList *rename_pos = compiler->rename_list; rename_pos; rename_pos = rename_pos->next) {
		if (!strcmp(rename_pos->renamed_name, identifier)) {
			EnumDefinition *ed = search_rename_enum(compiler,rename_pos);
			if (ed) {
				return ed;
			}
		}
	}
	
	for (CompilerList *compiler_pos = compiler->required_list; compiler_pos; compiler_pos = compiler_pos->next) {
		for (EnumDefinition *enum_pos = compiler_pos->compiler->enum_definition_list; enum_pos; enum_pos = enum_pos->next) {
			if (!strcmp(enum_pos->name, identifier)) {
				return enum_pos;
			}
		}
	}
	
	return NULL;
}


MemberDeclaration *mgc_search_member(ClassDefinition *class_def,char *member_name){
	MemberDeclaration *member;
	for (member = class_def->member; member; member = member->next) {
		if (member->kind == METHOD_MEMBER) {
			if (!strcmp(member->u.method.function_definition->name, member_name)) {
				break;
			}
			
		}else if (member->kind == FIELD_MEMBER){
			if (!strcmp(member->u.field.name, member_name)) {
				break;
			}
		}else{
			
		}
	}
	
	if (member) {
		return member;
	}
	
	if (class_def->super_class) {
		member = mgc_search_member(class_def->super_class, member_name);
	}
	
	if (member) {
		return member;
	}
	
	for (ExtendsList *extends_p = class_def->interface_list; extends_p; extends_p = extends_p->next) {
		member = mgc_search_member(extends_p->class_definition,member_name);
		if (member) {
			return member;
		}
	}
	
	return NULL;
}

TypeSpecifier *mgc_alloc_type_specifier(DVM_BaseType type){
	TypeSpecifier *ts = mgc_malloc(sizeof(*ts));
	ts->base_type = type;
	ts->line_number = 0;
	ts->derive = NULL;
	if (type == DVM_CLASS_TYPE) {
		ts->identifier = NULL;
		ts->u.class_ref.class_definition = NULL;
	}
	return ts;
}

TypeSpecifier *mgc_copy_type_specifier(TypeSpecifier *type){
	TypeSpecifier *ts = mgc_malloc(sizeof(TypeSpecifier));
	*ts = *type;
	return ts;
}

TypeDerive *mgc_alloc_type_derive(DeriveTag derive_tag){
	TypeDerive *type_derive = mgc_malloc(sizeof(*type_derive));
	type_derive->tag = derive_tag;
	type_derive->next = NULL;
	return type_derive;
}





void mgc_vstr_clear(VString *v){
	v->string = NULL;
}

static size_t my_strlen(char *str){
	if (str == NULL) {
		return 0;
	}
	return strlen(str);
}


void mgc_vstr_append_string(VString *v, char *str){
	size_t old_size = my_strlen(v->string);
	size_t new_size = old_size + strlen(str) + 1;
	v->string = MEM_realloc(v->string, new_size);
	strcpy(&v->string[old_size], str);
}

void mgc_vstr_append_ch(VString *v, char ch){
	size_t old_size = my_strlen(v->string);
	v->string = MEM_realloc(v->string, old_size + 2);
	v->string[old_size] = ch;
	v->string[old_size + 1] = '\0';

}

static size_t my_wcslen(DVM_Char *str){
	if (str == NULL) {
		return 0;
	}
	return  dvm_wcslen(str);
}


void mgc_vwstr_append_string(VWString *v, DVM_Char *str){
	size_t old_len = my_wcslen(v->string);
	size_t new_size = sizeof(DVM_Char) * (old_len + dvm_wcslen(str) + 1);
	v->string = MEM_realloc(v->string, new_size);
	dvm_wcscmp(&v->string[old_len], str);

}


void mgc_vwstr_append_character(VWString *v, DVM_Char ch){
	size_t len = my_wcslen(v->string);
	v->string = MEM_realloc(v->string, sizeof(DVM_Char) * (len + 2));
	v->string[len] = ch;
	v->string[len + 1] = L'\0';

}


char *mgc_get_base_type_name(DVM_BaseType type){
	switch (type) {
		case DVM_VOID_TYPE:
			return "void";
			break;
		case DVM_BOOLEAN_TYPE:
			return "boolean";
			break;
		case DVM_INT_TYPE:
			return "int";
			break;
		case DVM_DOUBLE_TYPE:
			return "double";
			break;
		case DVM_STRING_TYPE:
			return "string";
			break;
		case DVM_NATIVE_POINTER_TYPE:
			return "native_pointer";
			break;
		case DVM_CLASS_TYPE:
			return "class";
			break;
		case DVM_NULL_TYPE:
			return "null";
			break;
		case DVM_DELEGAET_TYPE:
		case DVM_ENUM_TYPE:
		case DVM_BASE_TYPE:
		case DVM_UNSPECIFIED_IDENTIFIER_TYPE:
		default:
			DBG_assert(0, "bad case. type..%d\n",type);
			
	}
	
	return NULL;
}



DVM_Char *mgc_expression_to_string(Expression *expr){
	DVM_Char wc_buf[LINE_BUF_SIZE];
	char buf[LINE_BUF_SIZE];
	
	if (expr->kind == BOOLEAN_EXPRESSION) {
		if (expr->u.boolean_value) {
			dvm_mbstowcs("true", wc_buf);
		}else{
			dvm_mbstowcs("false", wc_buf);
		}
	}else if (expr->kind == INT_EXPRESSION){
		sprintf(buf, "%d",expr->u.int_value);
		dvm_mbstowcs(buf, wc_buf);
	}else if (expr->kind == DOUBLE_EXPRESSION){
		sprintf(buf, "%lf", expr->u.double_value);
		dvm_mbstowcs(buf, wc_buf);
	}else if (expr->kind == STRING_EXPRESSION){
		return expr->u.string_value;
	}else{
		return NULL;
	}
	
	size_t len = dvm_wcslen(wc_buf);
	DVM_Char *new_str = MEM_malloc(sizeof(DVM_Char) * (len + 1));
	dvm_wcscpy(new_str, wc_buf);
	return new_str;
}

static void function_type_to_string(VString *vstr, TypeDerive *derive){
	mgc_vstr_append_string(vstr, "(");
	for (ParameterList *pos = derive->u.function_d.parameter_list; pos; pos = pos->next) {
		char *type_name = mgc_get_type_name(pos->type);
		mgc_vstr_append_string(vstr, type_name);
		mgc_vstr_append_string(vstr, " ");
		mgc_vstr_append_string(vstr, pos->name);
		if (pos->next) {
			mgc_vstr_append_string(vstr, ",");
		}
	}
	mgc_vstr_append_string(vstr, ")");
	
	if (derive->u.function_d.throws) {
		mgc_vstr_append_string(vstr, " throws ");
		for (ExceptionList *pos = derive->u.function_d.throws; pos; pos = pos->next) {
			mgc_vstr_append_string(vstr, pos->exception->identifer);
			if (pos->next) {
				mgc_vstr_append_string(vstr, ",");
			}
		}
	}
}


char *mgc_get_type_name(TypeSpecifier *type){
	VString vstr;

	if (type->base_type == DVM_CLASS_TYPE
		|| type->base_type == DVM_ENUM_TYPE
		|| type->base_type == DVM_DELEGAET_TYPE) {
		mgc_vstr_append_string(&vstr, type->identifier);
	}else{
		mgc_vstr_append_string(&vstr, mgc_get_base_type_name(type->base_type));
	}
	
	for (TypeDerive *pos = type->derive; pos; pos = pos->next) {
		if (pos->tag == FUNCTION_DERIVE) {
			function_type_to_string(&vstr, pos);
		}else if (pos->tag == ARRAY_DERIVE){
			mgc_vstr_append_string(&vstr, "[]");
		}else{
			DBG_assert(0, "derive_tag ..%d\n",pos->tag);
		}
	}
	
	return vstr.string;
	
}























