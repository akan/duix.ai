//
//  GJDownWavTool.h
//  Digital
//
//  Created by cunzhi on 2023/11/23.
//

#import <Foundation/Foundation.h>
#import "GJLDigitalAnswerModel.h"

NS_ASSUME_NONNULL_BEGIN

@protocol GJDownWavToolDelegate <NSObject>

@optional
- (void)downloadFinish:(GJLDigitalAnswerModel *)model finish:(BOOL)finish;

@end

@interface GJDownWavTool : NSObject

+ (GJDownWavTool *)sharedTool;

@property (nonatomic, weak) id <GJDownWavToolDelegate> delegate;

//每次会话单独文件夹存储
- (void)initCachesPath;

- (void)downWavWithModel:(GJLDigitalAnswerModel *)model;

- (void)cencelDown;


@end

NS_ASSUME_NONNULL_END
