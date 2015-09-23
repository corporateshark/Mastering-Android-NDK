OBJDIR=obj
CC = gcc

INCLUDE_DIRS = \
	-I . \
	-I Src \
	-I ../../Chapter8/2_ShadowMaps/Src \
	-I ../../Chapter7/1_SDL2UI/Src/Rendering \
	-I ../../Chapter7/1_SDL2UI/Src/UI \
	-I ../../Chapter7/1_SDL2UI/Src \
	-I ../../Chapter7/1_SDL2UI \
	-I ../../Chapter6/1_GLES3 \
	-I ../../Chapter6/1_GLES3/Engine \
	-I ../../Chapter6/1_GLES3/Engine/Core \
	-I ../../Chapter6/1_GLES3/Engine/Graphics \
	-I ../../Chapter5/1_InitOpenAL \
	-I ../../Chapter4/1_ArchiveFileAccess/src \
	-I ../../Chapter3/2_AsyncTaskQueues

LIBS = -lopengl32 -lstdc++ -lmingw32 libSDL2main.a libSDL2.a -lwinmm -lole32 -loleaut32 -luser32 -lgdi32 -limm32 -lversion
CPPFLAGS = $(INCLUDE_DIRS) -std=c++14

OBJS=\
	$(OBJDIR)/IntrusivePtr.o \
	$(OBJDIR)/LGL.o \
	$(OBJDIR)/GLClasses.o \
	$(OBJDIR)/Geometry.o \
	$(OBJDIR)/Archive.o \
	$(OBJDIR)/FileSystem.o \
	$(OBJDIR)/libcompress.o \
	$(OBJDIR)/Library.o \
	$(OBJDIR)/Bitmap.o \
	$(OBJDIR)/FI_Utils.o \
	$(OBJDIR)/VecMath.o \
	$(OBJDIR)/Timers.o \
	$(OBJDIR)/SDLWindow.o \
	$(OBJDIR)/Canvas.o \
	$(OBJDIR)/GLFrameBuffer.o \
	$(OBJDIR)/Light.o \
	$(OBJDIR)/Loader_OBJ.o \
	$(OBJDIR)/TextRenderer.o \
	$(OBJDIR)/Material.o \
	$(OBJDIR)/Technique.o \
	$(OBJDIR)/Nodes.o \
	$(OBJDIR)/ROP.o

$(OBJDIR)/Canvas.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter7/1_SDL2UI/Src/Rendering/Canvas.cpp -o $(OBJDIR)/Canvas.o

$(OBJDIR)/TextRenderer.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter7/1_SDL2UI/Src/Rendering/TextRenderer.cpp -o $(OBJDIR)/TextRenderer.o

$(OBJDIR)/GLFrameBuffer.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter8/2_ShadowMaps/Src/GLFrameBuffer.cpp -o $(OBJDIR)/GLFrameBuffer.o

$(OBJDIR)/Technique.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter8/2_ShadowMaps/Src/Technique.cpp -o $(OBJDIR)/Technique.o

$(OBJDIR)/Light.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter8/2_ShadowMaps/Src/Light.cpp -o $(OBJDIR)/Light.o

$(OBJDIR)/Loader_OBJ.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter8/2_ShadowMaps/Src/Loader_OBJ.cpp -o $(OBJDIR)/Loader_OBJ.o

$(OBJDIR)/Material.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter8/2_ShadowMaps/Src/Material.cpp -o $(OBJDIR)/Material.o

$(OBJDIR)/Nodes.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter8/2_ShadowMaps/Src/Nodes.cpp -o $(OBJDIR)/Nodes.o

$(OBJDIR)/ROP.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter8/2_ShadowMaps/Src/ROP.cpp -o $(OBJDIR)/ROP.o

$(OBJDIR)/IntrusivePtr.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp -o $(OBJDIR)/IntrusivePtr.o

$(OBJDIR)/Library.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Core/Library.cpp -o $(OBJDIR)/Library.o

$(OBJDIR)/VecMath.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Core/VecMath.cpp -o $(OBJDIR)/VecMath.o

$(OBJDIR)/Bitmap.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Graphics/Bitmap.cpp -o $(OBJDIR)/Bitmap.o

$(OBJDIR)/FI_Utils.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Graphics/FI_Utils.cpp -o $(OBJDIR)/FI_Utils.o

$(OBJDIR)/Geometry.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/Graphics/Geometry.cpp -o $(OBJDIR)/Geometry.o

$(OBJDIR)/SDLWindow.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/SDLWindow.cpp -o $(OBJDIR)/SDLWindow.o

$(OBJDIR)/Archive.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter4/1_ArchiveFileAccess/src/Archive.cpp -o $(OBJDIR)/Archive.o

$(OBJDIR)/FileSystem.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter4/1_ArchiveFileAccess/src/FileSystem.cpp -o $(OBJDIR)/FileSystem.o

$(OBJDIR)/LGL.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/LGL/LGL.cpp -o $(OBJDIR)/LGL.o

$(OBJDIR)/GLClasses.o:
	$(CC) $(CPPFLAGS) -c ../../Chapter6/1_GLES3/Engine/LGL/GLClasses.cpp -o $(OBJDIR)/GLClasses.o

$(OBJDIR)/libcompress.o:
	$(CC) -c $(CFLAGS) ../../Chapter4/1_ArchiveFileAccess/src/libcompress.c -o $(OBJDIR)/libcompress.o

$(OBJDIR)/Timers.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter5/1_InitOpenAL/Timers.cpp -o $(OBJDIR)/Timers.o

all: $(OBJS)
	gcc -o main.exe main.cpp $(OBJS) $(CPPFLAGS) $(LIBS) -g
