OBJDIR=obj
CC = gcc

INCLUDE_DIRS = \
	-I . \
	-I Engine \
	-I Engine/LGL \
	-I Engine/Core \
	-I../../Chapter3/2_AsyncTaskQueues \
	-I../../Chapter4/1_ArchiveFileAccess/src \

LIBS = -lopengl32 -lstdc++ -lmingw32 libSDL2main.a libSDL2.a -lwinmm -lole32 -loleaut32 -luser32 -lgdi32 -limm32 -lversion
CPPFLAGS = $(INCLUDE_DIRS) -std=c++14

OBJS=\
	$(OBJDIR)/IntrusivePtr.o \
	$(OBJDIR)/Library.o \
	$(OBJDIR)/LGL.o \
	$(OBJDIR)/GLClasses.o \
	$(OBJDIR)/Bitmap.o \
	$(OBJDIR)/FI_Utils.o \
	$(OBJDIR)/Geometry.o \
	$(OBJDIR)/VecMath.o \
	$(OBJDIR)/SDLWindow.o

$(OBJDIR)/IntrusivePtr.o:
	$(CC) -c $(CPPFLAGS) ../../Chapter3/2_AsyncTaskQueues/IntrusivePtr.cpp -o $(OBJDIR)/IntrusivePtr.o

$(OBJDIR)/Library.o:
	$(CC) $(CPPFLAGS) -c Engine/Core/Library.cpp -o $(OBJDIR)/Library.o

$(OBJDIR)/VecMath.o:
	$(CC) $(CPPFLAGS) -c Engine/Core/VecMath.cpp -o $(OBJDIR)/VecMath.o

$(OBJDIR)/LGL.o:
	$(CC) $(CPPFLAGS) -c Engine/LGL/LGL.cpp -o $(OBJDIR)/LGL.o

$(OBJDIR)/GLClasses.o:
	$(CC) $(CPPFLAGS) -c Engine/LGL/GLClasses.cpp -o $(OBJDIR)/GLClasses.o

$(OBJDIR)/Bitmap.o:
	$(CC) $(CPPFLAGS) -c Engine/Graphics/Bitmap.cpp -o $(OBJDIR)/Bitmap.o

$(OBJDIR)/FI_Utils.o:
	$(CC) $(CPPFLAGS) -c Engine/Graphics/FI_Utils.cpp -o $(OBJDIR)/FI_Utils.o

$(OBJDIR)/Geometry.o:
	$(CC) $(CPPFLAGS) -c Engine/Graphics/Geometry.cpp -o $(OBJDIR)/Geometry.o

$(OBJDIR)/SDLWindow.o:
	$(CC) $(CPPFLAGS) -c Engine/SDLWindow.cpp -o $(OBJDIR)/SDLWindow.o

all: $(OBJS)
	gcc -o main.exe main.cpp $(OBJS) $(CPPFLAGS) $(LIBS) -g
