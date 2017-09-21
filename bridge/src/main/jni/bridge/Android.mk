LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

JAVASCRIPTCORE_PATH := $(LOCAL_PATH)/../jsc

#LOCAL_C_INCLUDES := $(JAVASCRIPTCORE_PATH)

LOCAL_MODULE    := libbridge
LOCAL_SRC_FILES := init.cpp JsExecutor.cpp datamap.cpp

LOCAL_SHARED_LIBRARIES := libicu libjsc

LOCAL_LDLIBS := -llog -lz

include $(BUILD_SHARED_LIBRARY)