#include "script_console.h"
#include <iostream>
#include <sstream>

using namespace v8;

void Console::Create(Isolate* isolate, Local<Context> context) {
    Handle<ObjectTemplate> console = ObjectTemplate::New(isolate);
    console->Set(String::NewFromUtf8(isolate, "log"),
                 FunctionTemplate::New(isolate, Print));
    console->Set(String::NewFromUtf8(isolate, "warn"),
                 FunctionTemplate::New(isolate, Print));
    console->Set(String::NewFromUtf8(isolate, "info"),
                 FunctionTemplate::New(isolate, Print));
    console->Set(String::NewFromUtf8(isolate, "error"),
                 FunctionTemplate::New(isolate, Print));
    console->Set(String::NewFromUtf8(isolate, "assert"),
                 FunctionTemplate::New(isolate, Assert));
    console->Set(String::NewFromUtf8(isolate, "void"),
                 FunctionTemplate::New(isolate, VoidMessage));
    context->Global()->Set(
            String::NewFromUtf8(isolate, "console"),
            console->NewInstance());
}

void Console::Print(const FunctionCallbackInfo<Value>& args) {
    std::cout << "[Script Console] " << StringifyArguments(args) << std::endl;
    args.GetReturnValue().Set(args.Holder());
}

void Console::Assert(const FunctionCallbackInfo<Value>& args) {
    // bool to_assert = args[0]->IsFalse();
    // if (to_assert) {
    //     const std::string& assertion_msg = StringifyArguments(args, 1);

    //     auto stack_frame = StackTrace::CurrentStackTrace(
    //             args.GetIsolate(),
    //             1,
    //             StackTrace::kOverview)->GetFrame(0);
    //     auto filename = stack_frame->GetScriptName();
    //     auto line_number = stack_frame->GetLineNumber();
    //     String::Utf8Value script_name(filename);

    //     bool ignore = false;
    //     DisplayAssert(false,
    //                   assertion_msg.c_str(),
    //                   line_number,
    //                   *script_name,
    //                   ignore,
    //                   "");
    // }
    args.GetReturnValue().Set(args.Holder());
}

void Console::VoidMessage(const FunctionCallbackInfo<Value>& args) {
    args.GetReturnValue().Set(args.Holder());
}

std::string Console::StringifyArguments(const FunctionCallbackInfo<Value>& args,
                                   int start,
                                   int end) {
    std::stringstream sb;
    end = (end == -1 ? args.Length() : end);
    for (int i = start; i != end; ++i) {
        if (i > start)
            sb << " ";
        String::Utf8Value str(args[i]);
        sb << *str;
    }
    return sb.str();
}
