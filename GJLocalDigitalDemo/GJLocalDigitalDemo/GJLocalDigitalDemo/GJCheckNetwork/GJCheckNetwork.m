//
//  GJCheckNetwork.m
//  GJDigitalDemo
//
//  Created by cunzhi on 2023/7/6.
//

#import "GJCheckNetwork.h"
#import "AFNetworking.h"
#import <CoreTelephony/CTCellularData.h>
#import <CoreTelephony/CoreTelephonyDefines.h>
@implementation GJCheckNetwork
static GJCheckNetwork *manager = nil;
+ (GJCheckNetwork*)manager {
    if (manager == nil) {
        @synchronized(self) {
            if (manager == nil) {
                manager = [[GJCheckNetwork alloc] init];
            }
        }
    }
    return manager;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _showNetErrorAlert = YES;
    }
    return self;
}

- (void)getWifiState {
    __weak __typeof(self) weakSelf = self;
    AFNetworkReachabilityManager *manager = [AFNetworkReachabilityManager sharedManager];
    [manager startMonitoring];
    [manager setReachabilityStatusChangeBlock:^(AFNetworkReachabilityStatus status) {
        NSString * type = 0;
        switch (status)
        {
            case AFNetworkReachabilityStatusUnknown:
            {
                //未知网络
                NSLog(@"未知网络");
                weakSelf.isNoNetWork=1;
                weakSelf.net_type=Net_Unknown;
                if(weakSelf.on_net!=nil)
                {
                    weakSelf.on_net(Net_Unknown);
                }
            }
                break;
            case AFNetworkReachabilityStatusNotReachable:
            {
                //无法联网
                // NSLog(@"无法联网");
                type = @"-1";
                weakSelf.isNoNetWork=0;
                weakSelf.net_type=Net_NotReach;
                if(weakSelf.on_net!=nil)
                {
                    weakSelf.on_net(Net_NotReach);
                }
            }
                break;
                
            case AFNetworkReachabilityStatusReachableViaWWAN:
            {
                //手机自带网络
                type = @"1";
                NSLog(@"当前使用的是2g/3g/4g网络");
                weakSelf.net_type=Net_WWAN;
                weakSelf.isNoNetWork=1;
                if(weakSelf.on_net!=nil)
                {
                    weakSelf.on_net(Net_WWAN);
                }
            }
                break;
            case AFNetworkReachabilityStatusReachableViaWiFi:
            {
                type = @"2";
                //WIFI
                NSLog(@"当前在WIFI网络下");
                weakSelf.net_type=Net_WiFi;
                weakSelf.isNoNetWork=1;
                if(weakSelf.on_net!=nil)
                {
                    weakSelf.on_net(Net_WiFi);
                }
            }
                
        }
        
        [[NSNotificationCenter defaultCenter] postNotificationName:@"GY_NET_LINK_STATU_KEY" object:type];
        
    }];
}


@end
