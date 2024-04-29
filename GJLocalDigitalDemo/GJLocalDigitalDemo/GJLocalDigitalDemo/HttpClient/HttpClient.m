//
//  HttpClient.m
//  Xici
//
//  Created by XICI-Jacob on 10/31/13.
//
//

#import "HttpClient.h"
#import "OpenUDID.h"
#import <zlib.h>

@implementation HttpClient


+ (HttpClient *)manager {
    static dispatch_once_t onceToken;
    static HttpClient *_sharedMangaer=nil;
    dispatch_once(&onceToken, ^{
        if (!_sharedMangaer) {
            _sharedMangaer=[[HttpClient alloc]init];
        }
    });
    return _sharedMangaer;
}

- (id)init {
    self = [super init];
    if(self) {
    }
    return self;
}

-(void)requestWithBaseURL:(NSString *)url
                     para:(NSDictionary *)parameters
                  headers:(NSDictionary *)headers
               httpMethod:(HttpMethod)httpMethod
                  success:(void (^)( id responseObject))success
                  failure:(void (^)(NSURLSessionDataTask * task,NSError *error))failure
{
    
    AFHTTPSessionManager *manager = [AFHTTPSessionManager manager];
    //申明请求的数据是json类型
    manager.requestSerializer=[AFJSONRequestSerializer serializer];

    double time=[[NSDate date] timeIntervalSince1970];
    NSString * token =self.token?:@"";
    NSString *app_Version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    NSString * skipVersion=[[NSUserDefaults standardUserDefaults] valueForKey:@"skipVersion"];

    headers=@{@"deviceType":@"1",@"deviceID":[OpenUDID value],@"token":token,@"version":app_Version,@"skipVersion":skipVersion?:@"0"};

    //申明返回的结果是json类型
    //    manager.responseSerializer = [AFJSONResponseSerializer serializer];
    if (httpMethod == HttpMethodPost_Form) {
        [manager.requestSerializer setValue:@"application/x-www-form-urlencoded;charset=utf-8" forHTTPHeaderField:@"Content-Type"];
        httpMethod = HttpMethodPost;
        
    }
   else  if (httpMethod == HttpMethodGet_Form)
    {
        [manager.requestSerializer setValue:@"application/x-www-form-urlencoded;charset=utf-8" forHTTPHeaderField:@"Content-Type"];
        httpMethod =    HttpMethodGet;
    }
    else
    {
         manager.requestSerializer=[AFJSONRequestSerializer serializer];
         manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"application/json",@"text/json", @"text/plain", @"text/html",@"image/jpg",@"audio/mpeg", nil];
    
      
    }
    [manager.requestSerializer willChangeValueForKey:@"timeoutInterval"];
     manager.requestSerializer.timeoutInterval=60.0;
    [manager.requestSerializer didChangeValueForKey:@"timeoutInterval"];
    if (httpMethod == HttpMethodPost) {
        [manager POST:url parameters:parameters headers:headers progress:^(NSProgress * _Nonnull uploadProgress) {
            
        } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
            double time2=[[NSDate date] timeIntervalSince1970];
            NSLog(@"\nHttpClient====time:%f\nurl=%@:\nparameters=\n%@\nresponseObject=\n%@",time2-time,url,parameters,responseObject);
            success(responseObject);
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];
        } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
            NSLog(@"\nHttpClient==end==error=\nurl=%@:\nparameters=\n%@\nresponseObject=\n%@",url,parameters,error);
            failure(task,error);
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];
        }];
    } else if (httpMethod==HttpMethodGet) {        
        [manager GET:url parameters:parameters headers:headers progress:^(NSProgress * _Nonnull downloadProgress) {
            
        } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
            double time2=[[NSDate date] timeIntervalSince1970];
            NSLog(@"\nHttpClient====time:%f\nurl=%@:\nparameters=\n%@\nresponseObject=\n%@",time2-time,url,parameters,responseObject);
            success(responseObject);
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];
        } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
            NSLog(@"\nHttpClient====error=\nurl=%@:\nparameters=\n%@\nresponseObject=\n%@",url,parameters,error);
            failure(task,error);
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];
        }];
    }
}
- (NSURLSessionDownloadTask *)downloadWithURL:(NSString *)url
                                 savePathURL:(NSURL *)fileURL
                                 pathExtension:(NSString*)pathExtern
                                    progress:(void (^)(NSProgress *))progress
                                     success:(void (^)(NSURLResponse *response, NSURL *filePath))success
                                        fail:(void (^)(NSError *error))fail {
    NSLog(@"url:%@",url);

    AFHTTPSessionManager *manager = [self managerWithBaseURL:nil sessionConfiguration:NO];
//    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"application/json",@"text/json", @"text/plain", @"text/html",@"image/jpg",@"image/png",@"audio/wav",@"audio/mpeg", nil];
    NSURL *urlpath = [NSURL URLWithString:url];
    NSURLRequest *request = [NSURLRequest requestWithURL:urlpath];
    NSLog(@"fileURL:%@",fileURL.absoluteString) ;
    NSURLSessionDownloadTask *downloadtask = [manager downloadTaskWithRequest:request progress:^(NSProgress * _Nonnull downloadProgress) {
        progress(downloadProgress);
        
    } destination:^NSURL * _Nonnull(NSURL * _Nonnull targetPath, NSURLResponse * _Nonnull response) {
        NSString * filename=[response suggestedFilename];
        if (pathExtern!=nil) {
            filename= [filename stringByDeletingPathExtension];
            filename=[NSString stringWithFormat:@"%@.%@",filename,pathExtern];
        }
        return [fileURL URLByAppendingPathComponent:filename];
//        return fileURL;
    } completionHandler:^(NSURLResponse * _Nonnull response, NSURL * _Nullable filePath, NSError * _Nullable error) {
        if (error) {
            fail(error);
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];
        } else {
            success(response,filePath);
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];
        }
    }];
    [downloadtask resume];
    return downloadtask;
}

- (AFHTTPSessionManager *)managerWithBaseURL:(NSString *)baseURL  sessionConfiguration:(BOOL)isconfiguration {
    
    NSURLSessionConfiguration *configuration = [NSURLSessionConfiguration defaultSessionConfiguration];
    AFHTTPSessionManager *manager = nil;
    NSURL *url = [NSURL URLWithString:baseURL];
    if (isconfiguration) {
        
        manager = [[AFHTTPSessionManager alloc] initWithBaseURL:url sessionConfiguration:configuration];
    } else {
        manager = [[AFHTTPSessionManager alloc] initWithBaseURL:url];
    }
    manager.requestSerializer = [AFHTTPRequestSerializer serializer];
    manager.responseSerializer = [AFHTTPResponseSerializer serializer];
    return manager;
}

- (void)requestWithDownUrl:(NSString *)url
                     para:(id)parameters
                  headers:(NSDictionary *)headers
               httpMethod:(HttpMethod)httpMethod
                  success:(void (^)(NSURLSessionDataTask * task,id responseObject))success
                    failure:(void (^)(NSURLSessionDataTask * task,NSError *error))failure
{

    if (url.length == 0) {
        NSError *error = [NSError new];
        failure(nil,error);
        return;
    }
    AFHTTPSessionManager *manager = [AFHTTPSessionManager manager];
//    manager.requestSerializer=[AFJSONRequestSerializer serializer];
    manager.requestSerializer.timeoutInterval=60;


   

    //    manager.operationQueue.maxConcurrentOperationCount=1;

    //申明返回的结果是json类型
    manager.responseSerializer = [AFHTTPResponseSerializer serializer];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"application/json",@"text/json", @"text/plain", @"text/html",@"image/jpg",@"audio/wav",@"audio/mpeg", nil];
 
    if (httpMethod == HttpMethodPost) {
        
        [manager POST:url parameters:parameters headers:headers progress:^(NSProgress * _Nonnull uploadProgress) {
            
        } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
            
            NSLog(@"\nGuYuReuqest==end==\nurl=%@:\n%@",url,responseObject);
            success(task,responseObject);
      
      
            //             [manager.session finishTasksAndInvalidate];
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];

            //             [self loginback:responseObject];
        } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
            failure(task,error);
            NSLog(@"\nGuYuReuqest==end==error=\nurl=%@:\n%@",url,error);
            //             [manager.session finishTasksAndInvalidate];
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];
    
        }];
    } else if (httpMethod == HttpMethodGet) {
        
        
        [manager GET:url parameters:parameters headers:headers progress:^(NSProgress * _Nonnull downloadProgress) {
            
        } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
            
                     
            
            
            success(task,responseObject);
   
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];
           // [manager invalidateSessionCancelingTasks:YES resetSession:YES];
      
            //            [manager.session finishTasksAndInvalidate];
            //            [self loginback:responseObject];
        } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
            
            if ([task.response isKindOfClass:[NSHTTPURLResponse class]]) {

                NSHTTPURLResponse *r = (NSHTTPURLResponse *)task.response;

            }
            failure(task,error);
  

           // [manager.session finishTasksAndInvalidate];
            [manager invalidateSessionCancelingTasks:YES resetSession:YES];
    
        }];
        
    }
    //});
}
@end
