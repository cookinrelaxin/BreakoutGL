CC = clang++

CC_FLAGS = -w -std=c++14 -ferror-limit=1

INCLUDE_PATHS = -I../../../include -I../../../vendor

LIBRARY_PATHS = -L/usr/local/lib

LINKER_FLAGS = -framework CoreFoundation -framework OpenGL -lglfw3 -lglew -lSOIL
EXEC = positionable_camera.bin

SOURCES = $(wildcard *.cpp) ../../../src/ray.cpp ../../../src/sphere.cpp ../../../src/hitable_list.cpp ../../../src/rt_camera.cpp ../../../src/lambertian.cpp ../../../src/metal.cpp ../../../src/material.cpp ../../../src/dialectric.cpp

OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) -g $(OBJECTS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c -g $(CC_FLAGS) $(INCLUDE_PATHS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)

