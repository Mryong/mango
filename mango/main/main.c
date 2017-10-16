//
//  main.c
//  mango_preview
//
//  Created by jerry.yong on 2017/5/25.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "share.h"
#include <string.h>
#include <locale.h>
#include "MGC.h"

int a = 1;

int test(){
	return a++;
}

#include "DVM_code.h"
int main(int argc,char * argv[]){
	setlocale(LC_CTYPE, "zh_CN.UTF-8");

	
	
	
	if (argc <2) {
		fprintf(stderr, "usage:%s filename arg1 arg2",argv[0]);
		exit(1);
	}

	
	char *str1 = "";
	char *str2 = "mango.lang";
	strcmp(str1, str2);
	
	extern FILE *yyin;
    FILE *fp = fopen(argv[1], "r");

	MGC_Compiler *compler = mgc_create_compiler();
	DVM_ExecutableList *list = mgc_compile(compler, fp, argv[1]);
	DVM_VirtualMachine *dvm = dvm_create_virtual_machine();
	DVM_set_executable(dvm, list);
	mgc_dispose_compiler(compler);
	dvm_execute(dvm);

	
	
	
	

	
}
