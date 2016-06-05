#ifndef SHELL_H
#define SHELL_H

#include <thread>

#include "readerwriterqueue.h"
#include "atomicops.h"

#include <v8pp/context.hpp>

#include "zscene_node.h"

class Shell {
    public:
        Shell();
        ~Shell();

        void UninitializeV8();
        void LoadMainScript(const std::string& file_name);
        void CreateShellContext();

        ZSceneNode* Init();
        void Poll();
        bool Update(const float elapsedTime);
        void KeyDown(int key);
        void KeyUp(int key);
        
        static v8pp::context* _context;
        // v8pp::context _context;
        // static v8::Platform* _platform;

        static moodycamel::ReaderWriterQueue<std::string> UnevaluatedQueue;
        static moodycamel::ReaderWriterQueue<std::string> EvaluatedQueue;


    private:
        std::thread _debugger_thread;

};
#endif
