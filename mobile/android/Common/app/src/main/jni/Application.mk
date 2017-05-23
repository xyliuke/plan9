#APP_ABI := armeabi armeabi-v7a x86
APP_ABI := x86
NDK_TOOLCHAIN_VERSION := clang
# NDK_TOOLCHAIN_VERSION := gcc
NDK_TOOLCHAIN_VERSION := 4.9
TARGET_PLATFORM := android-21
APP_OPTIM := release
# APP_PLATFORM := android-21
# LIBCXX_FORCE_REBUILD := true
# x86时选择 gunstl_static
APP_STL := gnustl_static
# armeabi-v7a时选择 c++_static
# APP_STL := c++_static

APP_CPPFLAGS := -fexceptions -frtti #允许异常功能，及运行时类型识别  
APP_CPPFLAGS += -std=c++11 #允许使用c++11的函数等功能  
APP_CPPFLAGS += -fpermissive  #此项有效时表示宽松的编译形
LOCAL_LDFLAGS += -Wl,--gc-sections
LOCAL_LDLIBS += -lstdc++
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += -DMYMACRO=ANDROID
# LOCAL_LDLIBS += -latomic