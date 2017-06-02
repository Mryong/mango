//
//  share.h
//  mango_preview
//
//  Created by jerry.yong on 2017/5/25.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#ifndef share_h
#define share_h

#include "DVM.h"

/* wchar.c */
size_t dvm_mbstowcs_len(const char *src);
void dvm_mbstowcs(const char *src,wchar_t *dest);
size_t dvm_wcstombs_len(const wchar_t *src);
void dvm_wcstombs_i(const wchar_t *src,char *dest);
char *dvm_wcstombs_alloc(const wchar_t *src);
char dvm_wctochar(wchar_t src);
int dvm_print_wcs(FILE *fp,wchar_t *wcs);
int dvm_print_wcs_ln(FILE *fp,wchar_t *wcs);
DVM_Boolean dvm_idwdigit(wchar_t ch);
#endif /* share_h */
