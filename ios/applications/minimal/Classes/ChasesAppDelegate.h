//
//  ChasesAppDelegate.h
//  Chases
//
//  Created by Linda MacPhee-Cobb on 3/24/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ChasesAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    UIViewController* m_rootViewController;
}

@property (nonatomic, strong) IBOutlet UIWindow *window;

@end

