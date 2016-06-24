CC = clang++

CC_FLAGS = -w -std=c++14 -ferror-limit=1

INCLUDE_PATHS = -I../../include -I../../vendor

LIBRARY_PATHS = -L/usr/local/lib

LINKER_FLAGS = -framework CoreFoundation -framework OpenGL -lglfw3 -lglew -lSOIL
EXEC = normal_mapping.bin

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) -g $(OBJECTS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c -g $(CC_FLAGS) $(INCLUDE_PATHS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)

