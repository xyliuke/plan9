//
//  BizProxy.h
//  IOSDemo
//
//  Created by liuke on 15/12/25.
//  Copyright © 2015年 liuke. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BizProxy : NSObject

+ (void) initBiz;

+ (void) on:(NSString*)event notify:(void(^)(NSDictionary* data))notify;

+ (void) call:(NSString*)method param:(NSDictionary*)param callback:(void (^)(NSDictionary *data))callback;

+ (void) logI:(NSString*)msg;

+ (void) logW:(NSString*)msg;

+ (void) logE:(NSString*)msg;

@end
