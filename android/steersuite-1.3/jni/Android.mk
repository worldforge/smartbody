# log4cxx
#

STEER_LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include $(STEER_LOCAL_PATH)/../steerlib/jni/Android.mk

LOCAL_PATH = $(STEER_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE := pprAI
LOCAL_CFLAGS    := -DBUILD_ANDROID -frtti -fexceptions 
MY_DIR := ../../../core/smartbody/steersuite-1.3/pprAI
LOCAL_C_INCLUDES := $(STEER_LOCAL_PATH)/$(MY_DIR)/include \
					$(STEER_LOCAL_PATH)/$(MY_DIR)/../external \
					$(STEER_LOCAL_PATH)/$(MY_DIR)/../steerlib/include							
LOCAL_SRC_FILES := $(MY_DIR)/src/PPRAgent.cpp \
					$(MY_DIR)/src/PPRAIModule.cpp 		
LOCAL_STATIC_LIBRARIES := steerlib					

include $(BUILD_STATIC_LIBRARY)
