#ifndef SCRIPT_MANAGER_H
#define SCRIPT_MANAGER_H

#include <include/v8.h>
#include <include/libplatform/libplatform.h>
#include <map>
#include <string>

#include <glm/glm.hpp>

namespace v8 {
    class Isolate;
}

typedef v8::Persistent<v8::Function,
                       v8::CopyablePersistentTraits<v8::Function>> GlobalFunction;
typedef v8::Persistent<v8::Function,
                       v8::NonCopyablePersistentTraits<v8::Function>> SavedFunction;

template <class TypeName, class CopyTrait>
inline v8::Local<TypeName>
StrongPersistentToLocal(const v8::Persistent<TypeName, CopyTrait> & persistent) {
    return *reinterpret_cast<v8::Local<TypeName>*>(const_cast<v8::Persistent<TypeName, CopyTrait>*>(&persistent));
}

class ScriptManager {
    public:
        ScriptManager();
        ~ScriptManager();

        void Boot();
        bool Update(const float elapsedTime);
        void Render() const;
        void Shutdown();

        void PollShell();

        v8::Local<v8::Context> Context() {
            return StrongPersistentToLocal(_global_context);
        };
        v8::Local<v8::Context> Context() const {
            return StrongPersistentToLocal(_global_context);
        };

        void PrintJson(v8::Handle<v8::Value> object);

    private:

        void InitializeV8();
        void UninitializeV8();

        bool HasFunction(const std::string& functionName);
        GlobalFunction GetGlobalFunction(const std::string& functionName);

        v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isloate, const char* name);
        bool ExecuteString(v8::Isolate* isolate,
                           v8::Local<v8::String> source,
                           v8::Local<v8::Value> name,
                           bool print_result,
                           bool report_exceptions
        );

        bool ReportException(v8::Handle<v8::Value> er, v8::Handle<v8::Message> message);
        bool ReportException(const v8::TryCatch& try_catch);

        v8::Platform* _v8_platform;
        v8::Isolate* _isolate;
        v8::Persistent<v8::Context> _global_context;
        v8::Local<v8::Object> _self;
        GlobalFunction _init_function;
        GlobalFunction _update_function;
        GlobalFunction _render_function;
        GlobalFunction _shutdown_function;
};

#endif
