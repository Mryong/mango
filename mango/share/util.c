//
//  util.c
//  mango
//
//  Created by yongpengliang on 2017/6/10.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DVM.h"
#include "share.h"


SearchFileStatus dvm_search_file(char *search_path, char *search_file,
								 char *found_path, FILE **fp){
	return SEARCH_FILE_SUCCESS;


}

DVM_Boolean dvm_equal_package_name(char *p1, char *p2){
	return dvm_equal_string(p1, p2);
}

DVM_Boolean dvm_equal_string(char *str1, char *str2){
	if (str1 == NULL && str2 == NULL) {
		return DVM_TRUE;
	}
	
	if (str1 == NULL || str2 == NULL) {
		return DVM_FALSE;
	}
	
	
	return !strcmp(str1, str2);
}

void dvm_strncopy(char *dest, char *src, size_t buf_size){
	size_t i;
	for ( i = 0 ; i < buf_size - 1  && src[i] != '\0' ; i++) {
		dest[i] = src[i];
	}
	dest[i] = '\0';
}






