//
//  util.c
//  mango
//
//  Created by jerry.yong on 2017/6/5.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "mangoc.h"



MGC_Compiler *mgc_get_current_compiler(void){
	static MGC_Compiler st_current_Compiler;
	return &st_current_Compiler;
}
