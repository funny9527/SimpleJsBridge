#ifndef __JSEXECUTOR_H__
#define __JSEXECUTOR_H__

#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <map>
#include "log.h"
#include <JavaScriptCore/JavaScript.h>
#include "datamap.h"

class JsExecutor;

static const char* className = "com/szy/bridge/bridge/Bridge";

JSObjectCallAsFunctionCallback wrapMethod();

/**
* js调用C++注入的对象的属性
*/
static JSValueRef onGetModuleProperty(JSContextRef ctx, JSObjectRef function, JSStringRef propertyName, JSValueRef* execption);

static JSValueRef onGetSubProperty(JSContextRef ctx, JSObjectRef function, JSStringRef propertyName, JSValueRef* execption);

JSValueRef callNativeMethod(JSContextRef ctx, jobject module, jmethodID method, const char*, size_t count, const JSValueRef args[]);

class NativeModule;

class JsExecutor {
    public:
        ~JsExecutor();
        static JsExecutor* mInstance;
        static JsExecutor* getInstance() {
            if (mInstance == NULL) {
                LOGD("new mInstance");
                mInstance = new JsExecutor();
            }

            return mInstance;
        }

        void loadJs(const char* js, JavaVM*);
        void printErrors(JSValueRef exception, char* errs, int len);
        char* callJsFunction(const char* module, int count, const JSValueRef args[], const char* fun);
        void registerBaseModule();
        void addModules(NativeModule*);
        std::map<char*, NativeModule*> getNativeModules();
        //注册函数
        JSObjectRef installFunction(JSObjectRef object, const char* name, JSObjectCallAsFunctionCallback callback);
        //注册模块
        JSObjectRef installModule(JSObjectRef object, const char* name, JSObjectGetPropertyCallback callback);
        //获取对象
        JSObjectRef getProperty(JSObjectRef ref, const char* name);
        //获取NativeModules对象
        JSObjectRef getNativeModulesProperty();

        JSGlobalContextRef getContext();

        /**
        * 本地方法执行
        **/
        JSValueRef runNativeMethod(JSObjectRef, JSObjectRef, size_t, const JSValueRef[]);

        JavaVM* getVm();
    private:
        JsExecutor();
        JavaVM* mVm;
        JSGlobalContextRef globalContext;
        //全局
        JSObjectRef globalObject;
        //global对象
        JSObjectRef bridgeObject;
        //NativeModules对象
        JSObjectRef nativeModuleId;
        JSObjectRef batchObject;
        std::map<char*, NativeModule*> modulesVec;
};

struct Method {
    char* name;
    char* sign;
    jmethodID methodId;
    JSObjectRef functionId;
};

class NativeModule {
    public:
    NativeModule(char* _name, jclass _class, jobject _obj, Method** _methods, int _num) : name(_name),
         module_class(_class), module_obj(_obj), params(_methods), number(_num)
    {
    }
    jclass module_class;
    jobject module_obj;
    char* name;
    Method** params;
    int number;
    JSObjectRef moduleId;
};

#endif