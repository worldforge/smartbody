//
//  SBViewController.swift
//  Smartbody
//
//  Created by Anton Leuski on 9/23/17.
//  Copyright © 2017 Smartbody Project. All rights reserved.
//

import UIKit
import GLKit
import AVFoundation

extension GLKView {
  open var drawableSize: CGSize {
    return CGSize(width: drawableWidth, height: drawableHeight)
  }
}

extension SBPythonObject {
  public var intValue: Int? { return self.numberWithInt?.intValue }
  public var boolValue: Bool? { return self.numberWithBool?.boolValue }
  public var floatValue: Float? { return self.numberWithFloat?.floatValue }
}

open class SBViewController: GLKViewController, GLKViewControllerDelegate {
  private var _time: TimeInterval = 0
  private var _audioWasPlaying = false
  
  open var context: SBContext?
  
  open func makeOpenGLContext() -> EAGLContext? {
    return EAGLContext.init(api: .openGLES3)
  }
  
  open func setupOpenGL() {
    context?.reloadTexture()
  }

  open func teardownOpenGL() {    
  }
  
  open override func viewDidLoad() {
    super.viewDidLoad()
    self.delegate = self
    // Do any additional setup after loading the view, typically from a nib.
    isPaused = false
    
    guard let glview = view as? GLKView else {
      fatalError("view must be a subclass of GLKView")
    }
    
    guard let context = makeOpenGLContext() else {
      fatalError("failed to initialize OpenGL context")
    }
    
    glview.delegate = self;
    glview.drawableColorFormat = .RGBA8888
    glview.drawableDepthFormat = .format24
    glview.context = context
    glview.bindDrawable()

    setupOpenGL()
  }
  
  deinit {
    if let glview = self.view as? GLKView {
      glview.bindDrawable()
      teardownOpenGL()
    }
  }
  
  open override func glkView(_ view: GLKView, drawIn rect: CGRect) {
    glEnable(GLenum(GL_DEPTH_TEST))
    glClearColor(1.0, 0.0, 0.0, 1.0)
    glClear(GLbitfield(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT))
    context?.drawFrame(size: view.drawableSize)
  }

  open override func didReceiveMemoryWarning() {
    super.didReceiveMemoryWarning()
    // Dispose of any resources that can be recreated.
  }
  
  open override func viewDidLayoutSubviews() {
    super.viewDidLayoutSubviews()
  }
  
  open func glkViewController(_ controller: GLKViewController, willPause pause: Bool)
  {
    if pause {
      _time += timeSinceLastResume
      _audioWasPlaying = context?.audioPlayer?.isPlaying ?? false
      context?.audioPlayer?.pause()
    } else {
      if _audioWasPlaying {
        context?.audioPlayer?.play()
        _audioWasPlaying = false
      }
    }
  }
  
  open func glkViewControllerUpdate(_ controller: GLKViewController) {
    context?.update(_time+timeSinceLastResume)
  }
}

