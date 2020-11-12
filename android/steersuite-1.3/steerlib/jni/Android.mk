# log4cxx
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := steerlib
LOCAL_CFLAGS    := -DBUILD_ANDROID -frtti -fexceptions 
MY_DIR := ../../../../core/smartbody/steersuite-1.3/steerlib
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(MY_DIR)/include \
		    $(LOCAL_PATH)/$(MY_DIR)/../external \
    		    $(LOCAL_PATH)/$(MY_DIR)/../pprAI/include
LOCAL_SRC_FILES := $(MY_DIR)/src/AgentMetricsCollector.cpp \
		$(MY_DIR)/src/BenchmarkEngine.cpp \
		$(MY_DIR)/src/BoxObstacle.cpp \
		$(MY_DIR)/src/Camera.cpp \
		$(MY_DIR)/src/CircleObstacle.cpp \
		$(MY_DIR)/src/Clock.cpp \
		$(MY_DIR)/src/CommandLineParser.cpp \
		$(MY_DIR)/src/CompositeTechnique01.cpp \
		$(MY_DIR)/src/CompositeTechnique02.cpp \
		$(MY_DIR)/src/DrawLib.cpp \
		$(MY_DIR)/src/DynamicLibrary.cpp \
		$(MY_DIR)/src/GridDatabase2D.cpp \
		$(MY_DIR)/src/HighResCounter.cpp \
		$(MY_DIR)/src/MemoryMapper.cpp \
		$(MY_DIR)/src/Misc.cpp \
		$(MY_DIR)/src/OrientedBoxObstacle.cpp \
		$(MY_DIR)/src/PerformanceProfiler.cpp \
		$(MY_DIR)/src/RecFilePlayerModule.cpp \
		$(MY_DIR)/src/RecFileReader.cpp \
		$(MY_DIR)/src/RecFileWriter.cpp \
		$(MY_DIR)/src/SimulationEngine.cpp \
		$(MY_DIR)/src/SimulationMetricsCollector.cpp \
		$(MY_DIR)/src/SimulationOptions.cpp \
		$(MY_DIR)/src/SimulationRecorderModule.cpp \
		$(MY_DIR)/src/StateMachine.cpp \
		$(MY_DIR)/src/SteeringCommand.cpp \
		$(MY_DIR)/src/TestCasePlayerModule.cpp \
		$(MY_DIR)/src/TestCaseReader.cpp \
		$(MY_DIR)/src/TestCaseReaderPrivate.cpp \
		$(MY_DIR)/src/TestCaseWriter.cpp \
		$(MY_DIR)/src/ThreadedTaskManager.cpp \
		$(MY_DIR)/src/XMLParser.cpp \
		$(MY_DIR)/../external/tinyxml/ticpp.cpp \
		$(MY_DIR)/../external/tinyxml/tinystr.cpp \
		$(MY_DIR)/../external/tinyxml/tinyxml.cpp \
		$(MY_DIR)/../external/tinyxml/tinyxmlerror.cpp \
		$(MY_DIR)/../external/tinyxml/tinyxmlparser.cpp 

include $(BUILD_STATIC_LIBRARY)
