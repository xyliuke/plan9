LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_system
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../boost-1_62/armeabi-v7a/lib/libboost_system-clang-darwin-mt-1_62.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_filesystem
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../boost-1_62/armeabi-v7a/lib/libboost_filesystem-clang-darwin-mt-1_62.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_thread
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../boost-1_62/armeabi-v7a/lib/libboost_thread-clang-darwin-mt-1_62.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := curl
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../../3rd/curl/android/armeabi-v7a/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := uv
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../../3rd/libuv-1.x/lib/android-v7a/libuv.a
include $(PREBUILT_STATIC_LIBRARY)

else ifeq ($(TARGET_ARCH_ABI), x86)


LOCAL_MODULE    := boost_system
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../boost-1_62/x86/lib/libboost_system.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_filesystem
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../boost-1_62/x86/lib/libboost_filesystem.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_thread
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../boost-1_62/x86/lib/libboost_thread.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := curl
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../../3rd/curl/android/x86/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := uv
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../../3rd/libuv-1.x/lib/android-x86/libuv.a
include $(PREBUILT_STATIC_LIBRARY)

endif




include $(CLEAR_VARS)

LOCAL_CFLAGS := -I$(LOCAL_PATH)/../../../../../boost-1_62/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../common/src/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../common/header/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../3rd/jsoncpp/dist/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../3rd/zlib/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../3rd/unzip/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../3rd/sqlite/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../3rd/cryptopp565/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../3rd/curl/android/include/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../3rd/libuv-1.x/android-include/
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../../../../../3rd/
LOCAL_CFLAGS += -D__ARM_ARCH_5__
LOCAL_CFLAGS += -DANDROID
LOCAL_CFLAGS += -lstdc++

MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/../../../../../../../common/src/*/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../../../../../../common/src/*/*.c)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../../../../../../3rd/jsoncpp/dist/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../../../../../../3rd/zlib/*.c)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../../../../../../3rd/unzip/*.c)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../../../../../../3rd/sqlite/*.c)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/../../../../../../../3rd/cryptopp565/*.cpp)

LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)

LOCAL_STATIC_LIBRARIES := boost_system boost_filesystem curl uv

LOCAL_MODULE := common
LOCAL_SRC_FILES += cn_gocoding_common_Bizlayer.cpp util_string.cpp
include $(BUILD_SHARED_LIBRARY) 


