#ifndef SCRIPT_CONSOLE_H
#define SCRIPT_CONSOLE_H

#include <include/v8.h>
#include <string>

namespace v8{
    class Isolate;
}

class Console {
    public:
        static void Create(v8::Isolate* isolate, v8::Local<v8::Context> context);
        static void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Assert(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void VoidMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    private:
        static std::string StringifyArguments(
                const v8::FunctionCallbackInfo<v8::Value>& args,
                int start = 0,
                int end = -1);


};

#endif
