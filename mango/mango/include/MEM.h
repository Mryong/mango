//
//  MEN.h
//  mango_preview
//
//  Created by jerry.yong on 2017/5/19.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef MEM_h
#define MEM_h

#include <stdlib.h>
#include <string.h>

static char *strdups(char *str){
	char *ptr = malloc(sizeof(char) * (strlen(str) + 1));
	strcpy(ptr, str);
	return ptr;
}

#define MEM_malloc(size) 	malloc(size)
#define MEM_realloc(ptr,size) realloc(ptr, size)
#define MEM_strdup(str) strdups(str)
#define MEM_free(ptr) free(ptr)



#endif /* MEN_h */
