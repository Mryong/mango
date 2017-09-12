//
//  MGC.h
//  mango
//
//  Created by jerry.yong on 2017/6/5.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef MGC_h
#define MGC_h

typedef struct MGC_Compiler_tag MGC_Compiler;

MGC_Compiler *mgc_get_current_compiler(void);


MGC_Compiler *mgc_create_compiler(void);
DVM_ExecutableList *mgc_compile(MGC_Compiler *compiler, FILE *fp, char *path);
void mgc_dispose_compiler(MGC_Compiler *compiler);


#endif /* MGC_h */
