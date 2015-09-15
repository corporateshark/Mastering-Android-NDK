SRC = main.cpp
SRC += ../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp ../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp ../../Chapter4/1_ArchiveFileAccess/src/libcompress.c 
SRC += ../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp ../../Chapter3/2_AsyncTaskQueues/Thread.cpp ../../Chapter3/2_AsyncTaskQueues/tinythread.cpp
SRC += LAL.cpp Timers.cpp

all:
	gcc -o main.exe $(SRC) -std=c++11 -lstdc++ -I ../../Chapter3/2_AsyncTaskQueues -I ../../Chapter4/1_ArchiveFileAccess/src -I src
