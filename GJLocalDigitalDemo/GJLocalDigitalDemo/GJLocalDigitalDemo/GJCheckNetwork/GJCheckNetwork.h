//
//  GJCheckNetwork.h
//  GJDigitalDemo
//
//  Created by cunzhi on 2023/7/6.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN
typedef enum {
    Net_Unknown,
    Net_NotReach,
    Net_WWAN,
    Net_WiFi
}NetType;
typedef void (^onNet)(NetType state);

@interface GJCheckNetwork : NSObject
////默认配置
+ (GJCheckNetwork*)manager;
//是否有网络
@property (nonatomic, assign) NSInteger isNoNetWork;//1、有网 0无网
//网络状态回调
@property (nonatomic, copy) onNet on_net;


//网络状态
@property (nonatomic, assign) NSInteger net_type;

// 是否显示网络异常时候的alert，默认YES，如果设置为NO，需要在网络变动操作结束之后手动设置为YES，不然会影响其他地方使用
@property (nonatomic, assign) BOOL showNetErrorAlert ;

- (void)getWifiState;

-(void)toGetNetRegist;
@end

NS_ASSUME_NONNULL_END
