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



MGC_Compiler *mgc_get_current_compiler(void){
	static MGC_Compiler st_current_Compiler;
	return &st_current_Compiler;
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
	
	char *ret_val = malloc(len);
	fragment = package_name;
	ret_val[0] = '\0';
	while (fragment != NULL) {
		strcat(ret_val, fragment->name);
		if (fragment->next) {
			strcat(ret_val, ".");
		}
	}
	return ret_val;
}


DVM_Boolean mgc_equal_package_name(PackageName *package_name1, PackageName *package_name2){
	if (package_name1 == NULL || package_name2 == NULL) {
		return DVM_FALSE;
	}
	
	
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
