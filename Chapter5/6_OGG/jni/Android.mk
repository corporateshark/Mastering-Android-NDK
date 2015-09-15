TARGET_PLATFORM := android-19

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libModPlug
LOCAL_SRC_FILES :=../../../Libs.Android/libModPlug.$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libOGG
LOCAL_SRC_FILES :=../../../Libs.Android/libOGG.$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libVorbis
LOCAL_SRC_FILES :=../../../Libs.Android/libVorbis.$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libOpenAL
LOCAL_SRC_FILES :=../../../Libs.Android/libOpenAL.$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE     := NativeLib
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../include/vorbis \
	$(LOCAL_PATH)/../src \
	$(LOCAL_PATH)/../../1_InitOpenAL \
	$(LOCAL_PATH)/../../2_Streaming/src \
	$(LOCAL_PATH)/../../4_ModPlug/src \
	$(LOCAL_PATH)/../../../Chapter3/2_AsyncTaskQueues \
	$(LOCAL_PATH)/../../../Chapter4/1_ArchiveFileAccess/src

LOCAL_SRC_FILES  := ../main.cpp ../src/Decoders.cpp
LOCAL_SRC_FILES  +=	../../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp \
							../../../Chapter3/2_AsyncTaskQueues/Thread.cpp \
							../../../Chapter3/2_AsyncTaskQueues/tinythread.cpp \
							../../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp \
							../../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp \
							../../../Chapter4/1_ArchiveFileAccess/src/libcompress.c \
							../../../Chapter4/1_ArchiveFileAccess/jni/Wrappers.cpp \
							../../4_ModPlug/jni/WrappersJNI.c \
							../../1_InitOpenAL/LAL.cpp \
							../../1_InitOpenAL/Timers.cpp

LOCAL_ARM_MODE := arm
GLOBAL_CFLAGS  := -O3 -DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES -DHAVE_ANDROID=1 -Werror -DANDROID -DDISABLE_IMPORTGL

ifeq ($(TARGET_ARCH),x86)
	LOCAL_CFLAGS   := $(GLOBAL_CFLAGS)
else
	LOCAL_CFLAGS   := -mfpu=vfp -mfloat-abi=hard -mhard-float -fno-short-enums -D_NDK_MATH_NO_SOFTFP=1 $(GLOBAL_CFLAGS)
endif

LOCAL_LDLIBS     := -llog -lGLESv2 -Wl,-s

LOCAL_CPPFLAGS += -std=gnu++11

LOCAL_STATIC_LIBRARIES += ModPlug
LOCAL_STATIC_LIBRARIES += Vorbis
LOCAL_STATIC_LIBRARIES += OGG
LOCAL_STATIC_LIBRARIES += OpenAL

include $(BUILD_SHARED_LIBRARY)
