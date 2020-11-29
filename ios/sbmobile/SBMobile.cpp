#include "SBMobile.h"
#include "SBUtilities.h"
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>


// Python Interface
#include <boost/python/suite/indexing/vector_indexing_suite.hpp> 
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/args.hpp>
#include <boost/python.hpp>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image_write.h"
#include "stb_image_resize.h"



#if defined(__ANDROID__)
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#else
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif


BOOST_PYTHON_MODULE(SBMobile)
{	
	boost::python::class_<SBMobile>("SBMobile")
		//.staticmethod("setEngine", &VHEngineWrap::setEngine, "Set the VHEngine singleton")
		// Video API
		.def("playVideo", &SBMobile::playVideo, "Playback a video file from a specific video view.")
		.def("stopVideo", &SBMobile::stopVideo, "Stop the video playback.")		
		// Sound API
		.def("playSound", &SBMobile::playSound, "Playback a sound file.")
		.def("stopSound", &SBMobile::stopSound, "Stop the sound playback.")
        .def("handleCallback", &SBMobile::handleCallback, "Handle generic callback with a string message")
		.def("snapshotPNGResize", &SBMobile::snapshotPNGResize, "Save the current image to png file.")
		;	
}

void initSBMobilePythonModule()
{
	initSBMobile();
}

#if !defined(SB_IPHONE)
JavaVM* SBMobile::jvm = NULL;
JNIEnv* SBMobile::env = NULL;
bool    SBMobile::jvmIsAttached = false;
#endif

SBMobile::SBMobile()
{
	
}


SBMobile::~SBMobile()
{
	
}

#if !defined(SB_IPHONE)
jstring SBMobile::stringToJString( const std::string& str )
{
	int status;
	JNIEnv *env;
	bool isAttached = false;
	jstring resultJString;
	status = jvm->GetEnv((void **) &env, JNI_VERSION_1_4);
	if(status < 0) {
		SmartBody::util::log("callback_handler: failed to get JNI environment, "
			"assuming native thread");
		status = jvm->AttachCurrentThread(&env, NULL);
		if(status < 0) {
			SmartBody::util::log("callback_handler: failed to attach "
				"current thread");
			return resultJString;
		}
		isAttached = true;
	}
	resultJString = env->NewStringUTF(str.c_str());
	if(isAttached) jvm->DetachCurrentThread();

	return resultJString;
}


void SBMobile::handleCallback(std::string callbackMsg)
{
    
}

bool SBMobile::beforeCallJavaMethod( const std::string& className, const std::string& methodName, const std::string& methodDef, jclass& interfaceClass, jmethodID& method)
{
	//va_list args;
	//va_start(args, methodDef);

	
	int status;
	jvmIsAttached = false;

	status = jvm->GetEnv((void **) &env, JNI_VERSION_1_4);
	if(status < 0) {
		SmartBody::util::log("callback_handler: failed to get JNI environment, "
			"assuming native thread");
		status = jvm->AttachCurrentThread(&env, NULL);
		if(status < 0) {
			SmartBody::util::log("callback_handler: failed to attach "
				"current thread");
			return false;
		}
		jvmIsAttached = true;
	}

	interfaceClass = env->FindClass(className.c_str());
	if(!interfaceClass) {
		SmartBody::util::log("callback_handler: failed to get class reference");
		if(jvmIsAttached) jvm->DetachCurrentThread();
		return false;
	}
	/* Find the callBack method ID */
	method = env->GetStaticMethodID(
		interfaceClass, methodName.c_str(), methodDef.c_str());
	if(!method) {
		SmartBody::util::log("callback_handler: failed to get method ID");
		if(jvmIsAttached) jvm->DetachCurrentThread();
		return false;
	}

	//SmartBody::util::log("interfaceClass = %d, method = %d", interfaceClass, method);
	//jstring jname = env->NewStringUTF(widgetName.c_str());
	//env->CallStaticVoidMethod(interfaceClass, method, args);
	//if(jvmIsAttached) jvm->DetachCurrentThread();
	return true;
}

void SBMobile::callJNIMethod(std::string className, std::string methodName, std::string methodDef, std::vector<std::string> parameters)
{
	std::vector<jstring> parametersJStr;
	for (unsigned int i=0;i<parameters.size();i++)
	{
		jstring temp = stringToJString(parameters[i]);
		parametersJStr.emplace_back(temp);
	}
	int sizeOfParameters = parameters.size();

	jclass interfaceClass;
	jmethodID methodID;
	bool jvmSuccess = beforeCallJavaMethod(className,methodName,methodDef, interfaceClass, methodID);

	if (jvmSuccess)
	{
		if (sizeOfParameters == 0)
			env->CallStaticVoidMethod(interfaceClass, methodID);
		else if (sizeOfParameters == 1)
			env->CallStaticVoidMethod(interfaceClass, methodID, parametersJStr[0]);
		else if (sizeOfParameters == 2)
			env->CallStaticVoidMethod(interfaceClass, methodID, parametersJStr[0], parametersJStr[1]);
		else if (sizeOfParameters == 3)
			env->CallStaticVoidMethod(interfaceClass, methodID, parametersJStr[0], parametersJStr[1], parametersJStr[2]);
		else if (sizeOfParameters == 4)
			env->CallStaticVoidMethod(interfaceClass, methodID, parametersJStr[0], parametersJStr[1], parametersJStr[2], parametersJStr[3]);
		else if (sizeOfParameters >= 5) // only support up to 5 string parameters
			env->CallStaticVoidMethod(interfaceClass, methodID, parametersJStr[0], parametersJStr[1], parametersJStr[2], parametersJStr[3], parametersJStr[4]);
		
		if(jvmIsAttached) jvm->DetachCurrentThread();
	}
}


void SBMobile::playVideo(std::string videoViewName, std::string videoFilePath, bool looping)
{
	jstring videoViewNameJStr = stringToJString(videoViewName);
	jstring videoFilePathJStr = stringToJString(videoFilePath);
	
	jclass interfaceClass;
	jmethodID methodID;
	bool jvmSuccess = beforeCallJavaMethod("edu/usc/ict/sbmobile/SBMobileLib","playVideo","(Ljava/lang/String;Ljava/lang/String;Z)V", interfaceClass, methodID);

	if (jvmSuccess)
	{
		env->CallStaticVoidMethod(interfaceClass, methodID, videoViewNameJStr, videoFilePathJStr, looping);
		if(jvmIsAttached) jvm->DetachCurrentThread();
	}
}

void SBMobile::stopVideo(std::string videoViewName)
{
	jstring videoViewNameJStr = stringToJString(videoViewName);
	jclass interfaceClass;
	jmethodID methodID;
	bool jvmSuccess = beforeCallJavaMethod("edu/usc/ict/sbmobile/SBMobileLib","stopVideo","(Ljava/lang/String;)V", interfaceClass, methodID);

	if (jvmSuccess)
	{
		env->CallStaticVoidMethod(interfaceClass, methodID, videoViewNameJStr);
		if(jvmIsAttached) jvm->DetachCurrentThread();
	}
}

void SBMobile::playSound(std::string soundFilePath, bool looping)
{
	jstring soundFilePathJStr = stringToJString(soundFilePath);
	
	jclass interfaceClass;
	jmethodID methodID;
	bool jvmSuccess = beforeCallJavaMethod("edu/usc/ict/sbmobile/SBMobileLib","playSound","(Ljava/lang/String;Z)V", interfaceClass, methodID);

	if (jvmSuccess)
	{
		env->CallStaticVoidMethod(interfaceClass, methodID, soundFilePathJStr, looping);
		if(jvmIsAttached) jvm->DetachCurrentThread();
	}
}

void SBMobile::stopSound()
{
	jclass interfaceClass;
	jmethodID methodID;
	bool jvmSuccess = beforeCallJavaMethod("edu/usc/ict/sbmobile/SBMobileLib","stopSound","()V", interfaceClass, methodID);

	if (jvmSuccess)
	{
		env->CallStaticVoidMethod(interfaceClass, methodID);
		if(jvmIsAttached) jvm->DetachCurrentThread();
	}
}

#endif

void SBMobile::snapshotPNGResize(std::string imgFileName, int width, int height, int outW, int outH)
{
	int channels = 3;

	GLubyte *image = (GLubyte *) malloc(width * height * sizeof(GLubyte) * channels);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);


	GLubyte *resizeImg = NULL;
	if (outW != -1 && outH != -1)
	{
		// resize image
		resizeImg = (GLubyte *) malloc(outW * outH * sizeof(GLubyte) * channels);
		SmartBody::util::log("Before stbi resizing");
		int resize_result = stbir_resize_uint8(image, width, height, 0,  resizeImg, outW, outH, 0, channels);

		for(int j = 0; j*2 < outH; ++j )
	    {
	        int index1 = j * outW * channels;
	        int index2 = (outH - 1 - j) * outW * channels;
	        for(int i = outW * channels ; i > 0; --i )
	        {
	            unsigned char temp = resizeImg[index1];
	            resizeImg[index1] = resizeImg[index2];
	            resizeImg[index2] = temp;
	            ++index1;
	            ++index2;			
	        }
	    }

		SmartBody::util::log("before stbi write png");
		int save_result = stbi_write_png(imgFileName.c_str(), outW, outH, channels, resizeImg, outW*channels);
		SmartBody::util::log("after stbi write png");
		free(resizeImg);
	}
	else
	{
		// output img directly
		int save_result = stbi_write_png(imgFileName.c_str(), width, height, channels, image, width*channels);
	}

	
#if 0
	int save_result = SOIL_save_image
	(
		path.c_str(),
		SOIL_SAVE_TYPE_TGA,
		width, height, channels,
		image
	);
#endif

	free(image);
}










