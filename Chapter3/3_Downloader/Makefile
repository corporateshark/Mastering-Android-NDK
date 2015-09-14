CC = gcc

SRC = main.cpp 
SRC += CurlWrap.cpp Downloader.cpp DownloadTask.cpp
SRC += ../2_AsyncTaskQueues/Thread.cpp ../2_AsyncTaskQueues/IntrusivePtr.cpp ../2_AsyncTaskQueues/tinythread.cpp
SRC += urlparser/LUrlParser.cpp

all:
	$(CC) -o main.exe $(SRC) -I ../2_AsyncTaskQueues -lstdc++ -std=c++11
