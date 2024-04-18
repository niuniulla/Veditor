#define the link target
linkTarget = main

#define the libraries that we
LIBS = -lSDL2 -lSDL2_ttf -lwhisper -L./libs -I./includes

#define include path
INC = -I./includes

#define flags
CFLAGS = -std=c++17 -O0 -Wall

#define the object files
objects = main.o\
		  EditorApp.o\
		  $(patsubst %.cpp,%.o,$(wildcard ./src/*.cpp))\

#define the rebuilders
rebuildables = $(objects) $(linkTarget)

# rule to perform the build
$(linkTarget): $(objects) 
	g++ -g -o $(linkTarget) $(objects) $(LIBS)

#rule to create the .o (objects) files
%.o: %.cpp 
	g++ -g -o $@ -c $< $(CFLAGS)

.PHONEY:
clean:
	rm $(rebuildables)