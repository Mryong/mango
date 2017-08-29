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


typedef enum {
	DVM_FALSE = 0,
	DVM_TRUE = 1
}DVM_Boolean;
DVM_VirtualMachine *DVM_create_virtual_machine(void);



#endif /* DVM_h */
