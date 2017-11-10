//
//  stringUtil.c
//  ananasExample
//
//  Created by jerry.yong on 2017/11/1.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *anc_create_identifier(char *str){
	
	char *identifier = (char *)malloc(strlen(str) + 1);
	stpcpy(identifier, str);
	return identifier;
}
