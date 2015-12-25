//
//  Common.m
//  Common
//
//  Created by liuke on 15/12/25.
//  Copyright © 2015年 liuke. All rights reserved.
//

#import "bizlayer.h"
#import "JsonHelper.h"
#include <init/common.h>

static void(^callabck_to)(NSDictionary*);

@implementation bizlayer

+ (NSString*) getDataDir
{
    static NSString* folder = nil;
    if (folder) {
        return folder;
    }
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString * destRootPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *mainFolder = [destRootPath stringByAppendingPathComponent:@"data"];
    BOOL isDir;
    if (![fileManager fileExistsAtPath:mainFolder isDirectory:&isDir]) {
        [fileManager createDirectoryAtPath:mainFolder withIntermediateDirectories:YES attributes:nil error:nil];
    }
    folder = mainFolder;
    return folder;
}

+ (NSString*) getLuaDir
{
    static NSString* ret = nil;
    if (ret) {
        return ret;
    }
    NSString* data = [bizlayer getDataDir];
    NSString* lua = [data stringByAppendingPathComponent:@"lua"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:lua]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:lua withIntermediateDirectories:YES attributes:nil error:nil];
    }
    ret = lua;
    return lua;
}

+ (void) copyDir:(NSString*) src dest:(NSString*)dest
{
    if (![[NSFileManager defaultManager] fileExistsAtPath:dest]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:dest withIntermediateDirectories:YES attributes:nil error:nil];
    }
    NSArray* sub = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:src error:nil];
    for (NSString* s in sub) {
        NSString* subPath = [src stringByAppendingPathComponent:s];
        NSString* destPath = [dest stringByAppendingPathComponent:s];
        BOOL isDir = NO;
        [[NSFileManager defaultManager] fileExistsAtPath:subPath isDirectory:&isDir];
        if (isDir) {
            if (![[NSFileManager defaultManager] fileExistsAtPath:destPath]) {
                [[NSFileManager defaultManager] createDirectoryAtPath:destPath withIntermediateDirectories:YES attributes:nil error:nil];
            }
        }else{
            [[NSFileManager defaultManager] removeItemAtPath:destPath error:nil];
            [[NSFileManager defaultManager] copyItemAtPath:subPath toPath:destPath error:nil];
        }
    }
}

+ (BOOL) copyLua2Dir:(NSString*)luaSrcPath
{
    NSString* dest = [bizlayer getLuaDir];
    [[NSFileManager defaultManager] fileExistsAtPath:luaSrcPath];
    [bizlayer copyDir:luaSrcPath dest:dest];
    return YES;
}

+ (void) initCommon:(NSString *)luaSrcPath notify:(void (^)(NSDictionary *))notify
{
    [bizlayer copyLua2Dir:luaSrcPath];
    NSString* data = [bizlayer getDataDir];
    NSString* lua = [bizlayer getLuaDir];
    plan9::common::init([data UTF8String], [lua UTF8String]);
    callabck_to = notify;
}

+ (void) call:(NSString *)method param:(NSDictionary *)param callback:(void (^)(NSDictionary *))callback
{
    if (method && [method isKindOfClass:[NSString class]]) {
        NSString* p_str = @"";
        if (param) {
            p_str = [JsonHelper json2string:param];
        }
        
        if (![@"log" isEqualToString:method]) {
            [bizlayer logi:[NSString stringWithFormat:@"call method : %@, param : %@", method, p_str]];
        }
        
        if (callback) {
            plan9::common::call([method UTF8String], [p_str UTF8String], [=](std::string result){
                callback([JsonHelper string2json:[[NSString alloc] initWithUTF8String:result.c_str()]]);
            });
        } else {
            if (p_str) {
                plan9::common::call([method UTF8String], [p_str UTF8String], nullptr);
            } else {
                plan9::common::call([method UTF8String]);
            }
            
        }
    } else {
        [bizlayer loge:[NSString stringWithFormat:@"call method : %@ is not string", method]];
    }
}

+ (void) stop
{
    plan9::common::stop();
}

+ (void) logi:(NSString *)msg
{
    if (msg) {
        NSDictionary* tmp = @{@"target" : @"ui", @"level" : @"info", @"msg" : msg};
        [bizlayer call:@"log" param:tmp callback:nil];
    }
}

+ (void) logw:(NSString *)msg
{
    if (msg) {
        NSDictionary* tmp = @{@"target" : @"ui", @"level" : @"warn", @"msg" : msg};
        [bizlayer call:@"log" param:tmp callback:nil];
    }
}

+ (void) loge:(NSString *)msg
{
    if (msg) {
        NSDictionary* tmp = @{@"target" : @"ui", @"level" : @"error", @"msg" : msg};
        [bizlayer call:@"log" param:tmp callback:nil];
    }
}

+ (NSString*) toString:(NSDictionary *)data
{
    return [JsonHelper json2string:data];
}

+ (NSDictionary*) toJson:(NSString *)data
{
    return [JsonHelper string2json:data];
}

@end
