APP_ABI := x86
# x86时选择 gunstl_static
APP_STL := gnustl_static

# APP_ABI :=armeabi-v7a
# armeabi-v7a时选择 c++_static
# APP_STL := c++_static

NDK_TOOLCHAIN_VERSION := clang
# NDK_TOOLCHAIN_VERSION := 4.9
TARGET_PLATFORM := android-21
# APP_PLATFORM ：= android-21
APP_OPTIM := debug


APP_CPPFLAGS := -fexceptions -frtti #允许异常功能，及运行时类型识别  
APP_CPPFLAGS += -std=c++11 #允许使用c++11的函数等功能  
APP_CPPFLAGS += -fpermissive  #此项有效时表示宽松的编译形
LOCAL_LDFLAGS += -Wl,--gc-sections
LOCAL_LDLIBS += -lstdc++
LOCAL_LDLIBS += -llog -lpthread -ldl -rt
LOCAL_CFLAGS += -DMYMACRO=ANDROID
# LOCAL_LDLIBS += -latomic