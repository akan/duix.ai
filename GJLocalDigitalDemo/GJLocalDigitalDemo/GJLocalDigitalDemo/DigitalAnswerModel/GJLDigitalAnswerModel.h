//
//  DigitalAnswerModel.h
//  
//
//  Created by cunzhi on 2023/11/2.
//

#import <Foundation/Foundation.h>



@interface GJLDigitalAnswerModel : NSObject

@property (nonatomic, strong) NSString * questionId;
@property (nonatomic, strong) NSString *answer;
@property (nonatomic, strong) NSString *filePath;

@property (nonatomic, assign) BOOL isEnd;
@property (nonatomic, assign) NSInteger duration; //毫秒

//本地路径
@property (nonatomic, strong) NSString *localPath;
//音频是否已下载
@property (nonatomic, assign) BOOL download;


@end


