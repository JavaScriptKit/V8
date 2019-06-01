#ifndef c_v8_h
#define c_v8_h

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    // MARK: v8 specific

    // global
    void * _Nonnull initialize(const char * _Nonnull exec_path);
    void dispose(void * _Nonnull platform);
    // isolate
    void * _Nonnull createIsolate();
    void disposeIsolate(void * _Nonnull isolate);

    // MARK: shared with node

    // global template
    void * _Nonnull createTemplate(void * _Nonnull isolate);
    void disposeTemplate(void * _Nonnull context);
    // context
    void * _Nonnull createContext(void * _Nonnull isolate, void * _Nonnull globalTemplate);
    void disposeContext(void * _Nonnull context);


    // called from JSValue's destructor
    void disposeValue(void * _Nonnull pointer);

    void * _Nullable evaluate(void * _Nonnull isolatePtr, void * _Nonnull contextPtr, const char* _Nonnull scriptPtr, void * _Nullable* _Nullable exception);

    int getUtf8StringLength(void * _Nonnull isolatePtr, void * _Nonnull valuePtr);
    void copyUtf8String(void * _Nonnull isolatePtr, void * _Nonnull valuePtr, void * _Nonnull buffer, int count);

    int64_t valueToInt(void * _Nonnull isolatePtr, void * _Nonnull valuePtr);


    bool isNull(void * _Nonnull isolatePtr, void * _Nonnull valuePtr);
    bool isUndefined(void * _Nonnull isolatePtr, void * _Nonnull valuePtr);
    bool isBoolean(void * _Nonnull isolatePtr, void * _Nonnull valuePtr);
    bool isNumber(void * _Nonnull isolatePtr, void * _Nonnull valuePtr);
    bool isString(void * _Nonnull isolatePtr, void * _Nonnull valuePtr);
    bool isObject(void * _Nonnull isolatePtr, void * _Nonnull valuePtr);


    void (* _Nullable swiftCallback)(void * _Nonnull isolate, int32_t id, void * _Nullable * _Nonnull arguments, int32_t count, void * _Nonnull returnValue);
    void createFunction(void * _Nonnull  isolatePtr, void * _Nonnull contextPtr, const char* _Nonnull namePtr, int32_t id);

    void setReturnValueUndefined(void * _Nonnull isolatePtr, void * _Nonnull returnValuePtr);
    void setReturnValueNull(void * _Nonnull isolatePtr, void * _Nonnull returnValuePtr);
    void setReturnValueBoolean(void * _Nonnull isolatePtr, void * _Nonnull returnValuePtr, bool value);
    void setReturnValueNumber(void * _Nonnull isolatePtr, void * _Nonnull returnValuePtr, double value);
    void setReturnValueString(void * _Nonnull isolatePtr, void * _Nonnull returnValuePtr, const char* _Nonnull utf8);
    void setReturnValueEmptyString(void * _Nonnull isolatePtr, void * _Nonnull returnValuePtr);

    void * _Nullable getProperty(void * _Nonnull isolate, void * _Nonnull value, const char * _Nonnull key, void * _Nullable* _Nullable exception);


#ifdef __cplusplus
}
#endif

#endif /* c_v8_h */
