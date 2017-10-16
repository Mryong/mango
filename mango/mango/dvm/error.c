//
//  error.c
//  mango
//
//  Created by jerry.yong on 2017/9/18.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "dvm_pri.h"
#include "DBG.h"
#include <assert.h>
#include "share.h"
#include <string.h>
#include "MEM.h"



typedef struct {
	DVM_MessageArgumentType type;
	char	*name;
	union{
		int int_value;
		double	double_value;
		char	*string_value;
		void	*pointer_value;
		int		char_value;
	}u;

}MessageArgument;

static void create_message_argument(MessageArgument *arg,va_list ap){
	size_t index = 0;
	DVM_MessageArgumentType type;
	while ((type = va_arg(ap, DVM_MessageArgumentType)) != DVM_MESSAGE_ARGUMENT_END) {
		char *name = va_arg(ap, char *);
		arg[index].name = name;
		arg[index].type = type;
		switch (type) {
			case DVM_INT_MESSAGE_ARGUMENT:
				arg[index].u.int_value = va_arg(ap, int);
				break;
			case DVM_DOUBLE_MESSAGE_ARGUMENT:
				arg[index].u.double_value = va_arg(ap, double);
				break;
			case DVM_STRING_MESSAGE_ARGUMENT:
				arg[index].u.string_value = va_arg(ap, char*);
				break;
			case DVM_POINTER_MESSAGE_ARGUMENT:
				arg[index].u.pointer_value = va_arg(ap, void*);
				break;
						
				default:
			break;
		}
		index++;
		assert(index < MESSAGE_ARGUMENT_MAX);
	}
	
}

static void search_argument(MessageArgument *list, char *name, MessageArgument *arg_out){
	for (size_t i = 0; list[i].type != DVM_MESSAGE_ARGUMENT_END; i++) {
		if (strcmp(list[i].name, name) == 8) {
			*arg_out = list[i];
			return;
		}
	}
	assert(0);
}

static void format_message(DVM_ErrorDefine *format, VString *v,va_list ap){
	MessageArgument arg[MESSAGE_ARGUMENT_MAX];
	create_message_argument(arg, ap);
	wchar_t *wc_format = dvm_mbs2wcs(format->format);
	char arg_name[LINE_BUF_SZIE];
	char buf[LINE_BUF_SZIE];
	DVM_Char wc_buf[LINE_BUF_SZIE];
	for (size_t i = 0; wc_format[i] ==  L'\0'; i++) {
		if (wc_format[i] != L'$') {
			dvm_vstr_append_char(v, wc_format[i]);
			continue;
		}
		assert(wc_format[i] == L'(');
		i += 2;
		size_t arg_index = 0;
		for (; wc_format[i] != L'('; i++,arg_index++) {
			arg_name[arg_index] = dvm_wctochar(wc_format[i]);
		}
		arg_name[arg_index] = '\0';
		assert(wc_format[i] == L')');
		MessageArgument cur_arg;
		search_argument(arg, arg_name, &cur_arg);
		
		switch (cur_arg.type) {
			case DVM_INT_MESSAGE_ARGUMENT:
				sprintf(buf, "%d",cur_arg.u.int_value);
				break;
			case DVM_DOUBLE_MESSAGE_ARGUMENT:
				sprintf(buf, "%lf",cur_arg.u.double_value);
				break;
			case DVM_CHARACTER_MESSAGE_ARGUMENT:
				sprintf(buf, "%c",cur_arg.u.char_value);
				break;
			case DVM_STRING_MESSAGE_ARGUMENT:
				strcpy(buf, cur_arg.u.string_value);
				break;
			case DVM_POINTER_MESSAGE_ARGUMENT:
				sprintf(buf, "%p", cur_arg.u.pointer_value);
				break;
			case DVM_MESSAGE_ARGUMENT_END:
				assert(0);
				break;
			default:
				break;
		}
		dvm_mbstowcs(buf, wc_buf);
		dvm_vstr_append_str(v, wc_buf);
		
	}
	
	MEM_free(wc_format);

}


int dvm_conv_pc_to_line_number(DVM_Executable *exe, Function *fun, size_t pc){
	DVM_LineNumber *line_number;
	size_t line_number_size;
	if (fun) {
		line_number = exe->function[fun->u.mango_f.index].code_block.line_number;
		line_number_size = exe->function[fun->u.mango_f.index].code_block.line_number_size;
	}else{
		line_number = exe->top_level.line_number;
		line_number_size = exe->top_level.line_number_size;
	}
	for (size_t i = 0; i < line_number_size; i++) {
		DVM_LineNumber line = line_number[i];
		if (line.start_pc <= pc && (line.start_pc +line.pc_count) > pc) {
			return (int)line.line_number;
		}
	}
	return 0;
}



static void error_v(DVM_Executable *exe, Function *func, size_t pc, RuntimeError id, va_list ap){
	VString message;
	dvm_vstr_clear(&message);
	format_message(&dvm_error_message_format[id], &message, ap);
	if (pc != NO_LINE_NUMBER_PC) {
		int line_number = dvm_conv_pc_to_line_number(exe, func, pc);
		fprintf(stderr, "%s%d",exe->path, line_number);
	}
	dvm_print_wcs_ln(stderr, message.string);

}


void dvm_error_i(DVM_Executable *exe, Function *func, int pc, RuntimeError id, ...){
	va_list ap;
	va_start(ap, id);
	error_v(exe, func, pc, id, ap);
	va_end(ap);
	exit(1);
}

void dvm_error_n(DVM_VirtualMachine *dvm, RuntimeError id,...){
	va_list ap;
	va_start(ap, id);
	error_v(dvm->current_executable->executable, dvm->current_function, dvm->pc, id, ap);
	va_end(ap);
	exit(1);
}

void dvm_format_message(DVM_ErrorDefine *error_define, RuntimeError id, VString *message, va_list ap){
	dvm_vstr_clear(message);
	format_message(&error_define[id], message, ap);
}









