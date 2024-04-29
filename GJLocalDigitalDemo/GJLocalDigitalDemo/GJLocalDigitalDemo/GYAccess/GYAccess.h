//
//  GYAccessPermissions.h
//  GuiYuSiri
//
//  Created by ftt on 2021/8/4.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface GYAccess : NSObject
{
    BOOL isCamera;
    BOOL isPhoto;
    BOOL isRecord;
}
+(GYAccess*)manager;
-(BOOL)getCamerapermissions;
-(BOOL)getmicrophoneaccess;
-(BOOL)getPhotopermissions;
-(void)getCamerapermissions:(void (^)(bool isPermis))success;
-(void)getmicrophoneaccess:(void (^)(bool isPermis))success;
-(void)getPhotopermissions:(void (^)(bool isPermis))success;
@end

NS_ASSUME_NONNULL_END
