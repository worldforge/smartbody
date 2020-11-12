#ifndef __SB_MOBILE__
#define __SB_MOBILE__
#if defined(__ANDROID__)
#include <jni.h>
#endif
#include <vector>
#include <string>
#include <sr/sr_vec.h>

#include <sb/SBTypes.h>
void initSBMobilePythonModule();

class SBMobile
{
public:
	SBMobile();
	~SBMobile();

		
	// Video API
	void playVideo(std::string videoViewName, std::string videoFilePath, bool looping = false);
	void stopVideo(std::string videoViewName);
	
	// Sound API
	void playSound(std::string soundFilePath, bool looping = false);
    void handleCallback(std::string callbackMsg);
	void stopSound();

	void snapshotPNGResize(std::string imgFileName, int width, int height, int outW = -1, int outH = -1);
    
#if defined(__ANDROID__)
	void callJNIMethod(std::string className, std::string methodName, std::string methodDef, std::vector<std::string> parameters);		
	bool beforeCallJavaMethod( const std::string& className, const std::string& methodName, const std::string& methodDef, jclass& interfaceClass, jmethodID& method);	

	static JavaVM* jvm;
	static JNIEnv* env;
	static bool jvmIsAttached;
	
protected:
	jstring stringToJString(const std::string& str);
#endif
};



#endif
