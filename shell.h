#ifndef SHELL_H
#define SHELL_H

#include <include/v8.h>
#include <include/v8-debug.h>
#include <include/libplatform/libplatform.h>

#include <glm/glm.hpp>

#include <thread>
#include <atomic>

#include "json.hpp"

#include "readerwriterqueue.h"
#include "atomicops.h"

#include "zscene_node.h"

typedef v8::Persistent<v8::Function,
                       v8::CopyablePersistentTraits<v8::Function>> GlobalFunction;
typedef v8::Persistent<v8::Function,
                       v8::NonCopyablePersistentTraits<v8::Function>> SavedFunction;

template <class TypeName, class CopyTrait>
inline v8::Local<TypeName>
StrongPersistentToLocal(const v8::Persistent<TypeName, CopyTrait> & persistent) {
    return *reinterpret_cast<v8::Local<TypeName>*>(const_cast<v8::Persistent<TypeName, CopyTrait>*>(&persistent));
}

class Shell {
    public:
        Shell(int argc, char *argv[]);
        ~Shell();

        void UninitializeV8();
        static std::atomic<bool> terminateServer;
        void LoadMainScript(const std::string& file_name, v8::Local<v8::Context>& context);
        void CreateShellContext();
        static bool ExecuteString(v8::Isolate* isolate,
                           v8::Local<v8::String> source,
                           v8::Local<v8::Value> name,
                           bool print_result,
                           bool report_exceptions
        );
        static void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Load(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Quit(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Version(const v8::FunctionCallbackInfo<v8::Value>& args);

        ZSceneNode* Init();
        void Poll();
        std::string Eval(std::string jsExpression);
        bool Update(const float elapsedTime);
        void KeyDown(int key);
        void KeyUp(int key);
        
        static v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isolate,
                                            const char* name
        );
        static void ReportException(v8::Isolate* isolate, v8::TryCatch* handler);
        GlobalFunction GetGlobalFunction(const std::string& functionName);

        static v8::Local<v8::Object> _self;
        static v8::Isolate* _isolate;
        static v8::Persistent<v8::Context> _global_context;
        static v8::Platform* _platform;


    private:
        static inline const char* ToCString(const v8::String::Utf8Value& value) {
            return *value ? *value : "<string conversion failed>";
        }

        inline v8::Local<v8::Context> Context() {
            return StrongPersistentToLocal(_global_context);
        }


        std::thread _debugger_thread;
        static int _main_debug_client_socket;

        void DebuggerThread();

        moodycamel::ReaderWriterQueue<std::string> UnevaluatedQueue;
        moodycamel::ReaderWriterQueue<std::string> EvaluatedQueue;
};
#endif
