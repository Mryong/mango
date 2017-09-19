//
//  wchar.c
//  mango
//
//  Created by jerry.yong on 2017/9/18.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "share.h"
#include "MEM.h"


wchar_t *dvm_mbs2wcs(char *src){
	size_t len = dvm_mbstowcs_len(src);
	wchar_t *ret= MEM_malloc(sizeof(wchar_t) * (len + 1));
	dvm_mbstowcs(src, ret);
	return ret;
}

char *dvm_wcs2mbs(wchar_t *src){
	return dvm_wcstombs_alloc(src);
}
