//
//  generate.c
//  mango
//
//  Created by jerry.yong on 2017/6/27.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "../mangoc.h"

char *st_mango_lang_mgh_text[] = {"int xx;", NULL};
char *st_mango_lang_mgc_text[] = {"int yy;", NULL};
char *st_mango_math_mgh_text[] = {"", NULL};

BuiltinScript mgc_buildin_script[] = {
	{"mango.lang",MGH_SOURCE,st_mango_lang_mgh_text},
	{"mango.lang",MGM_SOURCE,st_mango_lang_mgc_text},
	{"mango.math",MGH_SOURCE,st_mango_math_mgh_text},
	{NULL,MGH_SOURCE,NULL}
};
