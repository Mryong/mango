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


typedef enum {
	SEARCH_FILE_SUCCESS,
	SEARCH_FILE_NOT_FOUND,
	SEARCH_FILE_PATH_TOO_LONG
}SearchFileStatus;

#ifdef MGC_WINDOWS
#define FILE_SEPARATOR      ('\\')
#define FILE_PATH_SEPARATOR (';')
#else
#define FILE_SEPARATOR      ('/')
#define FILE_PATH_SEPARATOR (':')
#endif



#define MANGO_REQUIRE_SUFFIX  (".mgh")
#define MANGO_IMPLEMENTATION_FUFFIX (".mgm")


#define ARRAY_METHOD_SIZE "size"
#define ARRAY_METHOD_RESIZE "resize"
#define ARRAY_METHOD_INSERT "insert"
#define ARRAY_METHOD_REMOVE "remove"
#define ARRAY_METHOD_ADD "add"
#define ARRAY_PREFIX "array#"

#define STRING_METHOD_LENGTH "length"
#define STRING_METHOD_SUBSTR "substr"
#define STRING_PREFIX "string#"

#define ARRAY_SZIE(array) (sizeof(array) / sizeof((array)[0]))



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



/* util.c */
SearchFileStatus dvm_search_file(char *search_path, char *search_file,
								 char *found_path, FILE **fp);
DVM_Boolean dvm_equal_string(char *str1, char *str2);
DVM_Boolean dvm_equal_package_name(char *p1, char *p2);
void dvm_strncopy(char *dest, char *src, size_t buf_size);
#endif /* share_h */
