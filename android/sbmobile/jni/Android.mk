LOCAL_SHORT_COMMANDS := true
SBM_ANDROID_LOCAL_PATH := $(call my-dir)
SBM_PATH := ../../../core/smartbody/SmartBody/
#include $(CLEAR_VARS)
include $(SBM_ANDROID_LOCAL_PATH)/../../smartbody/jni/Android.mk

TARGET_OUT=src/main/jniLibs/${TARGET_ARCH_ABI}

#ANDROIDINCLUDES=/home/ari/androidbuild/arm64-v8a/include
#ANDROIDLIBS=/home/ari/androidbuild/arm64-v8a/lib
ANDROIDINCLUDES=/home/ari/androidbuild/${TARGET_ARCH_ABI}/include
ANDROIDLIBS=/home/ari/androidbuild/${TARGET_ARCH_ABI}/lib

LOCAL_PATH = $(SBM_ANDROID_LOCAL_PATH)
include $(CLEAR_VARS)
SB_LIB_PATH := ../../../lib
LOCAL_MODULE    := libsbmobile
LOCAL_C_INCLUDES := $(ANDROIDINCLUDES) \
			$(ANDROIDINCLUDES)/boost-1_70 \
			$(ANDROIDINCLUDES)/python2.7 \
			$(LOCAL_PATH)/$(SB_LIB_PATH)/vhcl/include \
			$(LOCAL_PATH)/$(SB_LIB_PATH)/vhmsg/vhmsg-c/include \
			$(LOCAL_PATH)/$(SB_LIB_PATH)/boostnumeric \
			$(LOCAL_PATH)/$(SBM_PATH)/../steersuite-1.3/external/ \
			$(LOCAL_PATH)/$(SBM_PATH)/../steersuite-1.3/steerlib/include \
			$(LOCAL_PATH)/$(SBM_PATH)/../steersuite-1.3/pprAI/include \
			$(LOCAL_PATH)/$(SBM_PATH)/../../../android/include \
			$(LOCAL_PATH)/$(SBM_PATH)/src \
			/home/ari/smartbody/android/include/eigen \
					
LOCAL_CFLAGS    := -O3 -DSB_NO_ASSIMP=TRUE -DSB_NO_VHMSG=TRUE -DANDROID_BUILD -DBUILD_ANDROID -frtti -fexceptions  -DSB_NO_VHCL_AUDIO=TRUE -D__ANDROID__
LOCAL_SRC_FILES :=  SBMain.cpp Shader.cpp ImageTransfer.cpp esUtil.c AppListener.cpp SBWrapper.cpp SBMobile.cpp 
#LOCAL_LDLIBS    := -landroid -llog -lGLESv3 -L/home/ari/androidbuild/arm64-v8a/lib /home/ari/androidbuild/arm64-v8a/lib/libpython2.7.a -lboost_python27-clang-mt-a64-1_70
LOCAL_LDLIBS    := -landroid -llog -lGLESv3 -L${ANDROIDLIBS} 
#/home/ari/androidbuild/Python-2.7.16/build/lib.linux2-aarch64-2.7/pyexpat.so
#-L/home/ari/androidbuild/Python-2.7.16/build/lib.linux2-aarch64-2.7 pyexpat.so _functools.so cmath.so _random.so array.so strop.so itertools.so cStringIO.so cPickle.so math.so time.so datetime.so _collections.so operator.so _md5.so _sha.so _sha256.so _sha512.so select.so fcntl.so _struct.so _bisect.so parser.so binascii.so _elementtree.so
#LOCAL_STATIC_LIBRARIES := smartbody  xerces-prebuilt boost-filesystem-prebuilt boost-system-prebuilt boost-regex-prebuilt boost-python-prebuilt blas vhcl 
LOCAL_STATIC_LIBRARIES := boost-filesystem-prebuilt boost-system-prebuilt boost-regex-prebuilt blas vhcl vhmsg 
# lapack f2c apr-prebuilt apr-util-prebuilt expat-prebuilt  openal alut tremolo sndfile vhmsg
LOCAL_SHARED_LIBRARIES :=  smartbody
include $(BUILD_SHARED_LIBRARY) 
#include $(BUILD_STATIC_LIBRARY) 
