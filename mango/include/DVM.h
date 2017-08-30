//
//  DVM.h
//  mango_preview
//
//  Created by jerry.yong on 2017/5/31.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef DVM_h
#define DVM_h


typedef struct DVM_Executable_tag DVM_Executable;
typedef struct DVM_ExecutableList_tag DVM_ExecutableList;

typedef struct DVM_VirtualMachine_tag DVM_VirtualMachine;

typedef struct DVM_VTable_tag DVM_VTable;
typedef struct DVM_Object_tag DVM_Object;


typedef struct {
	DVM_VTable *v_table;
	DVM_Object *data;
}DVM_ObjectRef;

typedef struct {
	int		int_value;
	double	double_value;
	DVM_ObjectRef object;
}DVM_Value;


typedef enum {
	DVM_FALSE = 0,
	DVM_TRUE = 1
}DVM_Boolean;

DVM_VirtualMachine *DVM_create_virtual_machine(void);



#endif /* DVM_h */
