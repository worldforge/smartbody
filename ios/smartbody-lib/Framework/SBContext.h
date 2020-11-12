//
//  SBContext.h
//  Smartbody
//
//  Created by Anton Leuski on 9/24/17.
//  Copyright © 2017 Smartbody Project. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <simd/matrix_types.h>
#import <simd/vector_types.h>
#if !defined(SWIFT_WARN_UNUSED_RESULT)
# if __has_attribute(warn_unused_result)
#  define SWIFT_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
# else
#  define SWIFT_WARN_UNUSED_RESULT
# endif
#endif

#if !defined(SWIFT_UNAVAILABLE)
# define SWIFT_UNAVAILABLE __attribute__((unavailable))
#endif

#if !defined(OBJC_DESIGNATED_INITIALIZER)
# if __has_attribute(objc_designated_initializer)
#  define OBJC_DESIGNATED_INITIALIZER __attribute__((objc_designated_initializer))
# else
#  define OBJC_DESIGNATED_INITIALIZER
# endif
#endif

@class SBContext;

@protocol SBContextDelegate <NSObject>
- (void)context:(SBContext * _Nonnull)context log:(NSString * _Nonnull)message NS_SWIFT_NAME(context(_:log:));
- (void)contextWillStartPlayingAudio:(SBContext * _Nonnull)context  NS_SWIFT_NAME(contextWillStartPlayingAudio(_:));
- (void)contextDidStartPlayingAudio:(SBContext * _Nonnull)context  NS_SWIFT_NAME(contextDidStartPlayingAudio(_:));
- (void)context:(SBContext * _Nonnull)context didFinishPlayingAudioSuccessfully:(BOOL)flag NS_SWIFT_NAME(context(_:didFinishPlayingAudioSuccessfully:));
- (void)contextCallbackHandler:(SBContext * _Nonnull)context callbackMessage:(NSString * _Nonnull)callbackMessage NS_SWIFT_NAME(contextCallbackHandler(_:callbackMessage:));
@end

@class AVAudioPlayer;

@interface SBPythonObject : NSObject
@property (nonatomic, strong, readonly) NSNumber* _Nullable numberWithInt;
@property (nonatomic, strong, readonly) NSNumber* _Nullable numberWithBool;
@property (nonatomic, strong, readonly) NSNumber* _Nullable numberWithFloat;
@property (nonatomic, strong, readonly) NSString* _Nullable stringValue;
@end

@interface SBContext : NSObject
@property (nonatomic, weak) id<SBContextDelegate> _Nullable delegate;
@property (nonatomic, strong) AVAudioPlayer * _Nullable audioPlayer;
- (nonnull instancetype)initWithAssetsURL:(NSURL * _Nonnull)assetsURL;
- (nonnull instancetype)initWithAssetsURL:(NSURL * _Nonnull)assetsURL delegate:(id<SBContextDelegate> _Nullable)delegate OBJC_DESIGNATED_INITIALIZER;
- (void)setupDrawingWithSize:(CGSize)size NS_SWIFT_NAME(setupDrawing(size:)); 
- (void)drawFrame:(CGSize)size NS_SWIFT_NAME(drawFrame(size:));
- (void)drawFrame:(CGSize)size
  modelViewMatrix:(matrix_float4x4)modelViewMatrix
 projectionMatrix:(matrix_float4x4)projectionMatrix
     gazeAtCamera:(BOOL)gazeAtCamera NS_SWIFT_NAME(drawFrame(size:modelViewMatrix:projectionMatrix:gazeAtCamera:));
- (void)reloadTexture;
- (void)update:(NSTimeInterval)time;
- (void)executePythonCommand:(NSString * _Nonnull)command NS_SWIFT_NAME(run(script:));
- (SBPythonObject* _Nullable)evaluatePythonCommand:(NSString * _Nonnull)command NS_SWIFT_NAME(execute(command:));
- (void)cameraOperationWithDx:(float)dx dy:(float)dy mode:(NSInteger)mode NS_SWIFT_NAME(cameraOperation(dx:dy:mode:));
- (NSInteger)intForKey:(NSString * _Nonnull)key SWIFT_WARN_UNUSED_RESULT;
- (NSString * _Nonnull)stringForKey:(NSString * _Nonnull)key SWIFT_WARN_UNUSED_RESULT;
- (BOOL)boolForKey:(NSString * _Nonnull)key SWIFT_WARN_UNUSED_RESULT;
- (double)doubleForKey:(NSString * _Nonnull)key SWIFT_WARN_UNUSED_RESULT;
- (nonnull instancetype)init OBJC_DESIGNATED_INITIALIZER;

- (matrix_float4x4)modelViewMatrix:(CGSize)size NS_SWIFT_NAME(modelViewMatrix(size:));
- (matrix_float4x4)projectionMatrix:(CGSize)size NS_SWIFT_NAME(projectionMatrix(size:));
- (matrix_float4x4)transformForJoint:(NSString * _Nonnull)joint
                           character:(NSString * _Nonnull)character NS_SWIFT_NAME(transform(of:in:));
- (void)setGazeTarget:(simd_float3)target forKey:(NSString * _Nonnull)name;
- (simd_float3)gazeTargetForKey:(NSString * _Nonnull)name NS_SWIFT_NAME(gazeTarget(forKey:));
- (void)drawFrame:(CGSize)size
  modelViewMatrix:(matrix_float4x4)modelViewMatrix
 projectionMatrix:(matrix_float4x4)projectionMatrix NS_SWIFT_NAME(drawFrame(size:modelViewMatrix:projectionMatrix:));

/**
 Smartbody callback for the native platform to play a sound clip. It's made
 public so you can override the default implementation (AVAudioPlayer). If
 you override the method, you have to call the delegate methods youself.

 @param path the location of the sound clip
 @param loop if true, loop the clip playback
 */
- (void)playSoundFromFileAtPath:(NSString * _Nonnull)path loop:(BOOL)loop;

/**
 Smartbody callback for the native platform to stop the audio playback.
 It's made public so you can override the default implementation
 (AVAudioPlayer).
 */
- (void)stopSound;

@end
