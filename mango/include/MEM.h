//
//  MEN.h
//  mango_preview
//
//  Created by jerry.yong on 2017/5/19.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef MEM_h
#define MEM_h
#include <stdio.h>

typedef enum {
	MEM_FAIL_AND_EXIT,
	MEM_FAIL_AND_RETURN
}MEM_FailMode;


typedef struct MEM_Controller_tag *MEM_Controller;
typedef void (*MEM_ErrorHandler)(MEM_Controller, char *, int , char *);
typedef struct MEM_Storage_tag *MEM_Storage;








#endif /* MEN_h */
