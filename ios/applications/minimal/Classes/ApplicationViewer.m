//
//  ApplicationViewer.c
//  Chases
//
//  Created by Yuyu Xu on 8/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "ApplicationViewer.h"
#import "EAGLView.h"
#import "minimalwrapper.h"

@implementation ApplicationViewController
@synthesize glView;
@synthesize text;
@synthesize segmentedControl;
@synthesize toggleAliasing;

- (void)viewDidLoad 
{
    [super viewDidLoad];	
    glView.animationInterval = 2.0 / 60.0;
	[glView startAnimation];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    [glView stopAnimation];
}

- (void)dealloc 
{
	[self viewDidUnload];
}


- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	[text resignFirstResponder];
	return YES;
}

- (IBAction)changeCommand
{
    const char* command = [text.text UTF8String];
    printf("command is %s \n", command);
    SBExecutePythonCmd(command);
}

- (IBAction)segmentedControlIndexChanged
{
    int id = segmentedControl.selectedSegmentIndex;
    cameraMode = id;
    if (id == 2)    // reset camera
    {
        SBCameraOperation(0, 0);
        SBExecutePythonCmd("bml.interruptCharacter(\"ChrRachel\",0)");
    }
    else
        SBExecuteCmd("bml char ChrRachel file \"./Sounds/9.xml\"");
}

- (IBAction)turnOnAntiAliasing
{
    if (toggleAliasing.on)
    {
        glView->antialiasing = YES;
    }
    else
    {
        glView->antialiasing = NO;
    }
}

@end