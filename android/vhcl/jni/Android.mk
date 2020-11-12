# log4cxx
#

VHCL_LOCAL_PATH := $(call my-dir)
VHCL_SRC_DIR := ./src
#include $(VHCL_LOCAL_PATH)/../../openal/android/jni/Android.mk
#include $(VHCL_LOCAL_PATH)/../../libsndfile/jni/Android.mk

LOCAL_PATH = $(VHCL_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE := vhcl
TARGET_PLATFORM := android-9
#MY_VHCL_DIR := ../vhcl_src/
MY_VHCL_DIR := ../../../lib/vhcl/
UPDATE_VHCL_DIR := ../
LOCAL_CFLAGS    := -DBUILD_ANDROID -frtti -fexceptions
#LOCAL_C_INCLUDES := $(VHCL_LOCAL_PATH)/$(MY_VHCL_DIR)/include/ $(VHCL_LOCAL_PATH)/$(MY_VHCL_DIR)/openal/include $(VHCL_LOCAL_PATH)/$(MY_VHCL_DIR)/libsndfile/include
LOCAL_C_INCLUDES := $(VHCL_LOCAL_PATH)/$(MY_VHCL_DIR)/include/ 
LOCAL_SRC_FILES := $(MY_VHCL_DIR)/$(VHCL_SRC_DIR)/Pow2Assert.cpp \
				$(MY_VHCL_DIR)/$(VHCL_SRC_DIR)/vhcl_log.cpp \
				$(MY_VHCL_DIR)/$(VHCL_SRC_DIR)/vhcl_string.cpp \
				$(MY_VHCL_DIR)/$(VHCL_SRC_DIR)/vhcl_timer.cpp \
				$(MY_VHCL_DIR)/$(VHCL_SRC_DIR)/vhcl_memory.cpp \
				$(MY_VHCL_DIR)/$(VHCL_SRC_DIR)/vhcl_socket.cpp \
				$(MY_VHCL_DIR)/$(VHCL_SRC_DIR)/vhcl_crash.cpp  \
				$(MY_VHCL_DIR)/$(VHCL_SRC_DIR)/vhcl_math.cpp  \
				$(MY_VHCL_DIR)/$(VHCL_SRC_DIR)/vhcl_audio.cpp
LOCAL_LDLIBS    := -llog	
#LOCAL_STATIC_LIBRARIES := openal sndfile alut
LOCAL_STATIC_LIBRARIES := 

#$(MY_VHCL_DIR)/src/vhcl_audio.cpp \
				   
include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)
