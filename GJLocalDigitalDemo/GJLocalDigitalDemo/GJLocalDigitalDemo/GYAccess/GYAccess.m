//
//  GYAccessPermissions.m
//  GuiYuSiri
//
//  Created by ftt on 2021/8/4.
//

#import "GYAccess.h"
#import <Photos/Photos.h>
#import <PhotosUI/PHPhotoLibrary+PhotosUISupport.h>

@implementation GYAccess
static GYAccess *instance = nil;

#pragma mark -
#pragma mark 单实例模式、初始化
+(GYAccess*)manager
{
    @synchronized(self)
    {
        if (instance==nil)
        {
            instance=[[GYAccess alloc] init];
        
        }
    }
    return instance;
}



-(BOOL)getCamerapermissions
{
    
    return isCamera;
    
}

-(BOOL)getmicrophoneaccess{


    return isRecord;
    
}

-(BOOL)getPhotopermissions{
  
    return isPhoto;

}
//相机权限
-(void)getCamerapermissions:(void (^)(bool isPermis))success
{
//      BOOL ranted;
    [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
                if(granted){

                } else
                {
                    dispatch_async(dispatch_get_main_queue(), ^{
                    [self jumptoVC:@"您还没有允许相机权限,录制视频需要相机权限"];
                    });
                }
              self->isCamera=granted;
              success(granted);
            }];

    
//    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];//读取设备授权状态
//    if(authStatus == AVAuthorizationStatusRestricted || authStatus == AVAuthorizationStatusDenied)
//    {
//        ranted= NO;
//            NSLog(@"相机不可用");
//        dispatch_async(dispatch_get_main_queue(), ^{
//    //        // UI更新代码
//
//        });
//
//
//   } else
//   {
//
//                 //  DLog(@"相机可用");
//            NSLog(@"相机可用");
//            ranted= YES;
//  }
   

    
}

-(void)getmicrophoneaccess:(void (^)(bool isPermis))success
{
    __block  BOOL ranted;
    [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio completionHandler:^(BOOL granted) {
        
        ranted=granted;
        self->isRecord=granted;
        if (ranted==NO) {
            dispatch_async(dispatch_get_main_queue(), ^{
            [self jumptoVC:@"您还没有允许麦克风权限,录音和语音识别需要麦克风权限"];
            });
        }
        success(ranted);
    }];

    
}

-(void)getPhotopermissions:(void (^)(bool isPermis))success
{
    dispatch_async(dispatch_get_main_queue(), ^{
    
    __block  bool sranted=NO;
    
    if (@available(iOS 14, *)) {
   
        PHAccessLevel level = PHAccessLevelReadWrite;
        [PHPhotoLibrary requestAuthorizationForAccessLevel:level handler:^(PHAuthorizationStatus status) {
          switch (status) {
              case PHAuthorizationStatusLimited:
                  NSLog(@"limited");
                  sranted=YES;
                  self->isPhoto=sranted;
//                  [self jumptoVC:@"访问权限受限,无法访问相册中的所有照片"];
                  success(sranted);
                  break;
              case PHAuthorizationStatusNotDetermined:
                  NSLog(@"NotDetermined");
                  sranted=NO;
                  self->isPhoto=sranted;
                  [self jumptoVC:@"您还没有允许相册权限"];
                  success(sranted);
                  break;
              case PHAuthorizationStatusDenied:
                  NSLog(@"denied");
                  sranted=NO;
                  self->isPhoto=sranted;
                  [self jumptoVC:@"您还没有允许相册权限"];
                  success(sranted);
                  break;
              case PHAuthorizationStatusAuthorized:
                  NSLog(@"authorized");
                  sranted=YES;
                  self->isPhoto=sranted;
                  success(sranted);
                  break;
              default:
                  break;
                
                
          }
        }];

 


    } else {

        [PHPhotoLibrary requestAuthorization:^(PHAuthorizationStatus status) {
          
          
                if (status==PHAuthorizationStatusAuthorized) {
                    sranted=YES;
                   
                }else if (status==PHAuthorizationStatusDenied){
                    sranted=NO;
    
    
                    [self jumptoVC:@"您还没有允许相册权限"];
    
                    NSLog(@"1111sranted==%d",sranted);
    
                }else if (status==PHAuthorizationStatusRestricted){
    
                    sranted=YES;
    
                }else
                {
    
                    sranted=YES;
                }
              self->isPhoto=sranted;
                success(sranted);
            NSLog(@"sranted==%d",sranted);
        }];
        
        
        
    }
     
    });
    

    


   

}
-(void)jumptoVC:(NSString *)title{
    
    dispatch_async(dispatch_get_main_queue(), ^{
//        GYActionAlertView * alert = [GYActionAlertView new];
//        [alert setAlert:title message:@"去设置一下吧" attributeMessage:nil cancel:@"取消" confirm:@"去设置" delegate:nil handler:^(NSInteger type) {
//            if (type == 1) {
//                NSURL * url = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
//                [UIApplication.sharedApplication openURL:url options:nil completionHandler:^(BOOL success) {}];
//            }
//        }];
//        [alert show];
    });
    
}

//获取手机当前显示的ViewController
//
//-(UIViewController*)currentViewController{
//
//UIViewController* vc = [UIApplication sharedApplication].keyWindow.rootViewController;
//
//while (1) {
//    
//    if ([vc isKindOfClass:[UITabBarController class]]) {
//        
//        vc = ((UITabBarController*)vc).selectedViewController;
//        
//    }
//    if ([vc isKindOfClass:[UINavigationController class]]) {
//        
//        vc = ((UINavigationController*)vc).visibleViewController;
//        
//    }
//
//    if (vc.presentedViewController) {
//        
//        vc = vc.presentedViewController;
//        
//    }else{
//        
//        break;
//        
//    }
//}
//
//return vc;
//}
-(void)takePhotopermissions{
    
    
    [PHPhotoLibrary requestAuthorization:^(PHAuthorizationStatus status) {
      
      
//            if (status==PHAuthorizationStatusAuthorized) {
//                sranted=YES;
//            }else if (status==PHAuthorizationStatusDenied){
//                sranted=NO;
//
//
//                [self jumptoVC:@"您还没有允许相册权限"];
//
//                NSLog(@"1111sranted==%d",sranted);
//
//            }else if (status==PHAuthorizationStatusRestricted){
//
//                sranted=YES;
//
//            }else{
//
//                sranted=YES;
//            }

    
    }];
    
    
}

@end
