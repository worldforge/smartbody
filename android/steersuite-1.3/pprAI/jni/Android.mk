# log4cxx
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include $(LOCAL_PATH)/../../steerlib/jni/Android.mk

LOCAL_MODULE := steerlib
LOCAL_SRC_FILES := /../../steerlib/obj/local/arm64-v8a/libsteerlib.a
include $(PREBUILT_SHARED_LIBRARY)

LOCAL_MODULE := pprAI
LOCAL_CFLAGS    := -DBUILD_ANDROID
MY_DIR := ../../../core/smartbody/steersuite-1.3/pprAI
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(MY_DIR)/include \
					$(LOCAL_PATH)/../external \
					$(LOCAL_PATH)/../steerlib/include \
					$(LOCAL_PATH)/../external 				
LOCAL_SRC_FILES := $(MY_DIR)/src/PPRAgent.cpp \
					$(MY_DIR)/src/PPRAIModule.cpp 		
LOCAL_SHARED_LIBRARIES := steerlib					

include $(BUILD_SHARED_LIBRARY)
