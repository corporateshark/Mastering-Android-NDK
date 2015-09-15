PROJECT(ArchiveFileAccess)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(SRC_FILES
	src/Archive.cpp
	src/FileSystem.cpp
	src/libcompress.c
	../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp 
	main.cpp
)

set(HEADER_FILES
	src/Archive.h
	src/Blob.h
	src/Files.h
	src/FileSystem.h
	src/libcompress.h
	src/MountPoint.h
	src/Streams.h
	src/StrUtils.h
)

include_directories(
	../../Chapter3/2_AsyncTaskQueues
)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif(NOT MSVC)

add_executable(ArchiveFileAccess
	${SRC_FILES}
	${HEADER_FILES}
)
