PROJECT(4_ModPlug)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

if(MSVC)
    file(COPY modplug.dll  DESTINATION ./Debug)
    file(COPY OpenAL32.dll DESTINATION ./Debug)
    file(COPY wrap_oal.dll DESTINATION ./Debug)
endif()

set(SRC_FILES
	main.cpp
	src/Decoders.cpp
	../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp
	../../Chapter3/2_AsyncTaskQueues/Thread.cpp
	../../Chapter3/2_AsyncTaskQueues/tinythread.cpp
	../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp
	../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp
	../1_InitOpenAL/LAL.cpp
	../1_InitOpenAL/Timers.cpp
	../../Chapter4/1_ArchiveFileAccess/src/libcompress.c
)

set(HEADER_FILES
)

include_directories(
	.
	include
	src
	../../Chapter3/2_AsyncTaskQueues
	../../Chapter4/1_ArchiveFileAccess/src
	../2_Streaming/src
	../1_InitOpenAL
)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++14")
endif(NOT MSVC)

add_executable(4_ModPlug
	${SRC_FILES}
	${HEADER_FILES}
)
