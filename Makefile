# OBJS specifies which files to compile as part of the project
OBJS = *.cpp

# CC specifies which compiler we're using
CC = clang++

# INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I/usr/local/include -I/opt/X11/include -I./includes -I/usr/local/Cellar/freetype/2.6.3/include/freetype2 -I/usr/local/Cellar/v8/5.0.71.33/

# LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L/usr/local/lib -I/opt/X11/lib -L/usr/local/Cellar/v8/5.0.71.33/lib

# COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w -std=c++14 -ferror-limit=1

# LINKER_FLAGS specifies the libraries we're linking against
# Cocoa, IOKit, and CoreVideo are needed for static GLFW3.
LINKER_FLAGS = -framework CoreFoundation -framework OpenGL -lglfw3 -lglew -lSOIL -lirrKlang -lfreetype -lv8_base -lv8_libbase -lv8_libplatform -lv8_snapshot -llinenoise

# OBJ_NAME specifies the name of our exectuable
OBJ_NAME = main

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
