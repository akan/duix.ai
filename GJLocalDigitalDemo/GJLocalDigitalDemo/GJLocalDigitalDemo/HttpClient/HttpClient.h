//
//  HttpClient.h
//  Xici
//
//  Created by XICI-Jacob on 10/31/13.
//
//

#import "AFNetworking.h"

typedef enum HttpMethod{
    HttpMethodGet,
    HttpMethodPost,
    HttpMethodPost_Form, // post表单请求
    HttpMethodGet_Form, // GET表单请求
}HttpMethod;



@interface HttpClient : NSObject
{
    NSString *lastUploadString;
}

@property(nonatomic,strong)NSString* token;
+ (HttpClient *)manager;

- (void)requestWithBaseURL:(NSString *)url
      para:(NSDictionary *)parameters
     headers:(NSDictionary *)headers
    httpMethod:(HttpMethod)httpMethod
    success:(void (^)( id responseObject))success
    failure:(void (^)(NSURLSessionDataTask *task,NSError *error))failure;



- (NSURLSessionDownloadTask *)downloadWithURL:(NSString *)url
                                 savePathURL:(NSURL *)fileURL
                                 pathExtension:(NSString*)pathExtern
                                    progress:(void (^)(NSProgress *))progress
                                     success:(void (^)(NSURLResponse *response, NSURL *filePath))success
                                        fail:(void (^)(NSError *error))fail;

- (void)requestWithDownUrl:(NSString *)url
                     para:(id)parameters
                  headers:(NSDictionary *)headers
               httpMethod:(HttpMethod)httpMethod
                  success:(void (^)(NSURLSessionDataTask * task,id responseObject))success
                   failure:(void (^)(NSURLSessionDataTask * task,NSError *error))failure;


@end
