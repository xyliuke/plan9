#APP_ABI := armeabi
NDK_TOOLCHAIN_VERSION := 4.9
APP_STL := gnustl_static
APP_CPPFLAGS := -fexceptions -frtti #允许异常功能，及运行时类型识别  
APP_CPPFLAGS += -std=c++11 #允许使用c++11的函数等功能  
APP_CPPFLAGS += -fpermissive  #此项有效时表示宽松的编译形
LOCAL_LDLIBS += -latomic