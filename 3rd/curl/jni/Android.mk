LOCAL_PATH := $(call my-dir)/..

CFLAGS := -Wpointer-arith -Wwrite-strings -Wunused -Winline \
 -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -Wno-long-long \
 -Wfloat-equal -Wno-multichar -Wsign-compare -Wno-format-nonliteral \
 -Wendif-labels -Wstrict-prototypes -Wdeclaration-after-statement \
 -Wno-system-headers -DHAVE_CONFIG_H

include $(CLEAR_VARS)
include $(LOCAL_PATH)/lib/Makefile.inc

LOCAL_MODULE := curl

LOCAL_SRC_FILES := $(addprefix lib/,$(CSOURCES))
#FILE_LIST := $(wildcard $(LOCAL_PATH)/curl/lib/*.c)
#LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_CFLAGS += $(CFLAGS)
LOCAL_CFLAGS += -D__ARM_ARCH_5__
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ $(LOCAL_PATH)/lib

LOCAL_COPY_HEADERS_TO := libcurl
LOCAL_COPY_HEADERS := $(addprefix include/curl/,$(HHEADERS))


include $(BUILD_STATIC_LIBRARY)

