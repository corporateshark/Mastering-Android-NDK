#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libFreeImage
LOCAL_SRC_FILES :=../../../Libs.Android/libFreeImage.$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

TARGET_PLATFORM := android-19

LOCAL_MODULE   := NativeLib

LOCAL_SRC_FILES += Wrappers.cpp ../FI_Utils.cpp ../FITest.cpp

LOCAL_ARM_MODE := arm
COMMON_CFLAGS := -Werror -DANDROID -DDISABLE_IMPORTGL

ifeq ($(TARGET_ARCH),x86)
	LOCAL_CFLAGS   := $(COMMON_CFLAGS)
else
	LOCAL_CFLAGS   := -mfpu=vfp -mfloat-abi=hard -mhard-float -fno-short-enums -D_NDK_MATH_NO_SOFTFP=1 $(COMMON_CFLAGS)
endif

LOCAL_LDLIBS     := -llog -lGLESv2 -Wl,-s

LOCAL_CPPFLAGS += -std=gnu++11

LOCAL_STATIC_LIBRARIES += FreeImage

include $(BUILD_SHARED_LIBRARY)
