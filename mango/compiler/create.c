//
//  create.c
//  mango_preview
//
//  Created by jerry.yong on 2017/5/25.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include "mangoc.h"
#include <stdio.h>
#include <stdlib.h>

Expression *mgc_alloc_expression(ExpressionKind kind){
	Expression *exp = malloc(sizeof(*exp));
	exp->kind = kind;
	exp->line_number = 1;
	return exp;
}




int yyerror(char const *str){
	printf("%s\n",str);
	return 0;
}

void mgc_compile_error(int line_number,CompileError err,...){
	printf("%s\n","mgc_compile_error");
}


void t(){
	Expression *expression = mgc_alloc_expression(INT_EXPRESSION);
	sscanf("", "%d",&expression->u.int_value);
	
}
