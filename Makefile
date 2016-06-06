CC = clang++

CC_FLAGS = -w -std=c++14 -ferror-limit=1

INCLUDE_PATHS = -I/usr/local/include -I./includes -I/usr/local/Cellar/freetype/2.6.3/include/freetype2 -I/usr/local/Cellar/v8/5.0.71.33/ -I/usr/local/Cellar/nlohmann_json/1.1.0/include/

LIBRARY_PATHS = -L/usr/local/lib -L/usr/local/Cellar/v8/5.0.71.33/lib

LINKER_FLAGS = -framework CoreFoundation -framework OpenGL -lglfw3 -lglew -lSOIL -lirrKlang -lfreetype -lv8_base -lv8_libbase -lv8_libplatform -lv8_snapshot

EXEC = zengine

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) -g $(OBJECTS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c -g $(CC_FLAGS) $(INCLUDE_PATHS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)
