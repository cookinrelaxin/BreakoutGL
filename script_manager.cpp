// #include "script_manager.h"
// #include "script_console.h"
// 
// #include <fstream>
// #include <sstream>
// #include <iostream>
// 
// using namespace v8;
// 
// class ScriptObjectWrap {
//     public:
//         ScriptObjectWrap() {
//             _refs = 0;
//         }
// 
//         virtual ~ScriptObjectWrap() {
//             if (persistent().IsEmpty());
//                 return;
//             persistent().ClearWeak();
//             persistent().Reset();
//         }
// 
//         template <class T>
//         static inline T* Unwrap(Handle<Object> handle) {
//             void* ptr = handle->GetAlignedPointerFromInternalField(0);
//             ScriptObjectWrap* wrap = static_cast<ScriptObjectWrap*>(ptr);
//             return static_cast<T*>(wrap);
//         }
// 
//         inline Local<Object> handle() {
//             return handle(Isolate::GetCurrent());
//         }
// 
//         inline Local<Object> handle(Isolate* isolate) {
//             return Local<Object>::New(isolate, persistent());
//         }
// 
//         inline Persistent<Object>& persistent() {
//             return _handle;
//         }
// 
//     protected:
//         inline void Wrap(Handle<Object> handle) {
//             handle->SetAlignedPointerInInternalField(0, this);
//             persistent().Reset(Isolate::GetCurrent(), handle);
//             MakeWeak();
//         }
// 
//         inline void MakeWeak(void) {
//             persistent().SetWeak(this, WeakCallback);
//             persistent().MarkIndependent();
//         }
// 
//         virtual void Ref() {
//             persistent().ClearWeak();
//             _refs++;
//         }
// 
//         virtual void Unref() {
//             if (--_refs == 0)
//                 MakeWeak();
//         }
// 
//         int _refs;
// 
//     private:
//         static void WeakCallback(
//                 const WeakCallbackData<Object, ScriptObjectWrap>& data) {
//             Isolate* isolate = data.GetIsolate();
//             HandleScope scope(isolate);
//             ScriptObjectWrap* wrap = data.GetParameter();
//             wrap->_handle.Reset();
//             delete wrap;
//         }
// 
//         Persistent<Object> _handle;
// };
// 
// class ScriptVector3 : public ScriptObjectWrap {
//     public:
//         static void Create(Isolate* isolate, Local<Context> context) {
//             Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
//             tpl->SetClassName(String::NewFromUtf8(isolate, "Vector3"));
//             tpl->InstanceTemplate()->SetInternalFieldCount(1);
// 
//             tpl->PrototypeTemplate()->Set(
//                     String::NewFromUtf8(isolate, "normalize"),
//                     FunctionTemplate::New(isolate, Normalize)->GetFunction());
// 
//             tpl->InstanceTemplate()->SetAccessor(
//                     String::NewFromUtf8(isolate, "x"),
//                     GetX, 
//                     SetX);
// 
//             tpl->InstanceTemplate()->SetAccessor(
//                     String::NewFromUtf8(isolate, "y"),
//                     GetY, 
//                     SetY);
// 
//             tpl->InstanceTemplate()->SetAccessor(
//                     String::NewFromUtf8(isolate, "z"),
//                     GetZ, 
//                     SetZ);
// 
//             _constructor.Reset(isolate, tpl->GetFunction());
//             context->Global()->Set(
//                     String::NewFromUtf8(isolate, "Vector3"),
//                     StrongPersistentToLocal(_constructor));
//         }
// 
//         static Handle<Value> NewInstance(Isolate* isolate, float x, float y, float z) {
//             HandleScope scope(isolate);
//             Handle<Function> constructor = StrongPersistentToLocal(_constructor);
//             Local<Object> instance = constructor->NewInstance();
//             ScriptVector3* obj = Unwrap<ScriptVector3>(instance);
//             obj->_vec = glm::vec3(x, y, z);
//             
//             return instance;
//         }
// 
//     private:
//         ScriptVector3(float x, float y, float z)
//             : _vec(x, y, z) {}
// 
//         ~ScriptVector3() {}
// 
//         static void New(const FunctionCallbackInfo<Value>& info) {
//             HandleScope scope(info.GetIsolate());
// 
//             float x = static_cast<float>(info[0]->ToNumber()->Value());
//             float y = static_cast<float>(info[1]->ToNumber()->Value());
//             float z = static_cast<float>(info[2]->ToNumber()->Value());
// 
//             ScriptVector3* obj = new ScriptVector3(x, y, z);
//             obj->Wrap(info.This());
// 
//             return info.GetReturnValue().Set(info.This());
//         }
// 
//         static void GetX(Local<String> _property,
//                          const PropertyCallbackInfo<Value>& info) {
//             const ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
//             info.GetReturnValue().Set(Number::New(info.GetIsolate(), obj->_vec.x));
//         }
// 
//         static void GetY(Local<String> _property,
//                          const PropertyCallbackInfo<Value>& info) {
//             const ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
//             info.GetReturnValue().Set(Number::New(info.GetIsolate(), obj->_vec.y));
//         }
// 
//         static void GetZ(Local<String> _property,
//                          const PropertyCallbackInfo<Value>& info) {
//             const ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
//             info.GetReturnValue().Set(Number::New(info.GetIsolate(), obj->_vec.z));
//         }
// 
//         static void SetX(Local<String> _property,
//                          Local<Value> value,
//                          const PropertyCallbackInfo<void>& info) {
//             ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
//             obj->_vec.x = static_cast<float>(value->ToNumber()->Value());
//         }
// 
//         static void SetY(Local<String> _property,
//                          Local<Value> value,
//                          const PropertyCallbackInfo<void>& info) {
//             ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
//             obj->_vec.y = static_cast<float>(value->ToNumber()->Value());
//         }
// 
//         static void SetZ(Local<String> _property,
//                          Local<Value> value,
//                          const PropertyCallbackInfo<void>& info) {
//             ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
//             obj->_vec.z = static_cast<float>(value->ToNumber()->Value());
//         }
// 
//         static void Normalize(const FunctionCallbackInfo<Value>& info) {
//             ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
//             obj->_vec =  glm::normalize(obj->_vec);
//             return info.GetReturnValue().Set(info.This());
//         }
// 
//     private:
//         static Persistent<Function> _constructor;
// 
//         glm::vec3 _vec;
// 
// };
// 
// Persistent<Function> ScriptVector3::_constructor;
// 
// ScriptManager::ScriptManager()
//     : _v8_platform(nullptr) {
//         InitializeV8();
//     }
// 
// ScriptManager::~ScriptManager() {
//     UninitializeV8();
// }
// 
// class MallocArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
//     public:
//         virtual void* Allocate(size_t length) {
//             void* data = AllocateUninitialized(length);
//             return data == NULL ? data : memset(data, 0, length);
//         }
//         virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
//         virtual void Free(void* data, size_t) { free(data); }
// };
// 
// void ScriptManager::InitializeV8() {
//     std::cout << "initialize v8" << std::endl;
//     V8::InitializeICU();
//     _v8_platform = platform::CreateDefaultPlatform();
//     V8::InitializePlatform(_v8_platform);
//     V8::Initialize();
// 
//     MallocArrayBufferAllocator array_buffer_allocator;
//     Isolate::CreateParams create_params;
//     create_params.array_buffer_allocator = &array_buffer_allocator;
// 
//     _isolate = Isolate::New(create_params);
//     _isolate->Enter();
// 
//     HandleScope scope(_isolate);
// 
//     Local<v8::Context> context = Context::New(_isolate);
//     _global_context.Reset(_isolate, context);
// 
//     v8::Context::Scope contextScope(context);
// 
//     Handle<ObjectTemplate> t = ObjectTemplate::New();
//     t->SetInternalFieldCount(1);
//     _self = t->NewInstance();
//     _self->SetInternalField(0, External::New(_isolate, this));
// 
//     Console::Create(_isolate, context);
//     ScriptVector3::Create(_isolate, context);
// }
// 
// void ScriptManager::UninitializeV8() {
// 
//     _init_function = GlobalFunction();
//     _update_function = GlobalFunction();
//     _render_function = GlobalFunction();
//     _shutdown_function = GlobalFunction();
// 
//     _isolate->Exit();
//     _isolate->Dispose();
//     _isolate = nullptr;
// 
//     V8::Dispose();
//     V8::ShutdownPlatform();
//     delete _v8_platform;
// }
// 
// void ScriptManager::PollShell() {
//     return;
// }
// 
// void ScriptManager::Boot() {
//     std::cout << "boot script" << std::endl;
// 
//     HandleScope handle_scope(_isolate);
// 
//     std::cout << "THREE" << std::endl;
// 
//     std::cout << "FOUR" << std::endl;
//     Handle<Object> global = _isolate->GetCurrentContext()->Global();
//     std::cout << "FIVE" << std::endl;
//     Handle<Value> value = global->Get(
//             String::NewFromUtf8(_isolate, "init"));
//     std::cout << "SIX" << std::endl;
//     _init_function = GlobalFunction(_isolate, Handle<Function>::Cast(value));
// 
//     // _init_function = GetGlobalFunction("init");
//     // _update_function = GetGlobalFunction("update");
//     // _render_function = GetGlobalFunction("render");
//     // _shutdown_function = GetGlobalFunction("shutdown");
//     std::cout << "SEVEN" << std::endl;
// 
// 
//     HandleScope scope(_isolate);
//     Local<Function> func = Local<Function>::New(_isolate, _init_function);
//     Handle<Value> args[1];
//     func->Call(func->CreationContext()->Global(), 0, args);
// 
// }
// 
// bool ScriptManager::HasFunction(const std::string& functionName) {
//     Handle<Object> global = Context()->Global();
//     Handle<Value> value = global->Get(
//             String::NewFromUtf8(_isolate, functionName.c_str()));
//     return value->IsFunction();
// }
// 
// GlobalFunction ScriptManager::GetGlobalFunction(const std::string& functionName) {
//     std::cout << "FOUR" << std::endl;
//     Handle<Object> global = _isolate->GetCurrentContext()->Global();
//     std::cout << "FIVE" << std::endl;
//     Handle<Value> value = global->Get(
//             String::NewFromUtf8(_isolate, functionName.c_str()));
//     std::cout << "SIX" << std::endl;
//     return GlobalFunction(_isolate, Handle<Function>::Cast(value));
// }
// 
// bool ScriptManager::Update(const float elapsedTime) {
//     HandleScope scope(_isolate);
// 
//     Local<Function> func = Local<Function>::New(_isolate, _update_function);
//     Handle<Value> args[1];
//     args[0] = Number::New(_isolate, elapsedTime);
//     Handle<Value> result = func->Call(func->CreationContext()->Global(), 1, args);
// 
//     return !result->IsBoolean() || result->BooleanValue();
// }
// 
// void ScriptManager::Render() const {
//     HandleScope scope(_isolate);
// 
//     Local<Function> func = Local<Function>::New(_isolate, _render_function);
//     Handle<Value> args[1];
//     func->Call(func->CreationContext()->Global(), 0, args);
// }
// 
// void ScriptManager::Shutdown() {
//     HandleScope scope(_isolate);
//     Context::Scope debugScope(Context());
// 
//     Local<Function> func = Local<Function>::New(_isolate, _shutdown_function);
//     Handle<Value> args[1];
//     func->Call(func->CreationContext()->Global(), 0, args);
// }
// 
// void ScriptManager::PrintJson(Handle<Value> object) {
//     HandleScope scope(_isolate);
//     if (object->IsFunction())
//         return;
// 
//     Handle<Object> global = Context()->Global();
//     Handle<Object> JSON = global->Get(String::NewFromUtf8(_isolate, "JSON"))->ToObject();
//     Handle<Function> JSON_stringify = Handle<Function>::Cast(
//             JSON->Get(String::NewFromUtf8(_isolate, "stringify")));
// 
//     Handle<Value> args[1];
//     args[0] = object;
//     std::cout << *String::Utf8Value(JSON_stringify->Call(JSON, 1, args)) << std::endl;
// }
