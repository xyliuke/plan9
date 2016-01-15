//
//  BizProxy.m
//  IOSDemo
//
//  Created by liuke on 15/12/25.
//  Copyright © 2015年 liuke. All rights reserved.
//

#import "BizProxy.h"
#import "bizlayer.h"

static NSMutableDictionary* notifyMap;

@implementation BizProxy

+ (void) initBiz
{
    if (!notifyMap) {
        notifyMap = [NSMutableDictionary new];
    }
    
    NSString * destRootPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSLog(@"文档根目录：%@", destRootPath);
    NSString* bundle = [[NSBundle mainBundle] bundlePath];
    NSString* lua = [bundle stringByAppendingPathComponent:@"lua"];
    [bizlayer initCommon:lua notify:^(NSDictionary *data) {
        NSLog(@"%@", data);
        [BizProxy notify:data];
    }];
}

+ (void) on:(NSString *)event notify:(void (^)(NSDictionary *))notify
{
    [notifyMap setObject:notify forKey:event];
}

+ (void) notify:(NSDictionary*)data
{

}

+ (void) call:(NSString*)method param:(NSDictionary*)param callback:(void (^)(NSDictionary *data))callback
{
    [bizlayer call:method param:param callback:callback];
}

+ (void) logI:(NSString *)msg
{
    [bizlayer logi:msg];
}

+ (void) logW:(NSString *)msg
{
    [bizlayer logw:msg];
}

+ (void) logE:(NSString *)msg
{
    [bizlayer loge:msg];
}

@end
