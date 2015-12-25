//
//  Common.h
//  Common
//
//  Created by liuke on 15/12/25.
//  Copyright © 2015年 liuke. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface bizlayer : NSObject

/**
 *  初始化库
 *  @param luaSrcPath lua文件所在工程的根目录，需要从这个目录下copy到指定目录下
 *  @param notify 通知的接口，底层给上层的通知全部通过这个接口发送数据
 */
+ (void) initCommon:(NSString*)luaSrcPath notify:(void(^)(NSDictionary* data))notify;

/**
 *  调用函数接口
 *  @param  method 函数名
 *  @param  param  参数   可以为nil
 *  @param  callback    调用结果的回调，可以为nil
 */
+ (void) call:(NSString*)method param:(NSDictionary*)param callback:(void(^)(NSDictionary* data))callback;

+ (void) stop;
//日志函数
+ (void) logi:(NSString*)msg;
+ (void) logw:(NSString*)msg;
+ (void) loge:(NSString*)msg;

//将json转换成字符串
+ (NSString*) toString:(NSDictionary*)data;
//将字符串转换成json,如果输入不是json格式字符串，则返回空字典
+ (NSDictionary*) toJson:(NSString*)data;

@end
