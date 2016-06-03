#ifndef SCRIPT_ZNODE_H
#define SCRIPT_ZNODE_H

#include <include/v8.h>
#include "script_object_wrap.h"
#include "script_vector_2.h"
#include "znode.h"

#include <iostream>

using namespace v8;

class ScriptZNode : public ScriptObjectWrap {
    public:
        static void Create(Isolate* isolate, Local<Context> context) {
            Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
            tpl->SetClassName(String::NewFromUtf8(isolate, "ZNode"));
            tpl->InstanceTemplate()->SetInternalFieldCount(1);

            // tpl->InstanceTemplate()->SetAccessor(
            //         String::NewFromUtf8(isolate, "position"),
            //         GetPosition);
            //         // SetPosition);
            tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "position"),
                                         ScriptVector2::FuncTempl(isolate, context)); 

            _constructor.Reset(isolate, tpl->GetFunction());
            context->Global()->Set(
                    String::NewFromUtf8(isolate, "ZNode"),
                    StrongPersistentToLocal(_constructor));

        }
        static Handle<Value> NewInstance(Isolate* isolate) {
            HandleScope scope(isolate);
            Handle<Function> constructor = StrongPersistentToLocal(_constructor);
            Local<Object> instance = constructor->NewInstance();
            ScriptZNode* obj = Unwrap<ScriptZNode>(instance);
            obj->znode = ZNode();
            obj->scriptVec2Handle = ScriptVector2::NewInstance(isolate,
                                                               0,
                                                               0);
            std::cout << "new znode instance" << std::endl;

            
            return instance;
        }
    private:
        // ScriptZNode() : scriptVec2(0,0) {}
        ScriptZNode() = default;
        // ~ScriptZNode() = default;
        // ScriptZNode() {
        //     scriptVec2 = new ScriptVector2(0,0);
        // }
        ~ScriptZNode() {}
        // ~ScriptZNode() {
        //     delete scriptVec2;
        // }

        static void New(const FunctionCallbackInfo<Value>& info) {
            HandleScope scope(info.GetIsolate());

            ScriptZNode* obj = new ScriptZNode();
            obj->Wrap(info.This());
            // obj->scriptVec2Handle = ScriptVector2::NewInstance(info.GetIsolate(),
            //                                                    obj->znode.position.x,
            //                                                    obj->znode.position.y);
            obj->scriptVec2Handle = ScriptVector2::NewInstance(info.GetIsolate(),
                                                               0,
                                                               0);

            info.GetReturnValue().Set(info.This());
        }

        static void GetPosition(Local<String> _property,
                                const PropertyCallbackInfo<Value>& info) {
            std::cout << "get position" << std::endl;
            const ScriptZNode* obj = Unwrap<ScriptZNode>(info.This());
            // info.GetReturnValue().Set(
            //         ScriptVector2::NewInstance(info.GetIsolate(),
            //                                    obj->znode.position.x,
            //                                    obj->znode.position.y));

            info.GetReturnValue().Set(obj->scriptVec2Handle);
        }

        // static void SetPosition(Local<String> _property,
        //                         Local<Value> value,
        //                         const PropertyCallbackInfo<void>& info) {
        //     std::cout << "set position" << std::endl;
        //     ScriptZNode* obj = Unwrap<ScriptZNode>(info.This());
        //     ScriptVector2* vec = Unwrap<ScriptVector2>(value->ToObject());
        //     obj->znode.position = vec->_vec;
        // }

        static Persistent<Function> _constructor;

        ZNode znode;
        Handle<Value> scriptVec2Handle; 
};

Persistent<Function> ScriptZNode::_constructor;

#endif

