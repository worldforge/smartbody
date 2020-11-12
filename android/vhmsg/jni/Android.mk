# log4cxx
#
VHMSG_LOCAL_PATH := $(call my-dir)
LOCAL_PATH = $(VHMSG_LOCAL_PATH)

ANDROID_LIB_DIR := /home/ari/androidbuild/${TARGET_ARCH_ABI}/lib

#include $(CLEAR_VARS)
#LOCAL_MODULE := activemq-prebuilt
#LOCAL_SRC_FILES := $(ANDROID_LIB_DIR)/libactivemq-cpp.a
#include $(PREBUILT_STATIC_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := apr-prebuilt
#LOCAL_SRC_FILES := $(ANDROID_LIB_DIR)/libapr.a
#include $(PREBUILT_STATIC_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := apr-util-prebuilt
#LOCAL_SRC_FILES := $(ANDROID_LIB_DIR)/libaprutil-1.a
#include $(PREBUILT_STATIC_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := expat-prebuilt
#LOCAL_SRC_FILES := $(ANDROID_LIB_DIR)/libexpat.a
#include $(PREBUILT_STATIC_LIBRARY)

include $(LOCAL_PATH)/../../vhcl/jni/Android.mk

LOCAL_PATH = $(VHMSG_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE := vhmsg
TARGET_PLATFORM := android-21
#MY_VHMSG_DIR := ../vhmsg_src/
MY_VHMSG_DIR := ../../../lib/vhmsg/vhmsg-c/
LOCAL_CFLAGS    := -DBUILD_ANDROID -frtti -fexceptions 
LOCAL_C_INCLUDES := $(VHMSG_LOCAL_PATH)/$(MY_VHMSG_DIR)/include $(VHMSG_LOCAL_PATH)/$(MY_VHMSG_DIR)/../../vhcl/include \
					$(VHMSG_LOCAL_PATH)/../../include/activemq-cpp/include
LOCAL_SRC_FILES := $(MY_VHMSG_DIR)/src/HttpUtility.cpp \
				   $(MY_VHMSG_DIR)/src/vhmsg-tt.cpp \
				   $(MY_VHMSG_DIR)/src/vhmsg.cpp				   

LOCAL_STATIC_LIBRARIES := vhcl 
#LOCAL_STATIC_LIBRARIES := vhcl activemq-prebuilt apr-prebuilt apr-util-prebuilt expat-prebuilt
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)


