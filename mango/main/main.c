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


int main(int argc,char * argv[]){
	
	
	
	setlocale(LC_CTYPE, "zh_CN.UTF-8");
	int b = test();
//	
//	size_t a = -1;
//	int b = 2;
//	size_t c = 0;
//	if (a > c) {
//		printf("1");
//	}else{
//	 printf("2");
//	}
//	
	
	
	
	
	if (argc <2) {
		fprintf(stderr, "usage:%s filename arg1 arg2",argv[0]);
		exit(1);
	}

	
	
	
	extern FILE *yyin;
    FILE *fp = fopen(argv[1], "r");

	MGC_Compiler *compler = mgc_create_compiler();
	mgc_compile(compler, fp, argv[1]);
	
	

	
}
