//
//  debug.h
//  mango
//
//  Created by jerry.yong on 2017/6/20.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef debug_h
#define debug_h

#include <stdio.h>
#include "DBG.h"

struct DBG_Controller_tag{
	FILE		*debug_write_fp;
	int			current_debug_level;
};



#endif /* debug_h */
