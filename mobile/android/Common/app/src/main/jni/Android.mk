LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_system
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../boost-1_55/lib/libboost_system-gcc-mt-1_55.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_filesystem
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../boost-1_55/lib/libboost_filesystem-gcc-mt-1_55.a
include $(PREBUILT_STATIC_LIBRARY)





include $(CLEAR_VARS)

LOCAL_CFLAGS := -I$(LOCAL_PATH)/../../../../../boost-1_55/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../common/src/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../common/header/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../3rd/jsoncpp/dist
LOCAL_CFLAGS += -D__ARM_ARCH_5__
LOCAL_CFLAGS += -DANDROID
LOCAL_CFLAGS += -lstdc++

MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/../../../../../../../common/src/*/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../../../../../../common/src/*/*.c)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../../../../../../3rd/jsoncpp/dist/*.cpp)

LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)

LOCAL_STATIC_LIBRARIES := boost_system boost_filesystem

LOCAL_MODULE := common
LOCAL_SRC_FILES += cn_gocoding_common_Bizlayer.cpp util_string.cpp
include $(BUILD_SHARED_LIBRARY) 


