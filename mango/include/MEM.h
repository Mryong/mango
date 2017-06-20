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




#ifdef MEM_CONTROLLER
#define MEM_CURRENT_CONTROLLER MEM_CONTROLLER
#else
#define MEM_CURRENT_CONTROLLER men_default_controller
#endif

extern MEM_Controller men_default_controller;

void *MEM_malloc_func(MEM_Controller controller, char *filename, int line, size_t size);
void *MEM_realloc_func(MEM_Controller controller, char *filename, int line,
					   void *ptr, size_t size);
char *MEM_strdup_func(MEM_Controller controller, char *filename, int line, char *str);
void MEM_free_func(MEM_Controller controller, void *ptr);
void MEM_set_error_handler(MEM_Controller controller, MEM_ErrorHandler handler);
void MEM_set_fail_mode(MEM_Controller controller, MEM_FailMode mode);
void MEM_dump_blocks_func(MEM_Controller controller, FILE *fp);
void MEM_check_block_fun(MEM_Controller controller, char *filename, int line, void *ptr);
void MEM_check_all_blocks_fun(MEM_Controller controller, char *filename, int line);

MEM_Storage MEM_open_storage_func(MEM_Controller controller, char *filename, int line, int page_size);
void  *MEM_storage_malloc_fun(MEM_Controller controller, char *filename, int line, MEM_Storage storage, size_t size);
void MEM_dispose_storage_fun(MEM_Controller controller, MEM_Storage storage);

#define MEM_malloc(size) (MEM_malloc_func(MEM_CURRENT_CONTROLLER,__FILE__,__LINE__,size))
#define MEM_realloc(ptr,size) (MEM_realloc_func(MEM_CURRENT_CONTROLLER,__FILE__,__LINE__,ptr,size))
#define MEM_strdup(str) (MEM_strdup_func(MEM_CURRENT_CONTROLLER,__FILE__,__LINE__,str))
#define MEM_free(ptr) (MEM_free_func(MEM_CURRENT_CONTROLLER,ptr))

#define MEM_open_storage(page_size) (MEM_open_storage_func(MEM_CURRENT_CONTROLLER,__FILE__,__LINE__,page_size))
#define MEM_storage_malloc(storage, size) (MEM_storage_malloc_fun(MEM_CURRENT_CONTROLLER, __FILE__, __LINE__, storage, size))
#define MEM_dispose_storage(storage) (MEM_dispose_storage_fun(MEM_CURRENT_CONTROLLER,storage))



#ifdef DEBUG
#define MEM_dump_blocks(fp) (MEM_dump_blocks_func(MEM_CURRENT_CONTROLLER,fp))
#define MEM_check_block(ptr) (MEM_check_block_fun(MEM_CURRENT_CONTROLLER,__FILE__,__LINE__,ptr))
#define MEM_check_all_blocks() (MEM_check_all_blocks_fun(MEM_CURRENT_CONTROLLER,__FILE__,__LINE__))
#else
#define MEM_dump_blocks(fp) ((void)0)
#define MEM_check_block(ptr) ((void)0)
#define MEM_check_all_blocks() ((void)0)
#endif

#endif /* MEN_h */
