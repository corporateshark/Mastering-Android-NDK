#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libFreeImage
LOCAL_SRC_FILES := ../../../../Libs.Android/libFreeImage.$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := NativeLib

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/$(SDL_PATH)/include \
	$(LOCAL_PATH)/../../ \
	$(LOCAL_PATH)/../../Engine \
	$(LOCAL_PATH)/../../Engine/Core \
	$(LOCAL_PATH)../../SDL/src \
	$(LOCAL_PATH)../../../../../Chapter3/2_AsyncTaskQueues \
	$(LOCAL_PATH)../../../../../Chapter4/1_ArchiveFileAccess/src \

# Engine
LOCAL_SRC_FILES := ../SDL_android_main.c \
	../../../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp \
	../../../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp \
	../../../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp \
	../../../../Chapter4/1_ArchiveFileAccess/src/libcompress.c \
	../../Engine/Core/Library.cpp \
	../../Engine/Core/VecMath.cpp \
	../../Engine/Graphics/Bitmap.cpp \
	../../Engine/Graphics/FI_Utils.cpp \
	../../Engine/Graphics/Geometry.cpp \
	../../Engine/LGL/GLClasses.cpp \
	../../Engine/LGL/LGL.cpp \
	../../Engine/SDLWindow.cpp \

# Application
LOCAL_SRC_FILES += \
	../../main.cpp

LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_STATIC_LIBRARIES += FreeImage

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv3 -llog -ldl

LOCAL_CPPFLAGS = -std=c++14

include $(BUILD_SHARED_LIBRARY)
