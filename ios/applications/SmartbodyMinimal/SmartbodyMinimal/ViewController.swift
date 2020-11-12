//
//  ViewController.swift
//  SmartbodyMinimal
//
//  Created by Anton Leuski on 9/9/17.
//

import UIKit
import Smartbody

class ViewController: SBViewController {
  
  private class Delegate: NSObject, SBContextDelegate {
    func log(_ message: String) {
      Swift.print(message)
    }
  }
  
	private static let _context: SBContext = {
    guard let assetURL
      = Bundle.main.url(forResource: "assets", withExtension: nil)
      else {
        fatalError("assets directory must be present in the bundle")
    }
    return SBContext(assetsURL: assetURL, delegate: Delegate())
  }()
  
  override func viewDidLoad() {
    self.context = ViewController._context
    super.viewDidLoad()
  }
}

class SceneController: ViewController, UITextFieldDelegate {
  
  override func viewDidLoad() {
    super.viewDidLoad()
    
    let pinch = UIPinchGestureRecognizer(target: self, action: #selector(SceneController.pinchRecognized(_:)))
    view.addGestureRecognizer(pinch)

    let pan = UIPanGestureRecognizer(target: self, action: #selector(SceneController.panRecognized(_:)))
    pan.minimumNumberOfTouches = 1
    pan.maximumNumberOfTouches = 1
    view.addGestureRecognizer(pan)
  }

  private var _lastScale: CGFloat = 0
  private var _lastRotation = CGPoint()
  
  @objc func pinchRecognized(_ recognizer: UIPinchGestureRecognizer) {
    if recognizer.state == .changed {
      let delta = 100 * (recognizer.scale - _lastScale)
      _lastScale = recognizer.scale
      print("scale \(delta)")
      context?.cameraOperation(dx: Float(delta), dy: Float(delta), mode: 0)
    }
  }

  @objc func panRecognized(_ recognizer: UIPanGestureRecognizer) {
    if recognizer.state == .changed {
      let location = recognizer.translation(in: view)
      let dx = location.x - _lastRotation.x
      let dy = location.y - _lastRotation.y
      _lastRotation = location
      print("rotate \(dx) \(dy)")
      context?.cameraOperation(dx: Float(dx), dy: Float(dy), mode: 1)
    }
  }

  @IBAction func resetCamera(_ sender: Any) {
    context?.cameraOperation(dx: 0, dy: 0, mode: 2)
    context?.execute(pythonCommand: "bml.interruptCharacter(\"ChrRachel\",0)")
  }
  
  func textFieldShouldReturn(_ textField: UITextField) -> Bool
  {
    textField.resignFirstResponder()
    return true
  }
  
  @IBAction func commandEditingDidEnd(_ sender: UITextField) {
    guard let text = sender.text else { return }
    print("command is \(text)")
    context?.execute(pythonCommand: text)
  }
}
