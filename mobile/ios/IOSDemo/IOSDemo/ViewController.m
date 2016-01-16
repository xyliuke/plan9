//
//  ViewController.m
//  IOSDemo
//
//  Created by liuke on 15/12/25.
//  Copyright © 2015年 liuke. All rights reserved.
//

#import "ViewController.h"
#import "BizProxy.h"

@interface ViewController ()
@property (nonatomic, strong) NSTimer* timer;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    UIButton* btn = [[UIButton alloc] initWithFrame:CGRectMake(100, 100, 100, 100)];
    [btn setTitle:@"测试发送数据" forState:UIControlStateNormal];
    [btn addTarget:self action:@selector(action) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:btn];
    

    [BizProxy call:@"native.connect" param:nil callback:nil];
    
    [BizProxy call:@"native.get_error_code" param:nil callback:^(NSDictionary *data) {
        NSLog(@"%@",data);
    }];
}

- (void) action
{
    NSLog(@"xxxxxxxxxxx\n");
    [BizProxy call:@"native.send" param:@{@"msg" : @{@"test_data" : @"hello world from ios"}} callback:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
