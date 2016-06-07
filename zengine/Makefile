CC = clang++

CC_FLAGS = -w -std=c++14 -ferror-limit=1

INCLUDE_PATHS = -I/usr/local/include -I./zengine -I./src/include -I/usr/local/Cellar/freetype/2.6.3/include/freetype2 -I/usr/local/Cellar/nlohmann_json/1.1.0/include/

LIBRARY_PATHS = -L/usr/local/lib

LINKER_FLAGS = -framework CoreFoundation -framework OpenGL -lglfw3 -lglew -lSOIL -lirrKlang -lfreetype

#SOURCES = $(wildcard src/*.cpp)
SOURCES = ./src/zengine.cpp

OBJECTS = $(SOURCES:.cpp=.o)

TARGET = ./lib/libzengine.a

# all: $(TARGET)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $(INCLUDE_PATHS) $< -o $@

TARGET: $(OBJECTS)
	ar rcs $(TARGET) $^

clean:
	rm -f $(TARGET) $(OBJECTS)
