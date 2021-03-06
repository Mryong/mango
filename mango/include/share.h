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
#include <wctype.h>
#include "MGC.h"
#include "DVM_code.h"


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


#define EXCEPTION_CLASS_NAME ("Exception")


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

#define STACK_TRACE_CLASS "StackTrace"
#define PRINT_STACK_TRACE_FUNC "Exception#print_stack_trace"
#define EXCEPTION_CLASS "Exception"
#define BUG_EXCEPTION_CLASS "BugException"
#define RUNTIME_EXCEPTION_CLASS "RuntimeException"
#define ARRAY_INDEX_EXCEPTION_CLASS "ArrayIndexOutOfBoundsException"
#define STRING_INDEX_EXCEPTION_CLASS "StringIndexOutOfBoundsException"
#define DIVISION_BY_ZERO_EXCEPTION_CLASS "DivisonByZeroException"
#define CLASS_CAST_EXCEPTION_CLASS "ClassCastException"
#define MULTIPLE_CONVERSION_EXCEPTION "MultipleCharacterConversionException"
#define NUMBER_FORMAT_EXCEPTION_EXCEPTION "NumberFormatException"


typedef struct {
	char	*mnemonic;
	char	*parameter;
	size_t	stack_increment;
}OpcodeInfo;



/* wchar.c */
size_t dvm_wcslen(wchar_t *str);
wchar_t *dvm_wcscpy(wchar_t *dest, const wchar_t *src);
wchar_t *dvm_wcsncpy(wchar_t *dest, const wchar_t *src, size_t n);
int dvm_wcscmp(const wchar_t *s1, const wchar_t *s2);
wchar_t *dvm_wcscat(wchar_t *s1, const wchar_t *s2);

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
char *dvm_create_method_function_name(char *class_name, char *method_name);

SearchFileStatus mgc_dynamic_compile(MGC_Compiler *compiler, char *package_name,
									 DVM_ExecutableList *list, DVM_ExecutableItem **add_top,
									 char *search_file);
#endif /* share_h */
