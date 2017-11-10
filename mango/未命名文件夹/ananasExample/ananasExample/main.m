//
//  main.m
//  ananasExample
//
//  Created by jerry.yong on 2017/10/31.
//  Copyright © 2017年 yongpengliang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[]) {
	extern FILE *yyin;
	extern int yyparse(void);
	char *path = "/Users/yongpengliang/Documents/macalline/ananasExample/ananas/compiler/test.ana";
	yyin = fopen(path, "r");
	if(yyparse()){
		printf("编译错误");
	}
	
	
	
	
//	@autoreleasepool {
//	    return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
//	}
}
