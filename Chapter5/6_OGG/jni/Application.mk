#
APP_OPTIM := release
APP_PLATFORM := android-19
APP_STL := gnustl_static
APP_CPPFLAGS += -frtti 
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -DANDROID
APP_MODULES := NativeLib
APP_ABI := armeabi-v7a-hard
NDK_TOOLCHAIN_VERSION := clang
