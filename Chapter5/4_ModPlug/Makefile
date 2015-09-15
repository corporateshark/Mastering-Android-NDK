CC=gcc
CFLAGS = -g -Iinclude -Isrc -I../../Chapter3/2_AsyncTaskQueues -I../../Chapter4/1_ArchiveFileAccess/src -I../2_Streaming/src -I../1_InitOpenAL
CPPFLAGS = -std=c++11 $(CFLAGS)

OBJDIR=obj

OBJS=$(OBJDIR)/IntrusivePtr.o \
	$(OBJDIR)/Thread.o \
	$(OBJDIR)/tinythread.o \
	$(OBJDIR)/Archive.o \
	$(OBJDIR)/FileSystem.o \
	$(OBJDIR)/LAL.o \
	$(OBJDIR)/Timers.o \
	$(OBJDIR)/libcompress.o \
	$(OBJDIR)/Decoders.o

LIBS=-lstdc++

$(OBJDIR)/Decoders.o:
	$(CC) $(CPPFLAGS) -c src/Decoders.cpp -o $(OBJDIR)/Decoders.o

$(OBJDIR)/IntrusivePtr.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp -o $(OBJDIR)/IntrusivePtr.o

$(OBJDIR)/Thread.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter3/2_AsyncTaskQueues/Thread.cpp -o $(OBJDIR)/Thread.o

$(OBJDIR)/tinythread.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter3/2_AsyncTaskQueues/tinythread.cpp -o $(OBJDIR)/tinythread.o

$(OBJDIR)/Archive.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp -o $(OBJDIR)/Archive.o

$(OBJDIR)/FileSystem.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp -o $(OBJDIR)/FileSystem.o

$(OBJDIR)/LAL.o:
	$(CC) $(CPPFLAGS) -c ../1_InitOpenAL/LAL.cpp -o $(OBJDIR)/LAL.o

$(OBJDIR)/Timers.o:
	$(CC) $(CPPFLAGS) -c ../1_InitOpenAL/Timers.cpp -o $(OBJDIR)/Timers.o

$(OBJDIR)/libcompress.o:
	$(CC) -c $(CFLAGS) ../../Chapter4/1_ArchiveFileAccess/src/libcompress.c -o $(OBJDIR)/libcompress.o

all: $(OBJS)
	$(CC) $(CPPFLAGS) main.cpp $(OBJS) -o main.exe $(LIBS)
