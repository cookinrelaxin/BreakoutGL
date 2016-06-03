#ifndef SCRIPT_VECTOR_2_H
#define SCRIPT_VECTOR_2_H

#include <include/v8.h>
#include "script_object_wrap.h"

using namespace v8;

class ScriptVector2 : public ScriptObjectWrap {
    public:
        static Local<FunctionTemplate> FuncTempl(Isolate* isolate, Local<Context>context) {
            Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
            tpl->SetClassName(String::NewFromUtf8(isolate, "Vector2"));
            tpl->InstanceTemplate()->SetInternalFieldCount(1);

            tpl->PrototypeTemplate()->Set(
                    String::NewFromUtf8(isolate, "normalize"),
                    FunctionTemplate::New(isolate, Normalize)->GetFunction());

            tpl->InstanceTemplate()->SetAccessor(
                    String::NewFromUtf8(isolate, "x"),
                    GetX, 
                    SetX);

            tpl->InstanceTemplate()->SetAccessor(
                    String::NewFromUtf8(isolate, "y"),
                    GetY, 
                    SetY);
            return tpl;
        }
        static void Create(Isolate* isolate, Local<Context> context) {
            Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
            tpl->SetClassName(String::NewFromUtf8(isolate, "Vector2"));
            tpl->InstanceTemplate()->SetInternalFieldCount(1);

            tpl->PrototypeTemplate()->Set(
                    String::NewFromUtf8(isolate, "normalize"),
                    FunctionTemplate::New(isolate, Normalize)->GetFunction());

            tpl->InstanceTemplate()->SetAccessor(
                    String::NewFromUtf8(isolate, "x"),
                    GetX, 
                    SetX);

            tpl->InstanceTemplate()->SetAccessor(
                    String::NewFromUtf8(isolate, "y"),
                    GetY, 
                    SetY);

            _constructor.Reset(isolate, tpl->GetFunction());
            context->Global()->Set(
                    String::NewFromUtf8(isolate, "Vector2"),
                    StrongPersistentToLocal(_constructor));
        }

        static Handle<Value> NewInstance(Isolate* isolate, float x, float y) {
            HandleScope scope(isolate);
            Handle<Function> constructor = StrongPersistentToLocal(_constructor);
            Local<Object> instance = constructor->NewInstance();
            ScriptVector2* obj = Unwrap<ScriptVector2>(instance);
            obj->_vec = glm::vec2(x, y);
            std::cout << "new scriptvector 2 instance" << std::endl;
            
            return instance;
        }

        glm::vec2 _vec;

        ScriptVector2(float x, float y)
            : _vec(x, y) {}

        ~ScriptVector2() {}

    private:

        static void New(const FunctionCallbackInfo<Value>& info) {
            HandleScope scope(info.GetIsolate());

            float x = static_cast<float>(info[0]->ToNumber()->Value());
            float y = static_cast<float>(info[1]->ToNumber()->Value());

            ScriptVector2* obj = new ScriptVector2(x, y);
            obj->Wrap(info.This());

            info.GetReturnValue().Set(info.This());
        }

        static void GetX(Local<String> _property,
                         const PropertyCallbackInfo<Value>& info) {
            std::cout << "getX" << std::endl;
            const ScriptVector2* obj = Unwrap<ScriptVector2>(info.This());
            info.GetReturnValue().Set(Number::New(info.GetIsolate(), obj->_vec.x));
        }

        static void GetY(Local<String> _property,
                         const PropertyCallbackInfo<Value>& info) {
            const ScriptVector2* obj = Unwrap<ScriptVector2>(info.This());
            info.GetReturnValue().Set(Number::New(info.GetIsolate(), obj->_vec.y));
        }

        static void SetX(Local<String> _property,
                         Local<Value> value,
                         const PropertyCallbackInfo<void>& info) {
            std::cout << "setX" << std::endl;
            ScriptVector2* obj = Unwrap<ScriptVector2>(info.This());
            obj->_vec.x = static_cast<float>(value->ToNumber()->Value());
        }

        static void SetY(Local<String> _property,
                         Local<Value> value,
                         const PropertyCallbackInfo<void>& info) {
            ScriptVector2* obj = Unwrap<ScriptVector2>(info.This());
            obj->_vec.y = static_cast<float>(value->ToNumber()->Value());
        }

        static void Normalize(const FunctionCallbackInfo<Value>& info) {
            ScriptVector2* obj = Unwrap<ScriptVector2>(info.This());
            obj->_vec =  glm::normalize(obj->_vec);
            return info.GetReturnValue().Set(info.This());
        }

    private:
        static Persistent<Function> _constructor;

};

Persistent<Function> ScriptVector2::_constructor;

#endif
