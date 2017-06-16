//
//  memory.c
//  mango_preview
//
//  Created by jerry.yong on 2017/5/19.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include "memory.h"


static void default_error_handler(MEM_Controller controller, char *filename, int line, char *msg);

static struct MEM_Controller_tag st_default_controller = {
	NULL,
	default_error_handler,
	MEM_FAIL_AND_EXIT
};


MEM_Controller men_default_controller = &st_default_controller;


typedef union {
	long		l_dummy;
	double		d_dummy;
	void		*p_dummy;
}Align;


#define MARK_SIZE	(4)

typedef struct {
	int		size;
	char	*filename;
	int		line;
	Header	*prev;
	Header	*next;
	unsigned char	mark[MARK_SIZE];
}HeaderStruct;



#define ALIGN	(sizeof(Align))
#define revalue_up_align(val) ((val)?(( (val) - 1 ) / ALIGN + 1):0)
#define HEADER_ALIGN_SIZE revalue_up_align(sizeof(HeaderStruct))
#define MARK (0xCD)

union Header_tag{
	HeaderStruct		s;
	Align				u[HEADER_ALIGN_SIZE];
};


static void default_error_handler(MEM_Controller controller, char *filename, int line, char *msg){
	fprintf(controller->error_fp, "MEM:%s failed in %s at %d \n",msg, filename, line);
}













