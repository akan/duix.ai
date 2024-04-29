//
//  GJLDigitalManager.h
//  GJLocalDigitalSDK
//
//  Created by guiji on 2023/12/12.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface GJLDigitalManager : NSObject
//本地数字人模型下的最大帧数
@property (nonatomic, assign)NSInteger maxCount;

+ (GJLDigitalManager*)manager;
/*
 *basePath 底层通用模型路径-保持不变
 *digitalPath 数字人模型路径- 替换数字人只需要替换这个路径
 *return 1 返回成功 -1 基本模型路径错误 -2 基本模型路径错误 -3 数字人模型路径错误 -4 数字人模型路径错误 -5数字人模型配置文件错误
 */
-(NSInteger)initBaseModel:(NSString*)basePath digitalModel:(NSString*)digitalPath;
/*
 显示界面
 */
-(void)toShow:(UIView*)view;
/*
 *playImageIndex 播放数字人画面到第几帧
 *audioIndex 播放音频到第几帧
 *bbgPath 数字人背景-可替换
 */
-(void)toPlayNext:(NSInteger)playImageIndex audioIndex:(NSInteger)audioIndex bbgPath:(NSString*)bbgPath;
/*
 wavPath 音频的本地路径 
 */
-(void)onewavWithPath:(NSString*)wavPath;
/*
音频播放结束调用
 */
-(void)toWavPlayEnd;
/*
 释放
 */
-(void)toFree;
/*
 初始化模型过后才能获取
 getDigitalSize 数字人模型的宽度 数字人模型的高度
*/
-(CGSize)getDigitalSize;
@end


