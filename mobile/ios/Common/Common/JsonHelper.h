//
//  JsonHelper.h
//  LayoutFramework
//
//  Created by liuke on 14-3-13.
//  Copyright (c) 2014年 liuke. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface JsonHelper : NSObject

/**
 *  将字典转换成字符串
 *
 *  @param dic 字典
 *
 *  @return 返回json格式的字符串
 */
+ (NSString*) json2string:(NSDictionary*) dic;

/**
 *  将json转换成字典
 *
 *  @param string json格式的字符串
 *
 *  @return 返回字典
 */
+ (NSDictionary*) string2json:(NSString*) string;

@end
