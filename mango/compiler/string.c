//
//  string.c
//  mango_preview
//
//  Created by jerry.yong on 2017/5/25.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mangoc.h"
#include "share.h"

#define STRING_ALLOC_SIZE (256)
static char *st_string_literal_buffer = NULL;
static int st_string_literal_buffer_size = 0;
static int st_string_literal_buffer_alloc_size = 0;

void mgc_open_string_literal(void){
	st_string_literal_buffer_size = 0;
}
void mgc_add_string_literal(int letter){
	if (st_string_literal_buffer_size >= st_string_literal_buffer_alloc_size) {
		st_string_literal_buffer_alloc_size +=  STRING_ALLOC_SIZE;
		void *new_pointer = realloc(st_string_literal_buffer, st_string_literal_buffer_alloc_size);
		free(st_string_literal_buffer);
		st_string_literal_buffer = new_pointer;
	}
	
	st_string_literal_buffer[st_string_literal_buffer_size] = letter;
	st_string_literal_buffer_size++;
}
void mgc_rest_string_literal_buffer(void){
	free(st_string_literal_buffer);
	st_string_literal_buffer = NULL;
	st_string_literal_buffer_size = 0;
	st_string_literal_buffer_alloc_size = 0;
	
}
DVM_Char *mgc_close_string_literal(void){
	mgc_add_string_literal('\0');
	size_t wcs_len = dvm_mbstowcs_len(st_string_literal_buffer);
	if (wcs_len == (size_t)-1 || wcs_len == (size_t)-2) {
		mgc_compile_error(0,PARSE_ERR);
	}
	DVM_Char *new_str = malloc(sizeof(DVM_Char) * (wcs_len + 1));
	dvm_mbstowcs(st_string_literal_buffer, new_str);
	return new_str;
}
int mgc_close_character_literal(void){
	mgc_add_string_literal('\0');
	size_t wc_len = dvm_mbstowcs_len(st_string_literal_buffer);
	if (wc_len == (size_t)-1 || wc_len == (size_t)-2) {
		mgc_compile_error(0,PARSE_ERR);
	}else if (wc_len > 1){
		mgc_compile_error(0,PARSE_ERR);
	}
	DVM_Char buf[16];
	dvm_mbstowcs(st_string_literal_buffer, buf);
	return buf[0];
}


char *mgc_create_identifier(char *str){
	char *new_str = malloc(strlen(str) + 1);
	strcpy(new_str,str);
	return new_str;
}










