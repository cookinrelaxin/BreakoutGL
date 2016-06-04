// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <stb_image.h>
/////////////////////

#include <include/v8.h>
#include <include/libplatform/libplatform.h>

#include "v8pp/module.hpp"
#include "v8pp/class.hpp"
#include "v8pp/convert.hpp"
#include "v8pp/object.hpp"

#include "json.hpp"

#include "readerwriterqueue.h"
#include "atomicops.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>

#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
