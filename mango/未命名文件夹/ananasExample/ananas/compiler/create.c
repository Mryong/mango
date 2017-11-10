//
//  create.c
//  ananasExample
//
//  Created by jerry.yong on 2017/11/1.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "ananasc.h"

int line_number = 1;



int yyerror(char const *str){
	printf("line:%d: %s\n",line_number,str);
	return 0;
}


