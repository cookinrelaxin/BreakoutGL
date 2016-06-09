CC = clang++

CC_FLAGS = -w -std=c++14 -ferror-limit=1

INCLUDE_PATHS = -I/usr/local/include -I./zengine -I./src/ -I./src/include -I/usr/local/Cellar/freetype/2.6.3/include/freetype2 

SOURCES = $(wildcard src/*.cpp)
#SOURCES = ./src/zengine.cpp

OBJECTS = $(SOURCES:.cpp=.o)

TARGET = ./lib/libzengine.a

%.o: %.cpp
	$(CC) -c -g $(CC_FLAGS) $(INCLUDE_PATHS) $< -o $@

TARGET: $(OBJECTS)
	ar rcs $(TARGET) $^

clean:
	rm -f $(TARGET) $(OBJECTS)
