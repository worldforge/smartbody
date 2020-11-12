# log4cxx
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := bonebus
TARGET_PLATFORM := android-9
MY_DIR := ../../../lib/bonebus
LOCAL_CFLAGS    := -DBUILD_ANDROID
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(MY_DIR)/include $(LOCAL_PATH)/../../vhcl/vhcl_src
LOCAL_SRC_FILES := $(MY_DIR)/src/bonebus.cpp \
				   $(MY_DIR)/src/bonebus-c.cpp				   
include $(BUILD_STATIC_LIBRARY)
