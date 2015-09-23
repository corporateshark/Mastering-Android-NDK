cmake_minimum_required(VERSION 2.8)
project("ShadowMaps")

set(SRC_FILES
	main.cpp
	Src/Nodes.cpp
	Src/ROP.cpp
	Src/Light.cpp
	Src/Material.cpp
	Src/GLFrameBuffer.cpp
	Src/Loader_OBJ.cpp
	Src/Technique.cpp
	../../Chapter7/1_SDL2UI/Src/Rendering/Canvas.cpp
	../../Chapter7/1_SDL2UI/Src/Rendering/TextRenderer.cpp
	../../Chapter7/1_SDL2UI/Src/SDLWindow.cpp
	../../Chapter7/1_SDL2UI/Src/UI/UIView.cpp
	../../Chapter7/1_SDL2UI/Src/UI/LayoutController.cpp
	../../Chapter7/1_SDL2UI/Src/UI/UIStatic.cpp
	../../Chapter6/1_GLES3/Engine/Graphics/Bitmap.cpp
	../../Chapter6/1_GLES3/Engine/Graphics/FI_Utils.cpp
	../../Chapter6/1_GLES3/Engine/Core/VecMath.cpp
	../../Chapter6/1_GLES3/Engine/LGL/LGL.cpp
	../../Chapter6/1_GLES3/Engine/LGL/GLClasses.cpp
	../../Chapter5/1_InitOpenAL/Timers.cpp
	../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp
	../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp
	../../Chapter4/1_ArchiveFileAccess/src/libcompress.c
	../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp
)

set(HEADER_FILES
	Src/Material.h
	Src/Nodes.h
	Src/ROP.h
	Src/Light.h
	Src/Traversers.h
	Src/GLFrameBuffer.h
	Src/Loader_OBJ.h
	Src/Technique.h
	../../Chapter7/1_SDL2UI/Src/Rendering/Canvas.h
	../../Chapter7/1_SDL2UI/Src/Rendering/ft.h
	../../Chapter7/1_SDL2UI/Src/Rendering/TextRenderer.h
	../../Chapter7/1_SDL2UI/Src/SDLWindow.h
	../../Chapter7/1_SDL2UI/Src/UI/UIView.h
	../../Chapter7/1_SDL2UI/Src/UI/UIButton.h
	../../Chapter7/1_SDL2UI/Src/UI/LayoutController.h
	../../Chapter7/1_SDL2UI/Src/UI/UIStatic.h
	../../Chapter6/1_GLES3/Engine/Log.h
	../../Chapter6/1_GLES3/Engine/SDLLibrary.h
	../../Chapter6/1_GLES3/Engine/Graphics/Bitmap.h
	../../Chapter6/1_GLES3/Engine/Graphics/FI_Utils.h
	../../Chapter6/1_GLES3/Engine/Graphics/Geometry.cpp
	../../Chapter6/1_GLES3/Engine/Core/VecMath.h
	../../Chapter5/1_InitOpenAL/Timers.h
	../../Chapter4/1_ArchiveFileAccess/src/MountPoint.h
	../../Chapter4/1_ArchiveFileAccess/src/Streams.h
	../../Chapter4/1_ArchiveFileAccess/src/StrUtils.h
	../../Chapter4/1_ArchiveFileAccess/src/Archive.h
	../../Chapter4/1_ArchiveFileAccess/src/FileSystem.h
	../../Chapter4/1_ArchiveFileAccess/src/libcompress.h
	../../Chapter4/1_ArchiveFileAccess/src/Blob.h
	../../Chapter4/1_ArchiveFileAccess/src/Files.h
	../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.h
)

include_directories(
	.
	Src
	../../Chapter7/1_SDL2UI/Src/Rendering
	../../Chapter7/1_SDL2UI/Src/UI
	../../Chapter7/1_SDL2UI/Src
	../../Chapter7/1_SDL2UI/
	../../Chapter6/1_GLES3
	../../Chapter6/1_GLES3/Engine
	../../Chapter6/1_GLES3/Engine/Core
	../../Chapter6/1_GLES3/Engine/Graphics
	../../Chapter5/1_InitOpenAL
	../../Chapter4/1_ArchiveFileAccess/src
	../../Chapter3/2_AsyncTaskQueues
)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif(NOT MSVC)

add_executable(ShadowMaps
	${SRC_FILES}
	${HEADER_FILES}
)

if(MSVC)
	file(COPY SDL2.dll					DESTINATION ./Debug)
	file(COPY freeimage32.dll			DESTINATION ./Debug)
	file(COPY freeimage64.dll			DESTINATION ./Debug)
	file(COPY assets 		            DESTINATION .)

	find_library(SDL2 NAMES SDL2 PATHS libs.prebuilt ../libs.prebuilt)
	target_link_libraries(ShadowMaps
		${SDL2}
	)
endif()
