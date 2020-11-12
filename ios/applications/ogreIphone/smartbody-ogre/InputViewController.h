//
//  InputViewController.h
//  Test
//
//  Created by Yuyu Xu on 9/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#import <UIKit/UIKit.h>
@interface InputViewController : UIView <UITextFieldDelegate>
{
    CGFloat initialDistance;
    CGFloat prevFov;
}
@end