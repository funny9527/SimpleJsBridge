LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

subdirs := $(LOCAL_PATH)/jsc/Android.mk \
               $(LOCAL_PATH)/bridge/Android.mk

include $(subdirs)