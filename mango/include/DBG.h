//
//  DBG.h
//  mango
//
//  Created by jerry.yong on 2017/6/20.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef DBG_h
#define DBG_h


typedef struct DBG_Controller_tag *DBG_Controller;
void DBG_set(DBG_Controller controller, char *file, int line);
void DBG_set_expression(char *expression);

#ifdef DBG_NO_DEBUG
#define DBG_create_controller() ((void)0)
#define DBG_set_debug_level(level) ((void)0)
#define DBG_set_debug_write_fp(fp) ((void)0)
#define DGB_assert(expression,arg) ((void)0)
#define DBG_panic(arg) ((void)0)
#define DGB_debug_write(arg) ((void)0)
#else /* DBG_NO_DEBUG */
#ifdef DBG_CONTROLLER
#define DBG_CURRENT_CONTROLLER DBG_CONTROLLER
#else /* DBG_CONTROLLER */
#define DBG_CURRENT_CONTROLLER dbg_default_controller
#endif /* DBG_CONTROLLER */

extern DBG_Controller DBG_CURRENT_CONTROLLER;

#define DBG_create_controller() (DBG_create_controller_func())
#define DBG_set_debug_level(level) (DBG_set_debug_level_func(DBG_CURRENT_CONTROLLER,level))
#define DBG_set_debug_write_fp(fp) (DBG_set_debug_write_fp_func(DBG_CURRENT_CONTROLLER,fp))
#define DGB_assert(expression,fmt,...) ((expression) ? (void)0 :\
(\
(DBG_set(DBG_CURRENT_CONTROLLER,__FILE__,__LINE__)),\
(DBG_set_expression(#expression)),\
(DGB_assert_func(fmt,##__VA_ARGS__))\
))
#define DBG_panic(arg) ((DBG_set(DBG_CURRENT_CONTROLLER,__FILE__,__LINE__)),(DBG_panic_func arg))
#define DGB_debug_write(fmt,...) ((DBG_set(DBG_CURRENT_CONTROLLER,__FILE__,__LINE__)),(DGB_debug_write_func(fmt,##__VA_ARGS__)))


#endif /* DBG_NO_DEBUG */


typedef enum {
	DBG_TRUE = 1,
	DBG_FALSE = 0
}DBG_Boolean;


DBG_Controller DBG_create_controller_func(void);
void DBG_set_debug_level_func(DBG_Controller controller, int level);
void DBG_set_debug_write_fp_func(DBG_Controller controller, FILE *fp);
void DGB_assert_func(char *fmt,...);
void DBG_panic_func(char *fmt,...);
void DGB_debug_write_func(int level, char *fmt,...);

#endif /* DBG_h */
