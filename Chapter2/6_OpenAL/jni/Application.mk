#
APP_OPTIM := release
APP_PLATFORM := android-19
APP_STL := gnustl_static
APP_CPPFLAGS += -frtti 
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -DANDROID
APP_MODULES := OpenAL
APP_ABI := armeabi-v7a-hard x86
NDK_TOOLCHAIN_VERSION := clang
