//
//  EAGLView.m
//  Chases
//
//  Created by Linda MacPhee-Cobb on 3/24/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//



#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"

#define USE_DEPTH_BUFFER 1

// A class extension to declare private methods
@interface EAGLView ()



- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end


@implementation EAGLView

@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;


// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}


//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
    
    if ((self = [super initWithCoder:coder])) {
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context]) {
            return nil;
        }

        antialiasing = YES;
        time = 0.0;
		[self setupView];
    }
    return self;
}

- (void)drawView 
{
    [EAGLContext setCurrentContext:context];

    if (antialiasing)
    {
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, msaaFramebuffer);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    CGRect rect = [[UIScreen mainScreen] bounds];
	width = rect.size.width;
	height = rect.size.height;
    SBDrawFrame(width, height);
    
    if (!antialiasing)
    {
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
        [context presentRenderbuffer:GL_RENDERBUFFER_OES];
    }
    else
    {
        // Apple (and the khronos group) encourages you to discard depth
        // render buffer contents whenever is possible
         GLenum attachments[] = {GL_DEPTH_ATTACHMENT_OES};
         glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, attachments);
        
        //Bind both MSAA and View FrameBuffers.
        glBindFramebufferOES(GL_READ_FRAMEBUFFER_APPLE, msaaFramebuffer);
        glBindFramebufferOES(GL_DRAW_FRAMEBUFFER_APPLE, viewFramebuffer);
        
        // Call a resolve to combine both buffers
        glResolveMultisampleFramebufferAPPLE();
        
        // Present final image to screen
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
        [context presentRenderbuffer:GL_RENDERBUFFER_OES];
    }
}


- (void)update
{
    time += animationInterval;
    SBUpdate(time);
    [self drawView];
}

- (void)layoutSubviews {
    // prepare frame buffer
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    [self update];
}


- (BOOL)createFramebuffer {
    
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }

    //Generate our MSAA Frame and Render buffers
    glGenFramebuffersOES(1, &msaaFramebuffer);
    glGenRenderbuffersOES(1, &msaaRenderBuffer);
    
    //Bind our MSAA buffers
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, msaaFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, msaaRenderBuffer);
    
    // Generate the msaaDepthBuffer.
    // 4 will be the number of pixels that the MSAA buffer will use in order to make one pixel on the render buffer.
    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER_OES, 4, GL_RGB5_A1_OES, backingWidth, backingHeight);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, msaaRenderBuffer);
    glGenRenderbuffersOES(1, &msaaDepthBuffer);
    
    //Bind the msaa depth buffer.
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, msaaDepthBuffer);
    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER_OES, 4, GL_DEPTH_COMPONENT16_OES, backingWidth , backingHeight);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, msaaDepthBuffer);

    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
    
    return YES;
}


- (void)destroyFramebuffer {
    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    glDeleteFramebuffersOES(1, &msaaFramebuffer);
    msaaFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &msaaRenderBuffer);
    msaaRenderBuffer = 0;
    
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
    if(msaaDepthBuffer)
    {
        glDeleteRenderbuffersOES(1, &msaaDepthBuffer);
        msaaDepthBuffer = 0;
    }
}


- (void)startAnimation {
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(update) userInfo:nil repeats:YES];
}


- (void)stopAnimation {
    self.animationTimer = nil;
}


- (void)setAnimationTimer:(NSTimer *)newTimer {
    [animationTimer invalidate];
    animationTimer = newTimer;
}


- (void)setAnimationInterval:(NSTimeInterval)interval {
    
    animationInterval = interval;
    if (animationTimer) {
        [self stopAnimation];
        [self startAnimation];
    }
}


-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch = [touches anyObject];
	startPoint = [touch locationInView:self];
    prevPoint = startPoint;
}


-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch = [touches anyObject];
	endPoint = [touch locationInView:self];
    CGPoint diff;
    diff.x = endPoint.x - startPoint.x;
    diff.y = endPoint.y - startPoint.y;    
}


-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch* touch = [[event touchesForView:self] anyObject];
	currentPoint = [touch previousLocationInView:self];	
    CGPoint diff;
    diff.x = currentPoint.x - prevPoint.x;
    diff.y = currentPoint.y - prevPoint.y;
    prevPoint = currentPoint;
    
    SBCameraOperation(diff.x, diff.y);
}


- (void)setupView
{   
	// get screen dimensions
	CGRect rect = [[UIScreen mainScreen] bounds];
	width = rect.size.width;
	height = rect.size.height;

    static bool once = true;
    if (once)
    {
        
        NSString *dirPath = [[[NSBundle mainBundle] resourcePath]
                             stringByAppendingPathComponent:@"assests"];
        const char* mediaPath = [dirPath UTF8String];
        SBInitialize(mediaPath);
        SBSetupDrawing(width, height);
        once = false;
    }
}


- (void)dealloc {
    
    [self stopAnimation];
    
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
    
}

@end
