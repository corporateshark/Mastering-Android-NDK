#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libFreeImage
LOCAL_SRC_FILES := ../../../../Libs.Android/libFreeImage.$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libFreeType
LOCAL_SRC_FILES := ../../../../Libs.Android/libFreeType.$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := NativeLib

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/$(SDL_PATH)/include \
	$(LOCAL_PATH)/$(SDL_PATH)/src \
	$(LOCAL_PATH)/../../ \
	$(LOCAL_PATH)/../../Src \
	$(LOCAL_PATH)/../../../../Chapter7/1_SDL2UI/Src \
	$(LOCAL_PATH)/../../../../Chapter7/1_SDL2UI/Src/Rendering \
	$(LOCAL_PATH)/../../../../Chapter8/2_ShadowMaps/Src \
	$(LOCAL_PATH)/../../../../Chapter6/1_GLES3/Engine \
	$(LOCAL_PATH)/../../../../Chapter6/1_GLES3/Engine/Core \
	$(LOCAL_PATH)/../../../../Chapter6/1_GLES3/Engine/Graphics \
	$(LOCAL_PATH)/../../../../Chapter6/1_GLES3/Engine/LGL \
	$(LOCAL_PATH)/../../../../Chapter5/1_InitOpenAL \
	$(LOCAL_PATH)/../../../../Chapter4/1_ArchiveFileAccess/src \
	$(LOCAL_PATH)/../../../../Chapter3/2_AsyncTaskQueues \

# Engine
LOCAL_SRC_FILES := ../SDL_android_main.c \
	../../../../Chapter8/2_ShadowMaps/Src/Nodes.cpp \
	../../../../Chapter8/2_ShadowMaps/Src/ROP.cpp \
	../../../../Chapter8/2_ShadowMaps/Src/Light.cpp \
	../../../../Chapter8/2_ShadowMaps/Src/Material.cpp \
	../../../../Chapter8/2_ShadowMaps/Src/GLFrameBuffer.cpp \
	../../../../Chapter8/2_ShadowMaps/Src/Loader_OBJ.cpp \
	../../../../Chapter8/2_ShadowMaps/Src/Technique.cpp \
	../../../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp \
	../../../../Chapter7/1_SDL2UI/Src/Rendering/Canvas.cpp \
	../../../../Chapter7/1_SDL2UI/Src/Rendering/TextRenderer.cpp \
	../../../../Chapter7/1_SDL2UI/Src/SDLWindow.cpp \
	../../../../Chapter6/1_GLES3/Engine/Graphics/Bitmap.cpp \
	../../../../Chapter6/1_GLES3/Engine/Graphics/FI_Utils.cpp \
	../../../../Chapter6/1_GLES3/Engine/Graphics/Geometry.cpp \
	../../../../Chapter6/1_GLES3/Engine/Core/VecMath.cpp \
	../../../../Chapter6/1_GLES3/Engine/LGL/LGL.cpp \
	../../../../Chapter6/1_GLES3/Engine/LGL/GLClasses.cpp \
	../../../../Chapter5/1_InitOpenAL/Timers.cpp \
	../../../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp \
	../../../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp \
	../../../../Chapter4/1_ArchiveFileAccess/src/libcompress.c \
	../../../../Chapter4/1_ArchiveFileAccess/jni/Wrappers.cpp \


# Application
LOCAL_SRC_FILES += \
	../../main.cpp \
	../../Src/ParticleEmitter.cpp \
	../../Src/ParticleMaterial.cpp \
	../../Src/ParticleNode.cpp \
	../../Src/ParticleSystem.cpp

LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_STATIC_LIBRARIES += FreeImage FreeType

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv3 -llog -ldl

LOCAL_CPPFLAGS = -std=c++14

include $(BUILD_SHARED_LIBRARY)
