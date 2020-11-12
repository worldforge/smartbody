//
//  InputViewController.m
//  Test
//
//  Created by Yuyu Xu on 9/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "InputViewController.h"
#include "OgreFramework.h"
#include "vhmsg-tt.h"
using namespace Ogre; 

@implementation InputViewController

- (CGFloat)distanceBetweenTwoPoints: (CGPoint)fromPoint toPoint: (CGPoint)toPoint
{
    
    float x = toPoint.x - fromPoint.x;
    float y = toPoint.y - fromPoint.y;
    return sqrt(x * x + y * y);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
    NSSet *allTouches = [event allTouches];
    switch ([allTouches count]) 
    {
        case 1: { //Single touch
            
            //Get the first touch.
            UITouch *touch = [[allTouches allObjects] objectAtIndex:0];
            
            switch ([touch tapCount])
            {
                case 1: //Single Tap.
                {
                    
                } break;
                case 2: {//Double tap. 
                    
                } break;
            }
        } break;
        case 2: { //Double Touch
            
            
            //Track the initial distance between two fingers.
            UITouch *touch1 = [[allTouches allObjects] objectAtIndex:0];
            UITouch *touch2 = [[allTouches allObjects] objectAtIndex:1];
            
            
            initialDistance = [self distanceBetweenTwoPoints:[touch1 locationInView:self] 
                                                     toPoint:[touch2 locationInView:self]];
            prevFov = OgreFramework::getSingletonPtr()->m_pCamera->getFOVy().valueRadians();
            
        } break;
        default:
            break;  
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event 
{
    NSSet *allTouches = [event allTouches];
    
    switch ([allTouches count])
    {
        case 1: {
            UITouch *touch = [[touches allObjects] objectAtIndex:0];
            CGPoint currentLoc = [touch locationInView:self];
            CGPoint prevLoc = [touch previousLocationInView:self];
            float origTransX = currentLoc.x - prevLoc.x;
            float origTransY = currentLoc.y - prevLoc.y;
            OgreFramework::getSingletonPtr()->m_pCamera->yaw(Degree(origTransX * -0.1));
            OgreFramework::getSingletonPtr()->m_pCamera->pitch(Degree(origTransY * -0.1));  
        } break;
        case 2: {
           
            UITouch *touch1 = [[allTouches allObjects] objectAtIndex:0];
            UITouch *touch2 = [[allTouches allObjects] objectAtIndex:1];
            
            //Calculate the distance between the two fingers.
            
            
            CGFloat finalDistance = [self distanceBetweenTwoPoints:[touch1 locationInView:self]
                                                   toPoint:[touch2 locationInView:self]];	
            
            float diff = initialDistance - finalDistance;
            diff *= 0.001;
            float camera = diff + prevFov;
            if (camera > 0.1 && camera < 1.8)
                OgreFramework::getSingletonPtr()->m_pCamera->setFOVy(Ogre::Radian(camera));
        } break;
    }    
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField 
{
    [textField resignFirstResponder];
    return NO;
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{
    const char* cString = [textField.text UTF8String];         
    OgreFramework::getSingletonPtr()->m_sbmDLL->ProcessVHMsgs("sbm", cString);
}

@end