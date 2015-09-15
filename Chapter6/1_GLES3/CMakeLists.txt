PROJECT(GLES3_Example)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

if(MSVC)
    file(COPY freeimage32.dll  DESTINATION ./Debug)
    file(COPY freeimage64.dll  DESTINATION ./Debug)
    file(COPY SDL2.dll         DESTINATION ./Debug)
endif()

set(SRC_FILES
	../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp
	../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp
	../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp
	../../Chapter4/1_ArchiveFileAccess/src/libcompress.c
	Engine/Core/Library.cpp
	Engine/Core/VecMath.cpp
	Engine/Graphics/Bitmap.cpp
	Engine/Graphics/FI_Utils.cpp
	Engine/Graphics/Geometry.cpp
	Engine/LGL/GLClasses.cpp
	Engine/LGL/LGL.cpp
	Engine/SDLWindow.cpp
	main.cpp
)

set(HEADER_FILES
	../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.h
	../../Chapter4/1_ArchiveFileAccess/src/StrUtils.h
	../../Chapter4/1_ArchiveFileAccess/src/Archive.h
	../../Chapter4/1_ArchiveFileAccess/src/Blob.h
	../../Chapter4/1_ArchiveFileAccess/src/Files.h
	../../Chapter4/1_ArchiveFileAccess/src/FileSystem.h
	../../Chapter4/1_ArchiveFileAccess/src/libcompress.h
	../../Chapter4/1_ArchiveFileAccess/src/MountPoint.h
	../../Chapter4/1_ArchiveFileAccess/src/Streams.h
	Engine/Core/Library.h
	Engine/Core/Platform.h
	Engine/Core/VecMath.h
	Engine/Graphics/Bitmap.h
	Engine/Graphics/FI_Utils.h
	Engine/Graphics/Geometry.h
	Engine/LGL/GLClasses.h
	Engine/LGL/LGL.h
	Engine/LGL/LGL_Android.h
	Engine/LGL/LGL_Windows.h
	Engine/LGL/LGLAPI.h
	Engine/Callbacks.h
	Engine/Log.h
	Engine/SDLLibrary.h
	Engine/SDLWindow.h
)

include_directories(
	.
	Engine
	Engine/LGL
	Engine/Core
	../../Chapter3/2_AsyncTaskQueues
	../../Chapter4/1_ArchiveFileAccess/src
)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++14")
endif(NOT MSVC)

add_executable(GLES3Example
	${SRC_FILES}
	${HEADER_FILES}
)

if(MSVC)
	find_library(SDL2 NAMES SDL2 PATHS libs ../libs)
	target_link_libraries(GLES3Example
		${SDL2}
	)
endif(MSVC)
