//
//  wchar.c
//  mango_preview
//
//  Created by jerry.yong on 2017/6/1.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include "DVM.h"

size_t dvm_mbstowcs_len(const char *src){
	size_t src_idx = 0, dest_count = 0, wc_len = 0;
	mbstate_t ps;
	memset(&ps, 0, sizeof(mbstate_t));
	while (src[src_idx] != '\0') {
		wc_len = mbrtowc(NULL, &src[src_idx], MB_LEN_MAX, &ps);
		if (wc_len == (size_t)-1 || wc_len == (size_t)-2) {
			return wc_len;
		}
		dest_count++;
		src_idx += wc_len;
	}
	return dest_count;
}

void dvm_mbstowcs(const char *src,wchar_t *dest){
	size_t src_idx = 0, dest_idx = 0, wc_len = 0;
	mbstate_t ps;
	memset(&ps, 0, sizeof(mbstate_t));
	while (src[src_idx] != '\0') {
		wc_len = mbrtowc(&dest[dest_idx], &src[src_idx], MB_LEN_MAX, &ps);
		dest_idx++;
		src_idx += wc_len;
	}
	dest[dest_idx] = L'\0';
}


size_t dvm_wcstombs_len(const wchar_t *src){
	size_t src_idx = 0, dest_count = 0, wc_len = 0;
	char dummy[MB_LEN_MAX];
	mbstate_t ps;
	memset(&ps, 0, sizeof(mbstate_t));
	while (src[src_idx] != L'\0') {
		wc_len = wcrtomb(dummy, src[src_idx], &ps);
		if (wc_len == (size_t)-1) {
			return -1;
		}
		src_idx++;
		dest_count += wc_len;
	}
	return dest_count;
}


void dvm_wcstombs_i(const wchar_t *src,char *dest){
	size_t src_idx = 0, dest_idx = 0, wc_len = 0;
	mbstate_t ps;
	memset(&ps, 0, sizeof(mbstate_t));
	while (src[src_idx] != L'\0') {
		wc_len = wcrtomb(&dest[dest_idx], src[src_idx], &ps);
		if (wc_len == (size_t)-1) {
			return;
		}
		src_idx++;
		dest_idx += wc_len;
	}
	dest[dest_idx] = '\0';
}


char *dvm_wcstombs_alloc(const wchar_t *src){
	size_t mb_len = dvm_wcstombs_len(src) + 1;
	char *mbs  = malloc(mb_len);
	dvm_wcstombs_i(src, mbs);
	return mbs;
}

char dvm_wctochar(wchar_t src){
	char dummy[MB_LEN_MAX];
	mbstate_t ps;
	memset(&ps, 0, sizeof(mbstate_t));
	size_t wc_len = wcrtomb(dummy, src, &ps);
	assert(wc_len == 1);
	return dummy[0];
}


int dvm_print_wcs(FILE *fp,wchar_t *wcs){
	char *mbs = dvm_wcstombs_alloc(wcs);
	int result = fprintf(fp, "%s",mbs);
	free(mbs);
	return result;
}

int dvm_print_wcs_ln(FILE *fp,wchar_t *wcs){
	int result = dvm_print_wcs(fp, wcs);
	fprintf(fp, "\n");
	return result;
}


DVM_Boolean dvm_idwdigit(wchar_t ch){
	return ch >= L'0' && ch <= L'9';
}





















