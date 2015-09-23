CC=gcc
CFLAGS = -g -Iinclude -Isrc -I../../Chapter3/2_AsyncTaskQueues -I../../Chapter4/1_ArchiveFileAccess/src -I../2_Streaming/src -I../../Chapter5/1_InitOpenAL
CFLAGS += -I../../Chapter6/1_GLES3
CFLAGS += -I../../Chapter6/1_GLES3/Engine
CFLAGS += -I../../Chapter6/1_GLES3/Engine/Core
CFLAGS += -I../../Chapter6/1_GLES3/Engine/Graphics
CPPFLAGS = -std=c++11 $(CFLAGS)

OBJDIR=obj

OBJS=$(OBJDIR)/IntrusivePtr.o \
	$(OBJDIR)/Thread.o \
	$(OBJDIR)/tinythread.o \
	$(OBJDIR)/Archive.o \
	$(OBJDIR)/FileSystem.o \
	$(OBJDIR)/Timers.o \
	$(OBJDIR)/libcompress.o \
	$(OBJDIR)/Library.o \
	$(OBJDIR)/LGL.o \
	$(OBJDIR)/GLClasses.o \
	$(OBJDIR)/Bitmap.o \
	$(OBJDIR)/FI_Utils.o \
	$(OBJDIR)/Geometry.o \
	$(OBJDIR)/VecMath.o \
	$(OBJDIR)/SDLWindow.o \
	$(OBJDIR)/Nodes.o \
	$(OBJDIR)/ROP.o

LIBS = -lopengl32 -lstdc++ -lmingw32 ../../Chapter6/1_GLES3/libSDL2main.a ../../Chapter6/1_GLES3/libSDL2.a -lwinmm -lole32 -loleaut32 -luser32 -lgdi32 -limm32 -lversion

$(OBJDIR)/Nodes.o:
	$(CC) -c $(CPPFLAGS) Src/Nodes.cpp -o $(OBJDIR)/Nodes.o

$(OBJDIR)/ROP.o:
	$(CC) -c $(CPPFLAGS) Src/ROP.cpp -o $(OBJDIR)/ROP.o

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

$(OBJDIR)/libcompress.o:
	$(CC) -c $(CFLAGS) ../../Chapter4/1_ArchiveFileAccess/src/libcompress.c -o $(OBJDIR)/libcompress.o

$(OBJDIR)/Timers.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter5/1_InitOpenAL/Timers.cpp -o $(OBJDIR)/Timers.o

$(OBJDIR)/Library.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Core/Library.cpp -o $(OBJDIR)/Library.o

$(OBJDIR)/VecMath.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Core/VecMath.cpp -o $(OBJDIR)/VecMath.o

$(OBJDIR)/LGL.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/LGL/LGL.cpp -o $(OBJDIR)/LGL.o

$(OBJDIR)/GLClasses.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/LGL/GLClasses.cpp -o $(OBJDIR)/GLClasses.o

$(OBJDIR)/Bitmap.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Graphics/Bitmap.cpp -o $(OBJDIR)/Bitmap.o

$(OBJDIR)/FI_Utils.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Graphics/FI_Utils.cpp -o $(OBJDIR)/FI_Utils.o

$(OBJDIR)/Geometry.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Graphics/Geometry.cpp -o $(OBJDIR)/Geometry.o

$(OBJDIR)/SDLWindow.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/SDLWindow.cpp -o $(OBJDIR)/SDLWindow.o


all: $(OBJS)
	$(CC) $(CPPFLAGS) main.cpp $(OBJS) -o main.exe $(LIBS)
