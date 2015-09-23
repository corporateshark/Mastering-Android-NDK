cmake_minimum_required(VERSION 2.8)
project("SDL2UI")

set(SRC_FILES
	main.cpp
	Src/Rendering/Canvas.cpp
	Src/Rendering/TextRenderer.cpp
	Src/UI/UIView.cpp
	Src/UI/UIStatic.cpp
	Src/UI/LayoutController.cpp
	Src/SDLWindow.cpp
	../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp
	../../Chapter6/1_GLES3/Engine/Graphics/Bitmap.cpp
	../../Chapter6/1_GLES3/Engine/Graphics/FI_Utils.cpp
	../../Chapter6/1_GLES3/Engine/Core/VecMath.cpp
	../../Chapter6/1_GLES3/Engine/LGL/LGL.cpp
	../../Chapter6/1_GLES3/Engine/LGL/GLClasses.cpp
	../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp
	../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp
	../../Chapter4/1_ArchiveFileAccess/src/libcompress.c
)

set(HEADER_FILES
	Src/Rendering/Canvas.h
	Src/Rendering/TextRenderer.h
	Src/UI/UIView.h
	Src/UI/UIStatic.h
	Src/UI/LayoutController.h
	Src/SDLWindow.h
	../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.h
	../../Chapter6/1_GLES3/Engine/Core/Platform.h
	../../Chapter6/1_GLES3/Engine/Log.h
	../../Chapter6/1_GLES3/Engine/SDLLibrary.h
	../../Chapter6/1_GLES3/Engine/Graphics/Bitmap.h
	../../Chapter6/1_GLES3/Engine/Graphics/FI_Utils.h
	../../Chapter6/1_GLES3/Engine/Graphics/Geometry.cpp
	../../Chapter6/1_GLES3/Engine/Core/VecMath.h
	../../Chapter4/1_ArchiveFileAccess/src/MountPoint.h
	../../Chapter4/1_ArchiveFileAccess/src/Streams.h
	../../Chapter4/1_ArchiveFileAccess/src/StrUtils.h
	../../Chapter4/1_ArchiveFileAccess/src/Archive.h
	../../Chapter4/1_ArchiveFileAccess/src/FileSystem.h
	../../Chapter4/1_ArchiveFileAccess/src/libcompress.h
	../../Chapter4/1_ArchiveFileAccess/src/Blob.h
	../../Chapter4/1_ArchiveFileAccess/src/Files.h
)

include_directories(
	.
	Src
	Src/Rendering
	../../Chapter6/1_GLES3
	../../Chapter6/1_GLES3/Engine
	../../Chapter6/1_GLES3/Engine/Core
	../../Chapter6/1_GLES3/Engine/Graphics
	../../Chapter4/1_ArchiveFileAccess/src
	../../Chapter3/2_AsyncTaskQueues
)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif(NOT MSVC)

add_executable(SDL2UI
	${SRC_FILES}
	${HEADER_FILES}
)

file(COPY receipt.ttf	DESTINATION .)

if(MSVC)
	file(COPY SDL2.dll					DESTINATION .)
	file(COPY freeimage32.dll			DESTINATION .)
	file(COPY libfreetype-6-32.dll	DESTINATION .)
	file(COPY libfreetype-6-64.dll	DESTINATION .)

	find_library(SDL2 NAMES SDL2 PATHS libs.prebuilt ../libs.prebuilt)
	target_link_libraries(SDL2UI
		${SDL2}
	)
else()
#	find_package(SDL REQUIRED)
endif()

if(LINUX)
	target_link_libraries(SDL2UI
		-lstdc++
	)
endif()

if(APPLE)
	find_library(SDL2osx NAMES SDL2osx.a PATHS libs.prebuilt ../libs.prebuilt)
	target_link_libraries(SDL2UI
		-lstdc++ ${SDL2osx}
	)
endif()
