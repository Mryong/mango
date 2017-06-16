//
//  storage.c
//  mango
//
//  Created by jerry.yong on 2017/6/16.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include "MEM.h"

typedef union {
	long	l_dummy;
	double	d_dummy;
	void	*p_dummy;
}Cell;

#define CELL_SIZE (sizeof(Cell))
#define DEFAULT_PAGE_SIZE (1069)

typedef struct MemoryPage_tag MemoryPage;
typedef MemoryPage *MemoryPageList;

struct MemoryPage_tag{
	size_t				cell_num;
	size_t				use_cell_num;
	MemoryPageList		next;
	Cell				cell[1];
};


struct MEM_Storage_tag{
	MemoryPageList		page_list;
	int					current_page_size;
};


#define larger(a,b) ((a) > (b) ? (a) : (b))


MEM_Storage MEM_open_storage_func(MEM_Controller controller, char *filename, int line, int page_size){
	return NULL;
}
