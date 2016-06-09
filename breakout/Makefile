CC = clang++

CC_FLAGS = -w -std=c++14 -ferror-limit=1

INCLUDE_PATHS = -I../zengine/ -I../zengine/src/include/ -I../zengine/src -I/usr/local/Cellar/nlohmann_json/1.1.0/include/

LIBRARY_PATHS = -L/usr/local/lib -L../zengine/lib

LINKER_FLAGS = -framework CoreFoundation -framework OpenGL -lglfw3 -lglew -lSOIL -lirrKlang -lfreetype -lzengine

EXEC = breakout.bin

#SOURCES = $(wildcard *.cpp)
SOURCES = ./src/main.cpp ./src/breakout.cpp

OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) -g $(OBJECTS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c -g $(CC_FLAGS) $(INCLUDE_PATHS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)
