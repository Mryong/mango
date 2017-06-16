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


int main(int argc,char * argv[]){
	setlocale(LC_CTYPE, "zh_CN.UTF-8");
	if (argc <2) {
		fprintf(stderr, "usage:%s filename arg1 arg2",argv[0]);
		exit(1);
	}
	

	
	
	
	extern FILE *yyin;
    yyin = fopen(argv[1], "r");
	extern int yyparse();
	
	while (!feof(yyin)) {
		yyparse();
	}
	
	MGC_Compiler *compiler = mgc_get_current_compiler();
	
	

	
}
