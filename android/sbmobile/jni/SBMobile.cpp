#include "SBMobile.h"
#include "SBWrapper.h"
#include <vhcl.h>
#include <sb/SBUtilities.h>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sr/sr_plane.h>
#include <sr/sr_box.h>
#include <sr/sr_camera.h>
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

#include "external/stb/stb_image_write.h"
#include "external/stb/stb_image_resize.h"



#if defined(__ANDROID__)
#include <GLES3/gl3.h>
#elif defined(SB_IPHONE)
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

struct SBMobileWrap :  SBMobile, boost::python::wrapper<SBMobile>
{
	virtual bool eventScreenTouch(int action, float x, float y);	
	bool default_eventScreenTouch(int action, float x, float y)  { return SBMobile::eventScreenTouch(action, x, y); }		
};

void setSBMobile(SBMobile* inEngine) { SBMobile::setSBMobile(inEngine); }



bool SBMobileWrap::eventScreenTouch( int action, float x, float y )
{	
	if (boost::python::override o = this->get_override("eventScreenTouch"))
	{
		try {
			return o(action,x,y);
		} catch (...) {
			PyErr_Print();
		}
	}	
	
	return SBMobile::eventScreenTouch(action,x,y);		
}


void setEngine(SBMobile* inEngine) { SBMobile::setSBMobile(inEngine); }

BOOST_PYTHON_MODULE(SBMobile)
{	
	boost::python::def("setEngine", setEngine, "Set Engine");
	boost::python::class_<SBMobileWrap, boost::noncopyable>("SBMobile")	
		.def("playVideo", &SBMobile::playVideo, "Playback a video file from a specific video view.") // Video API
		.def("stopVideo", &SBMobile::stopVideo, "Stop the video playback.")			
		.def("playSound", &SBMobile::playSound, "Playback a sound file.") // Sound API
		.def("stopSound", &SBMobile::stopSound, "Stop the sound playback.")
		.def("engineCallback", &SBMobile::engineCallback, "Callbacks on the engine to indicate events generated from the simulation.")
		.def("convertScreenSpaceTo3D", &SBMobile::convertScreenSpaceTo3D, boost::python::return_value_policy<boost::python::return_by_value>(), "Event called when the screen is touched.")			
		.def("convertScreenSpaceTo3DAR", &SBMobile::convertScreenSpaceTo3DAR, boost::python::return_value_policy<boost::python::return_by_value>(), "Event called when the screen is touched.")
		.def("testCharacterIntersection", &SBMobile::testCharacterIntersection, boost::python::return_value_policy<boost::python::return_by_value>(), "Test intersection with a character.")						
		.def("snapshotPNGResize", &SBMobile::snapshotPNGResize, "Save the current image to png file.")
		.def("eventScreenTouch", &SBMobile::eventScreenTouch, &SBMobileWrap::default_eventScreenTouch, "Event called when the screen is touched.")
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
SBMobile* SBMobile::engine = NULL;
#endif

SBMobile::SBMobile()
{
	
}


SBMobile::~SBMobile()
{
	
}

void SBMobile::resize( int w, int h )
{
	screenWidth = w;
	screenHeight = h;	
	
}


void SBMobile::setTransformMatrces(SrMat& mv, SrMat& proj)
{
	modelViewMat = mv;
	projMat = proj;
}

std::string SBMobile::testCharacterIntersection(float x, float y, std::string charName)
{	
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBCharacter* sbChar = scene->getCharacter(charName);
	if (!sbChar) return "none";
	SmartBody::SBSkeleton* skel = sbChar->getSkeleton();
	SrVec rayPt = convertScreenSpaceTo3D(x,y,SrVec(0,0,0), SrVec(0,0,1));
	SrBox bbox = skel->getBoundingBox();	
	// check projection only in 2D
	bbox.a.z = 0.0;
	bbox.b.z = 0.0;	
	rayPt.z = 0.0;	
	//SmartBody::util::log("rayPt = %s, bbox min = %s, max = %s", rayPt.toString().c_str(), bbox.a.toString().c_str(), bbox.b.toString().c_str());
	if (!bbox.contains(rayPt)) return "none";

	skel->update_global_matrices();
	std::vector<SkJoint*>& joints = skel->get_joint_array();	
	float dist = 1e30;
	int closestIdx = -1;
	std::string closestJointName = "none";
	for (size_t j = 0; j < joints.size(); j++)
	{
		if (joints[j]->getJointType() != SkJoint::TypeJoint)
			continue;
		joints[j]->update_gmat();
		const SrMat& gmat = joints[j]->gmat();
		SrVec point(gmat.get(3, 0), gmat.get(3, 1), 0.0);
		//initialBoundingBox.extend(point);
		if ((point - rayPt).norm() < dist)
		{
			dist = (point - rayPt).norm();
			closestIdx = j;
		}
	}
	if (closestIdx >= 0 && closestIdx < joints.size())
		closestJointName = joints[closestIdx]->getMappedJointName();
	return closestJointName;
}

SrVec SBMobile::convertScreenSpaceTo3D(float x, float y, SrVec ground, SrVec upVector)
{
	SrVec p1;
	SrVec p2;
	float screenX, screenY;
	screenX = (float)(x*2.f)/screenWidth - 1.f;
	screenY = (float)(y*2.f)/screenHeight - 1.f;
	screenY *= -1.0;

	//LOG("convertScreenSpaceTo3D, screenX = %f, screenY = %f", x, y);
	SmartBody::SBScene::getScene()->getActiveCamera()->get_ray(screenX, screenY, p1, p2);
	//SmartBody::util::log("mouse click = %d, %d,   p1 = %f %f %f, p2 = %f %f %f", x,y, p1[0], p1[1], p1[2],  p2[0], p2[1], p2[2]);
	bool intersectGround = true;
	SrVec dest, src;				
	SrPlane plane(ground, upVector);
	dest = plane.intersect(p1, p2);
	//SmartBody::util::log("mouse click = %d, %d,   p1 = %f %f %f, p2 = %f %f %f", x, y, p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
	//SmartBody::util::log("touchPos = %f %f %f", dest[0], dest[1], dest[2]);
	return dest;
}


SrVec SBMobile::convertScreenSpaceTo3DAR(float x, float y, SrVec ground, SrVec upVector)
{
	float screenX, screenY;
	screenX = (float)(x*2.f) / screenWidth - 1.f;
	screenY = (float)(y*2.f) / screenHeight - 1.f;
	screenY *= -1.0;

	SrMat invModelView = modelViewMat.inverse();
	SrVec newUp = upVector*invModelView.get_rotation();
	SrMat invMVP = (modelViewMat*projMat).inverse();

	float nearPlane = 0.1f, farPlane = 100.f;

	SrVec p1 = SrVec(screenX, screenY, nearPlane);
	SrVec p2 = SrVec(screenX, screenY, farPlane);
	p1 = p1*invMVP;
	p2 = p2*invMVP;

	SrPlane plane(ground, newUp);
	SrVec dest = plane.intersect(p1, p2);
	//SmartBody::util::log("AR mouse click = %f, %f,   p1 = %f %f %f, p2 = %f %f %f", x, y, p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
	//SmartBody::util::log("AR touchPos = %f %f %f", dest[0], dest[1], dest[2]);
	return dest;

}

bool SBMobile::eventScreenTouch(int action, float x, float y)
{
	//LOG("touch screen event, action = %d, x = %f, y = %f", action, x, y);
	static bool firstTime = true;
	static float prevx = 0.f, prevy = 0.f;
	
	// normalize coordinate first
	x = x/screenWidth;
	y = y/screenHeight;

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();	
	if (action == 0) // action down
		firstTime = true;
	//SrCamera* cam = scene->getCamera("defaultCamera");
	//if (!cam) return false;
	float deltaX, deltaY;
	deltaX = x - prevx; 
	deltaY = y - prevy;
	if (firstTime)
	{
		deltaX = 0.f;
		deltaY = 0.f;
		firstTime = false;		
	}
	prevx = x; 
	prevy = y;
	
	SBCameraOperation(deltaX,deltaY);

	return true;
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
		//SmartBody::util::log("callback_handler: failed to get JNI environment, "
		//	"assuming native thread");
		status = jvm->AttachCurrentThread(&env, NULL);
		if(status < 0) {
			//SmartBody::util::log("callback_handler: failed to attach "
			//	"current thread");
			return resultJString;
		}
		isAttached = true;
	}
	resultJString = env->NewStringUTF(str.c_str());
	if(isAttached) jvm->DetachCurrentThread();

	return resultJString;
}



bool SBMobile::beforeCallJavaMethod( const std::string& className, const std::string& methodName, const std::string& methodDef, jclass& interfaceClass, jmethodID& method)
{
	//va_list args;
	//va_start(args, methodDef);

	
	int status;
	jvmIsAttached = false;

	status = jvm->GetEnv((void **) &env, JNI_VERSION_1_4);
	if(status < 0) {
		//SmartBody::util::log("callback_handler: failed to get JNI environment, "
		//	"assuming native thread");
		status = jvm->AttachCurrentThread(&env, NULL);
		if(status < 0) {
		//	SmartBody::util::log("callback_handler: failed to attach "
		//		"current thread");
			return false;
		}
		jvmIsAttached = true;
	}

	interfaceClass = env->FindClass(className.c_str());
	if(!interfaceClass) {
		//SmartBody::util::log("callback_handler: failed to get class reference");
		if(jvmIsAttached) jvm->DetachCurrentThread();
		return false;
	}
	/* Find the callBack method ID */
	method = env->GetStaticMethodID(
		interfaceClass, methodName.c_str(), methodDef.c_str());
	if(!method) {
		//SmartBody::util::log("callback_handler: failed to get method ID");
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
		parametersJStr.push_back(temp);
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

void SBMobile::engineCallback(std::string params)
{
	jstring paramsJStr = stringToJString(params);
	jclass interfaceClass;
	jmethodID methodID;
	bool jvmSuccess = beforeCallJavaMethod("edu/usc/ict/sbmobile/SBMobileLib","engineCallback","(Ljava/lang/String;)V", interfaceClass, methodID);

	if (jvmSuccess)
	{
		env->CallStaticVoidMethod(interfaceClass, methodID, paramsJStr);
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
		//SmartBody::util::log("Before stbi resizing");
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

		//SmartBody::util::log("before stbi write png");
		int save_result = stbi_write_png(imgFileName.c_str(), outW, outH, channels, resizeImg, outW*channels);
		//SmartBody::util::log("after stbi write png");
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










