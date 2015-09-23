#
APP_OPTIM := release
APP_PLATFORM := android-21
APP_STL := gnustl_static
APP_CPPFLAGS += -frtti 
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -DANDROID -D__ANDROID__ -DHAVE_M_PI -g
APP_ABI := armeabi-v7a-hard
APP_MODULES := NativeLib
NDK_TOOLCHAIN_VERSION := clang
APP_LDFLAGS += -fuse-ld=gold
