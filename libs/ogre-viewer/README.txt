                      SmartBody OgreTest
                    ======================

OgreTest is a test of driving Ogre3D skeletons with SmartBody animation
via the BoneBus protocol.  It is also a very early reference
implementation we would like to use for SBM development, replacing our
(USC's) current proprietary Unreal Engine based renderer.

Known problems:
* You may need to download the DirectX 9 SDK from Microsoft to run it
  (the SDK is not redistributable).
* Application takes over the mouse (uses FPS-style mouse-look navigation)
* Resources must exist in the current directory.
* Loading resources (like loading your first character) cause a
  significant delay that interrupts frame rendering.
* All characters use the Brad model.
* The Brad model has not been tested for face animation.
* Characters & floor plane are not aligned to each other.

Any help dedicating a bit of time to any of these issues would be
greatly appreciated.


For detailed documentation on the SmartBody project, please see the
SmartBody website:
  http://smartbody.ict.usc.edu/


SmartBody was built by University of Southern California's Institute for
Creative Technologies and Information Sciences Institute.
