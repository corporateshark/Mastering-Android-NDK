OBJDIR=obj
CC = gcc

INCLUDE_DIRS = \
	-I . \
	-I Src \
	-I ../../Chapter6/1_GLES3/Engine \
	-I ../../Chapter6/1_GLES3/Engine/LGL \
	-I ../../Chapter6/1_GLES3/Engine/Core \
	-I ../../Chapter6/1_GLES3/Engine/Graphics \
	-I ../../Chapter4/1_ArchiveFileAccess/src \
	-I ../../Chapter3/2_AsyncTaskQueues \

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
	$(OBJDIR)/SDLWindow.o \
	$(OBJDIR)/UIView.o \
	$(OBJDIR)/UIStatic.o \
	$(OBJDIR)/LayoutController.o \
	$(OBJDIR)/Canvas.o \
	$(OBJDIR)/TextRenderer.o \
#	$(OBJDIR)/ImageCache.o \

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
	$(CC) $(CPPFLAGS) -c Src/SDLWindow.cpp -o $(OBJDIR)/SDLWindow.o

$(OBJDIR)/Canvas.o:
	$(CC) $(CPPFLAGS) -c Src/Rendering/Canvas.cpp -o $(OBJDIR)/Canvas.o

$(OBJDIR)/ImageCache.o:
	$(CC) $(CPPFLAGS) -c Src/Rendering/ImageCache.cpp -o $(OBJDIR)/ImageCache.o

$(OBJDIR)/TextRenderer.o:
	$(CC) $(CPPFLAGS) -c Src/Rendering/TextRenderer.cpp -o $(OBJDIR)/TextRenderer.o

$(OBJDIR)/UIView.o:
	$(CC) $(CPPFLAGS) -c Src/UI/UIView.cpp -o $(OBJDIR)/UIView.o

$(OBJDIR)/UIStatic.o:
	$(CC) $(CPPFLAGS) -c Src/UI/UIStatic.cpp -o $(OBJDIR)/UIStatic.o

$(OBJDIR)/LayoutController.o:
	$(CC) $(CPPFLAGS) -c Src/UI/LayoutController.cpp -o $(OBJDIR)/LayoutController.o

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

all: $(OBJS)
	gcc -o main.exe main.cpp $(OBJS) $(CPPFLAGS) $(LIBS) -g
