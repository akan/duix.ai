//
//  DigitalAnswerModel.m
//  
//
//  Created by cunzhi on 2023/11/2.
//

#import "GJLDigitalAnswerModel.h"

@implementation GJLDigitalAnswerModel

- (instancetype)init {
    
    self = [super init];
    if (self) {
        self.answer = @"";
        self.filePath = @"";
        self.isEnd = NO;
        self.localPath = @"";
        self.duration = 0;
        self.download = NO;
    }
    return self;
}


@end
