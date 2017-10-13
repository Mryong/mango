//
//  opcode.c
//  mango
//
//  Created by jerry.yong on 2017/8/18.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "share.h"
#include "DVM.h"



DVM_ObjectRef dvm_null_object_ref = {NULL,NULL};


OpcodeInfo dvm_opcode_info[] = {
	{"dummy", "", 0},
	{"push_int_1byte", "b", 1},
	{"push_int_2byte", "s", 1},
	{"push_int", "p", 1},
	{"push_double_0", "", 1},
	{"push_double_1", "", 1},
	{"push_double", "p", 1},
	{"push_string", "p", 1},
	{"push_null", "", 1},
	/**********/
	{"push_stack_int", "s", 1},
	{"push_stack_double", "s", 1},
	{"push_stack_object", "s", 1},
	{"pop_stack_int", "s", -1},
	{"pop_stack_double", "s", -1},
	{"pop_stack_object", "s", -1},
	/**********/
	{"push_static_int", "s", 1},
	{"push_static_double", "s", 1},
	{"push_static_object", "s", 1},
	{"pop_static_int", "s", -1},
	{"pop_static_double", "s", -1},
	{"pop_static_object", "s", -1},
	/**********/
	{"push_constant_int", "s", 1},
	{"push_constant_double", "s", 1},
	{"push_constant_object", "s", 1},
	{"pop_constant_int", "s", -1},
	{"pop_constant_double", "s", -1},
	{"pop_constant_object", "s", -1},
	/**********/
	{"push_array_int", "", 1},
	{"push_array_double", "", 1},
	{"push_array_object", "", 1},
	{"pop_array_int", "", -1},
	{"pop_array_double", "", -1},
	{"pop_array_object", "", -1},
	/**********/
	{"push_character_in_string", "", -1},
	/**********/
	{"push_field_int", "s", 1},
	{"push_field_double", "s", 1},
	{"push_field_object", "s", 1},
	{"pop_field_int", "s", -1},
	{"pop_field_double", "s", -1},
	{"pop_field_object", "s", -1},
	/**********/
	{"add_int", "", -1},
	{"add_double", "", -1},
	{"add_string", "", -1},
	{"sub_int", "", -1},
	{"sub_double", "", -1},
	{"mul_int", "", -1},
	{"mul_double", "", -1},
	{"div_int", "", -1},
	{"div_double", "", -1},
	{"mod_int", "", -1},
	{"mod_double", "", -1},
	{"bit_and", "", -1},
	{"bit_or", "", -1},
	{"bit_xor", "", -1},
	{"minus_int", "", 0},
	{"minus_double", "", 0},
	{"bit_not", "", 0},
	{"increment", "", 0},
	{"decrement", "", 0},
	{"cast_int_to_double", "", 0},
	{"cast_double_to_int", "", 0},
	{"cast_boolean_to_string", "", 0},
	{"cast_int_to_string", "", 0},
	{"cast_double_to_string", "", 0},
	{"cast_enum_to_string", "s", 0},
	{"up_cast", "s", 0},
	{"down_cast", "s", 0},
	{"eq_int", "", -1},
	{"eq_double", "", -1},
	{"eq_object", "", -1},
	{"eq_string", "", -1},
	{"gt_int", "", -1},
	{"gt_double", "", -1},
	{"gt_string", "", -1},
	{"ge_int", "", -1},
	{"ge_double", "", -1},
	{"ge_string", "", -1},
	{"lt_int", "", -1},
	{"lt_double", "", -1},
	{"lt_string", "", -1},
	{"le_int", "", -1},
	{"le_double", "", -1},
	{"le_string", "", -1},
	{"ne_int", "", -1},
	{"ne_double", "", -1},
	{"ne_object", "", -1},
	{"ne_string", "", -1},
	{"logical_and", "", -1},
	{"logical_or", "", -1},
	{"logical_not", "", 0},
	{"pop", "", -1},
	{"duplicate", "", 1},
	{"duplicate_offset", "s", 1},
	{"jump", "s", 0},
	{"jump_if_true", "s", -1},
	{"jump_if_false", "s", -1},
	/**********/
	{"push_function", "s", 1},
	{"push_method", "s", 1},
	{"push_delegate", "s", 1},
	{"push_method_delegate", "s", 1},
	{"invoke", "", -1},
	{"invoke_delegate", "", -1},
	{"return", "", -1},
	/**********/
	{"new", "s", 1},
	{"new_array", "bs", 0},
	{"new_array_literal_int", "s", 1},
	{"new_array_literal_double", "s", 1},
	{"new_array_literal_object", "s", 1},
	{"super", "", 0},
	{"instanceof", "s", 0},
	/**********/
	{"throw", "", -1},
	{"rethrow", "", -1},
	{"go_finally", "s", 1},
	{"finally_end", "", -1},
};
