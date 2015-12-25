//
//  JsonHelper.m
//  LayoutFramework
//
//  Created by liuke on 14-3-13.
//  Copyright (c) 2014年 liuke. All rights reserved.
//

#import "JsonHelper.h"

@implementation JsonHelper

+ (NSString*) json2string:(NSDictionary *)dic
{
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dic
                                                       options:0
                                                         error:&error];
    
    if (! jsonData) {
        return @"{}";
    } else {
        return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    }
}


+ (NSDictionary*) tryString2dictionary:(NSString *)string
{
    NSError* error;
    NSDictionary* dic = [NSJSONSerialization JSONObjectWithData:[string dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingMutableLeaves error: &error];
    if (dic) {
        return dic;
    }else{
        return [[NSDictionary alloc] init];
    }
}

+ (NSDictionary*) string2json:(NSString *)string
{
    if ([string hasPrefix:@"\""] && [string hasSuffix:@"\""]) {
        NSString* s = [string substringWithRange:NSMakeRange(1, string.length - 2)];
        return [JsonHelper tryString2dictionary:s];
    }
    return [JsonHelper tryString2dictionary:string];
}

@end
