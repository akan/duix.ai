//
//  GJDownWavTool.m
//  Digital
//
//  Created by cunzhi on 2023/11/23.
//

#import "GJDownWavTool.h"
#import "HttpClient.h"

@interface GJDownWavTool ()

@property (nonatomic, strong) HttpClient *client;
@property (nonatomic, strong) NSMutableArray *arrList;
@property (nonatomic, strong) NSFileManager *fileManager;
@property (nonatomic, strong) NSString *cachesPath;

@end
//static GJDownWavTool *instance = nil;
@implementation GJDownWavTool

+ (GJDownWavTool *)sharedTool {
    static GJDownWavTool *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken,^{
        instance = [[GJDownWavTool alloc] init];
    });

    return instance;
}
- (instancetype)init
{
    self = [super init];
    if (self) {
 
        self.client = [[HttpClient alloc] init];
        self.arrList=[[NSMutableArray alloc] init];
        self.fileManager = [NSFileManager defaultManager];

    }
    return self;
}

- (void)initCachesPath {
    
//    self.cachesPath =[GlobalFunc getHistoryCachePath:[NSString stringWithFormat:@"DownVideo/%@",[GlobalFunc getCurrentTimeYYYYMMDDHHMM]]];
    [self cencelDown];
}

- (void)downWavWithModel:(GJLDigitalAnswerModel *)model{
    

    [self.arrList addObject:model];

    if (self.arrList.count > 1) {
        return;
    }
    [self downloadWithModel:model];


}

- (void)downloadWithModel:(GJLDigitalAnswerModel *)model {
    
    NSLog(@"model.filePath:%@",model.filePath);
//    if (IS_Agent_EMPTY(self.cachesPath)) {
//        self.cachesPath =[GlobalFunc getHistoryCachePath:[NSString stringWithFormat:@"DownVideo/%@",[GlobalFunc getCurrentTimeYYYYMMDDHHMM]]];
//    }
    self.cachesPath = [self getHistoryCachePath:@"DownVideo"];
    NSString * filename = [model.filePath lastPathComponent];
    NSString *filePathStr = [NSString stringWithFormat:@"%@/%@",self.cachesPath,filename];
    NSString * pathExtern = [[filePathStr lastPathComponent] pathExtension];
    if(pathExtern==nil)
    {
        filePathStr = [NSString stringWithFormat:@"%@/%@.wav", self.cachesPath ,filename];
    }
    BOOL fileExists = [self.fileManager fileExistsAtPath:filePathStr];
    if (fileExists) {
        model.download = YES;
        model.localPath = filePathStr;
        [self downloadEnd:model];
        NSLog(@"下载==有缓存==%@\n本地=%@",model.filePath,filePathStr);
        if (self.delegate && [self.delegate respondsToSelector:@selector(downloadFinish:finish:)]) {
            [self.delegate downloadFinish:model finish:YES];
        }
    } else {
        __weak typeof(self)weakSelf = self;
        if ([[model.filePath.pathExtension lowercaseString] isEqualToString:@"wav"]) {
            NSURL *fileUrl = [NSURL fileURLWithPath:self.cachesPath];

            [[HttpClient manager] downloadWithURL:model.filePath savePathURL:fileUrl pathExtension:@"wav" progress:^(NSProgress *progress) {
                
            } success:^(NSURLResponse *response, NSURL *filePath) {
                model.download = YES;
                NSString *localPath = [filePath.absoluteString stringByReplacingOccurrencesOfString:@"file://" withString:@""];
                BOOL fileExists = [weakSelf.fileManager fileExistsAtPath:localPath];
                if (fileExists) {
                    model.localPath = localPath;
                    if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(downloadFinish:finish:)]) {
                        [weakSelf.delegate downloadFinish:model finish:YES];
                    }
                    NSLog(@"下载==成功==%@\n本地=%@",model.filePath,localPath);
                } else {
                    if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(downloadFinish:finish:)]) {
                        NSLog(@"下载==结束==音频不存在");
                        [weakSelf.delegate downloadFinish:model finish:NO];
                    }
                }
                [weakSelf downloadEnd:model];
            } fail:^(NSError *error) {
                model.download = YES;
                [weakSelf downloadEnd:model];
                if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(downloadFinish:finish:)]) {
        
                    [weakSelf.delegate downloadFinish:model finish:NO];
                }
                NSLog(@"下载==失败==%@",error.localizedDescription);
            }];
        } else {
            
            [[HttpClient manager] requestWithDownUrl:model.filePath para:nil headers:nil httpMethod:HttpMethodGet success:^(NSURLSessionDataTask *task, id responseObject) {
                if([responseObject isKindOfClass:[NSData class]])
                {
                    NSString *base64String = [responseObject base64EncodedStringWithOptions:0];
                    NSData *data = [[NSData alloc] initWithBase64EncodedString:base64String options:0];
                    if (data) {
                        
                        NSString * filename2 = [[model.filePath lastPathComponent] stringByDeletingPathExtension];
                        NSString *filePath2 = [NSString stringWithFormat:@"%@/%@.wav",self.cachesPath,filename2];
                        [weakSelf.fileManager createFileAtPath:filePath2 contents:data attributes:nil];
          
//                        [[WebCacheHelpler sharedWebCache] storeDataToDiskCache:data key:model.filePath extension:@"wav"];
//                        NSString *path = [[WebCacheHelpler sharedWebCache] isCachePathForKey:model.filePath extension:@"wav"];
                        NSString *localPath = [filePath2 stringByReplacingOccurrencesOfString:@"file://" withString:@""];
                        BOOL fileExists = [weakSelf.fileManager fileExistsAtPath:localPath];
                        if (fileExists) {
                            model.localPath = localPath;
                            if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(downloadFinish:finish:)]) {
                                [weakSelf.delegate downloadFinish:model finish:YES];
                            }
                            NSLog(@"下载==成功==%@\n本地=%@",model.filePath,localPath);
                        } else {
                            if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(downloadFinish:finish:)]) {
                                NSLog(@"下载==结束==音频不存在");
                                [weakSelf.delegate downloadFinish:model finish:NO];
                            }
                        }
                        [weakSelf downloadEnd:model];
                        
                    } else {
                        if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(downloadFinish:finish:)]) {
                            NSLog(@"下载==结束==音频不存在");
                            [weakSelf.delegate downloadFinish:model finish:NO];
                        }
                        [weakSelf downloadEnd:model];
                    }
                }
            } failure:^(NSURLSessionDataTask *task, NSError *error) {
                 
            }];
        }
        
    }
}

- (void)downloadEnd:(GJLDigitalAnswerModel *)model {
    
    [self.arrList removeObject:model];
    if (self.arrList.count >0) {
        GJLDigitalAnswerModel * answer_model=self.arrList[0];
        [self downloadWithModel:answer_model];

    }
}

- (void)cencelDown {
    
    [self.arrList removeAllObjects];
    
}
-(NSString *)getHistoryCachePath:(NSString*)pathName
{
    NSString* folderPath =[[self getFInalPath] stringByAppendingPathComponent:pathName];
    //创建文件管理器
    NSFileManager *fileManager = [NSFileManager defaultManager];
    //判断temp文件夹是否存在
    BOOL fileExists = [fileManager fileExistsAtPath:folderPath];
    //如果不存在说创建,因为下载时,不会自动创建文件夹
    if (!fileExists)
    {
        [fileManager createDirectoryAtPath:folderPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    return folderPath;
}

- (NSString *)getFInalPath
{
    NSString* folderPath =[[NSHomeDirectory() stringByAppendingPathComponent:@"Library"] stringByAppendingPathComponent:@"Cache"];
    //创建文件管理器
    NSFileManager *fileManager = [NSFileManager defaultManager];
    //判断temp文件夹是否存在
    BOOL fileExists = [fileManager fileExistsAtPath:folderPath];
    //如果不存在说创建,因为下载时,不会自动创建文件夹
    if (!fileExists) {
        [fileManager createDirectoryAtPath:folderPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    return folderPath;
}



@end
