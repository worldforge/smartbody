//
//  ApplicationViewer.h
//  Chases
//
//  Created by Yuyu Xu on 8/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EAGLView;

@interface ApplicationViewController : UIViewController < UITextFieldDelegate >
{
    EAGLView *glView;
	UIViewController* m_currentController;
    IBOutlet UITextField *text;    
    UISegmentedControl *segmentedControl;
    UISwitch *toggleAliasing;
}

@property (nonatomic, strong) IBOutlet EAGLView* glView;
@property (nonatomic, strong) IBOutlet UITextField* text;
@property (nonatomic, strong) IBOutlet UISegmentedControl* segmentedControl;
@property (nonatomic, strong) IBOutlet UISwitch* toggleAliasing;


- (IBAction)changeCommand;
- (IBAction)segmentedControlIndexChanged;
- (IBAction)turnOnAntiAliasing;

@end

