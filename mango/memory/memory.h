//
//  memory.h
//  mango_preview
//
//  Created by jerry.yong on 2017/5/19.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef memory_h
#define memory_h

#include <stdio.h>
#include "MEM.h"

typedef union Header_tag Header;

struct MEM_Controller_tag{
	FILE			*error_fp;
	MEM_ErrorHandler	error_handler;
	MEM_FailMode		fail_mode;
	Header				*block_header;
};

#endif /* memory_h */
