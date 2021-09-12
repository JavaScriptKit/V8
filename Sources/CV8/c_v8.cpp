
#include <stdlib.h> // malloc, free
#include <string.h> // memset, memcpy
#include <v8.h>
#include "c_v8.h"

#include <libplatform/libplatform.h>

using namespace v8;

class GlobalValue {
public:
    explicit GlobalValue(Isolate* isolate, Global<Value>* value):
        isolate_locker(isolate), isolate_scope(isolate), handle_scope(isolate),
        isolate(isolate), value(value) {
    }

    explicit GlobalValue(void* isolate, void* value)
    : GlobalValue(reinterpret_cast<Isolate*>(isolate), reinterpret_cast<Global<Value>*>(value)) {
    }

    ~GlobalValue() { }

    V8_INLINE Local<Value> operator*() const {
        return value->Get(isolate);
    }

    V8_INLINE Local<Value> operator->() const {
        return value->Get(isolate);
    }

private:
    Locker isolate_locker;
    Isolate::Scope isolate_scope;
    HandleScope handle_scope;

    Isolate* isolate;
    Global<Value>* value;

    // Prevent copying of GlobalValue objects.
    GlobalValue(const GlobalValue&);
    GlobalValue& operator=(const GlobalValue&);
};

extern "C" {

    // MARK: v8 specific

    void* initialize(const char *exec_path) {
        V8::InitializeICUDefaultLocation(exec_path);
        v8::V8::InitializeExternalStartupData(exec_path);
        auto platform = platform::NewDefaultPlatform().release();
        V8::InitializePlatform(platform);
        V8::Initialize();
        return platform;
    }

    void dispose(void* platform) {
        V8::Dispose();
        V8::ShutdownPlatform();
        delete reinterpret_cast<Platform*>(platform);;
    }

    void* createIsolate() {
        Isolate::CreateParams create_params;
        create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        return Isolate::New(create_params);
    }

    void disposeIsolate(void* isolate) {
        reinterpret_cast<Isolate*>(isolate)->Dispose();
    }

    // MARK: shared with node
    
    void* createTemplate(void* isolatePtr) {
        auto isolate = reinterpret_cast<Isolate*>(isolatePtr);
        Locker isolateLocker(isolate);
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);
        Local<ObjectTemplate> globalObject = ObjectTemplate::New(isolate);
        return new Global<ObjectTemplate>(isolate, globalObject);
    }

    void disposeTemplate(void* context) {
        delete reinterpret_cast<Global<ObjectTemplate>*>(context);
    }

    void* createContext(void* isolatePtr, void* templatePtr) {
        auto isolate = reinterpret_cast<Isolate*>(isolatePtr);
        auto globalGlobalTemplate = reinterpret_cast<Global<ObjectTemplate>*>(templatePtr);

        Locker isolateLocker(isolate);
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);

        auto globalTemplate = globalGlobalTemplate->Get(isolate);
        Local<Context> context = Context::New(isolate, NULL, globalTemplate);
        return new Global<Context>(isolate, context);
    }

    void disposeContext(void* context) {
        delete reinterpret_cast<Global<Context>*>(context);
    }

    void* evaluate(void* isolatePtr, void* contextPtr, const char* scriptPtr, void** exception) {
        auto isolate = reinterpret_cast<Isolate*>(isolatePtr);
        auto globalContext = reinterpret_cast<Global<Context>*>(contextPtr);

        Locker isolateLocker(isolate);
        TryCatch trycatch(isolate);
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);
        Local<Context> context = globalContext->Get(isolate);
        Context::Scope context_scope(context);
        Local<String> source = String::NewFromUtf8(isolate, scriptPtr).ToLocalChecked();
        MaybeLocal<Script> maybeScript = Script::Compile(context, source);

        if(maybeScript.IsEmpty()) {
            if (exception != nullptr) {
                *exception = new Global<Value>(isolate, trycatch.Exception());
            }
            return nullptr;
        }

        Local<Script> script = maybeScript.ToLocalChecked();
        MaybeLocal<Value> result = script->Run(context);

        if (result.IsEmpty()) {
            if (exception != nullptr) {
                *exception = new Global<Value>(isolate, trycatch.Exception());
            }
            return nullptr;
        }
        return new Global<Value>(isolate, result.ToLocalChecked());
    }

    void disposeValue(void* pointer) {
        delete reinterpret_cast<Global<Value>*>(pointer);
    }

    int64_t valueToInt(void* isolatePtr, void* value) {
        auto isolate = reinterpret_cast<Isolate*>(isolatePtr);
        GlobalValue scoped(isolate, value);
        return scoped->IntegerValue(isolate->GetCurrentContext()).ToChecked();
    }

    int getUtf8StringLength(void* isolatePtr, void* value) {
        auto isolate = reinterpret_cast<Isolate*>(isolatePtr);
        GlobalValue scoped(isolate, value);
        String::Utf8Value utf8(isolate, *scoped);
        return utf8.length();
    }

    void copyUtf8String(void* isolatePtr, void* value, void* buffer, int count) {
        auto isolate = reinterpret_cast<Isolate*>(isolatePtr);
        GlobalValue scoped(isolate, value);
        String::Utf8Value utf8(isolate, *scoped);
        memcpy(buffer, *utf8, count);
    }

    // MARK: type checks

    bool isNull(void* isolate, void* value) {
        GlobalValue scoped(isolate, value);
        return scoped->IsNull();
    }

    bool isUndefined(void* isolate, void* value) {
        GlobalValue scoped(isolate, value);
        return scoped->IsUndefined();
    }

    bool isBoolean(void* isolate, void* value) {
        GlobalValue scoped(isolate, value);
        return scoped->IsBoolean();
    }

    bool isNumber(void* isolate, void* value) {
        GlobalValue scoped(isolate, value);
        return scoped->IsNumber();
    }

    bool isString(void* isolate, void* value) {
        GlobalValue scoped(isolate, value);
        return scoped->IsString();
    }

    bool isObject(void* isolate, void* value) {
        GlobalValue scoped(isolate, value);
        return scoped->IsObject();
    }

    // MARK: functions

    static void callback(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto context = args.GetIsolate()->GetCurrentContext();
        int32_t id = args.Data()->Int32Value(context).ToChecked();

        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        void* values[args.Length()];
        for(int i = 0; i < args.Length(); i++) {
            values[i] = new Global<Value>(isolate, args[i]);
        }
        auto returnValue = args.GetReturnValue();
        swiftCallback(isolate, id, values, args.Length(), &returnValue);
    }

    void createFunction(void* isolatePtr, void* contextPtr, const char* namePtr, int32_t id) {
        auto isolate = reinterpret_cast<Isolate*>(isolatePtr);
        auto globalContext = reinterpret_cast<Global<Context>*>(contextPtr);

        Locker isolateLocker(isolate);
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);

        auto context = globalContext->Get(isolate);
        Context::Scope context_scope(context);

        auto data = Integer::New(isolate, id);
        auto name = String::NewFromUtf8(isolate, namePtr);
        auto functionTemplate = FunctionTemplate::New(isolate, callback, data);
        auto prototype = Local<Object>::Cast(context->Global()->GetPrototype());
        auto result = prototype->Set(context, name.ToLocalChecked(), functionTemplate->GetFunction(context).ToLocalChecked());
    }

    void setReturnValueUndefined(void* isolatePtr, void* returnValuePtr) {
        auto returnValue = reinterpret_cast<ReturnValue<Value>*>(returnValuePtr);
        returnValue->SetUndefined();
    }

    void setReturnValueNull(void* isolatePtr, void* returnValuePtr) {
        auto returnValue = reinterpret_cast<ReturnValue<Value>*>(returnValuePtr);
        returnValue->SetNull();
    }

    void setReturnValueBoolean(void* isolatePtr, void* returnValuePtr, bool value) {
        auto returnValue = reinterpret_cast<ReturnValue<Value>*>(returnValuePtr);
        returnValue->Set(value);
    }

    void setReturnValueNumber(void* isolatePtr, void* returnValuePtr, double value) {
        auto returnValue = reinterpret_cast<ReturnValue<Value>*>(returnValuePtr);
        returnValue->Set(value);
    }

    void setReturnValueString(void* isolatePtr, void* returnValuePtr, const char* utf8) {
        auto isolate = reinterpret_cast<Isolate*>(isolatePtr);
        auto returnValue = reinterpret_cast<ReturnValue<Value>*>(returnValuePtr);

        auto string = String::NewFromUtf8(isolate, utf8);
        returnValue->Set(string.ToLocalChecked());
    }

    void setReturnValueEmptyString(void* isolatePtr, void* returnValuePtr) {
        auto returnValue = reinterpret_cast<ReturnValue<Value>*>(returnValuePtr);
        returnValue->SetEmptyString();
    }

    // MARK: properties

    void* getProperty(void* isolatePtr, void* valuePtr, const char* keyPtr, void** exception) {
        auto isolate = reinterpret_cast<Isolate*>(isolatePtr);
        auto value = reinterpret_cast<Global<Value>*>(valuePtr);

        Locker isolateLocker(isolate);
        TryCatch trycatch(isolate);
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);

        auto object = value->Get(isolate)->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
        auto context = isolate->GetEnteredOrMicrotaskContext();
        auto key = String::NewFromUtf8(isolate, keyPtr);

        auto result = object->GetRealNamedProperty(context, key.ToLocalChecked());

        if (result.IsEmpty()) {
            if (exception != nullptr) {
                *exception = new Global<Value>(isolate, trycatch.Exception());
            }
            return nullptr;
        }
        return new Global<Value>(isolate, result.ToLocalChecked());
    }
}
