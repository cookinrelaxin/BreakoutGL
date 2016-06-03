#ifndef SCRIPT_VECTOR_3_H
#define SCRIPT_VECTOR_3_H

#include <include/v8.h>
#include "script_object_wrap.h"

using namespace v8;

class ScriptVector3 : public ScriptObjectWrap {
    public:
        static void Create(Isolate* isolate, Local<Context> context) {
            Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
            tpl->SetClassName(String::NewFromUtf8(isolate, "Vector3"));
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

            tpl->InstanceTemplate()->SetAccessor(
                    String::NewFromUtf8(isolate, "z"),
                    GetZ, 
                    SetZ);

            _constructor.Reset(isolate, tpl->GetFunction());
            context->Global()->Set(
                    String::NewFromUtf8(isolate, "Vector3"),
                    StrongPersistentToLocal(_constructor));
        }

        static Handle<Value> NewInstance(Isolate* isolate, float x, float y, float z) {
            HandleScope scope(isolate);
            Handle<Function> constructor = StrongPersistentToLocal(_constructor);
            Local<Object> instance = constructor->NewInstance();
            ScriptVector3* obj = Unwrap<ScriptVector3>(instance);
            obj->_vec = glm::vec3(x, y, z);
            
            return instance;
        }

    private:
        ScriptVector3(float x, float y, float z)
            : _vec(x, y, z) {}

        ~ScriptVector3() {}

        static void New(const FunctionCallbackInfo<Value>& info) {
            HandleScope scope(info.GetIsolate());

            float x = static_cast<float>(info[0]->ToNumber()->Value());
            float y = static_cast<float>(info[1]->ToNumber()->Value());
            float z = static_cast<float>(info[2]->ToNumber()->Value());

            ScriptVector3* obj = new ScriptVector3(x, y, z);
            obj->Wrap(info.This());

            return info.GetReturnValue().Set(info.This());
        }

        static void GetX(Local<String> _property,
                         const PropertyCallbackInfo<Value>& info) {
            const ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
            info.GetReturnValue().Set(Number::New(info.GetIsolate(), obj->_vec.x));
        }

        static void GetY(Local<String> _property,
                         const PropertyCallbackInfo<Value>& info) {
            const ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
            info.GetReturnValue().Set(Number::New(info.GetIsolate(), obj->_vec.y));
        }

        static void GetZ(Local<String> _property,
                         const PropertyCallbackInfo<Value>& info) {
            const ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
            info.GetReturnValue().Set(Number::New(info.GetIsolate(), obj->_vec.z));
        }

        static void SetX(Local<String> _property,
                         Local<Value> value,
                         const PropertyCallbackInfo<void>& info) {
            ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
            obj->_vec.x = static_cast<float>(value->ToNumber()->Value());
        }

        static void SetY(Local<String> _property,
                         Local<Value> value,
                         const PropertyCallbackInfo<void>& info) {
            ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
            obj->_vec.y = static_cast<float>(value->ToNumber()->Value());
        }

        static void SetZ(Local<String> _property,
                         Local<Value> value,
                         const PropertyCallbackInfo<void>& info) {
            ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
            obj->_vec.z = static_cast<float>(value->ToNumber()->Value());
        }

        static void Normalize(const FunctionCallbackInfo<Value>& info) {
            ScriptVector3* obj = Unwrap<ScriptVector3>(info.This());
            obj->_vec =  glm::normalize(obj->_vec);
            return info.GetReturnValue().Set(info.This());
        }

    private:
        static Persistent<Function> _constructor;

        glm::vec3 _vec;

};

Persistent<Function> ScriptVector3::_constructor;

#endif
