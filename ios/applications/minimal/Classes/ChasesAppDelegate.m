//
//  ChasesAppDelegate.m
//  Chases
//
//  Created by Linda MacPhee-Cobb on 3/24/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "ChasesAppDelegate.h"
#import "EAGLView.h"
#import "ApplicationViewer.h"
#import <sys/utsname.h>
#import <QuartzCore/QuartzCore.h>

@implementation ChasesAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(UIApplication *)application
{    
    UIDevice* thisDevice = [UIDevice currentDevice];
    
    if(thisDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad)
        m_rootViewController = [[ApplicationViewController alloc] initWithNibName:@"Window-iPad" bundle:nil];
    else
        m_rootViewController = [[ApplicationViewController alloc] initWithNibName:@"Window" bundle:nil];

    
	[window addSubview:m_rootViewController.view];
    [window makeKeyAndVisible]; 
}

- (void) applicationWillResignActive:(UIApplication *)application
{
    EAGLView* glView = (EAGLView*)m_rootViewController.view;
    if (glView != nil)
        [glView stopAnimation];
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
    EAGLView* glView = (EAGLView*)m_rootViewController.view;
    if (glView != nil)
        [glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    EAGLView* glView = (EAGLView*)m_rootViewController.view;
    if (glView != nil)
        [glView stopAnimation];
}

@end
