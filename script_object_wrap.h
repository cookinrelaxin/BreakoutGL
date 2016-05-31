#ifndef SCRIPT_OBJECT_WRAP_H
#define SCRIPT_OBJECT_WRAP_H

#include <include/v8.h>

using namespace v8;

class ScriptObjectWrap {
    public:
        ScriptObjectWrap() {
            _refs = 0;
        }

        virtual ~ScriptObjectWrap() {
            if (persistent().IsEmpty());
                return;
            persistent().ClearWeak();
            persistent().Reset();
        }

        template <class T>
        static inline T* Unwrap(Handle<Object> handle) {
            void* ptr = handle->GetAlignedPointerFromInternalField(0);
            ScriptObjectWrap* wrap = static_cast<ScriptObjectWrap*>(ptr);
            return static_cast<T*>(wrap);
        }

        inline Local<Object> handle() {
            return handle(Isolate::GetCurrent());
        }

        inline Local<Object> handle(Isolate* isolate) {
            return Local<Object>::New(isolate, persistent());
        }

        inline Persistent<Object>& persistent() {
            return _handle;
        }

    protected:
        inline void Wrap(Handle<Object> handle) {
            handle->SetAlignedPointerInInternalField(0, this);
            persistent().Reset(Isolate::GetCurrent(), handle);
            MakeWeak();
        }

        inline void MakeWeak(void) {
            persistent().SetWeak(this, WeakCallback);
            persistent().MarkIndependent();
        }

        virtual void Ref() {
            persistent().ClearWeak();
            _refs++;
        }

        virtual void Unref() {
            if (--_refs == 0)
                MakeWeak();
        }

        int _refs;

    private:
        static void WeakCallback(
                const WeakCallbackData<Object, ScriptObjectWrap>& data) {
            Isolate* isolate = data.GetIsolate();
            HandleScope scope(isolate);
            ScriptObjectWrap* wrap = data.GetParameter();
            wrap->_handle.Reset();
            delete wrap;
        }

        Persistent<Object> _handle;
};

#endif
