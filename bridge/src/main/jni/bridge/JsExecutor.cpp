#include "JsExecutor.h"

JsExecutor* JsExecutor::mInstance = NULL;

template<JSValueRef (JsExecutor::*f)(JSObjectRef, JSObjectRef, size_t, const JSValueRef[])>
inline JSObjectCallAsFunctionCallback wrapMethod() {
    struct funcWrapper {
        static JSValueRef call(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
         const JSValueRef arguments[], JSValueRef *exception)
        {

           JSObjectRef obj = JSContextGetGlobalObject(ctx);
           JsExecutor* executor = static_cast<JsExecutor*> (JSObjectGetPrivate(obj));
           return (executor->*f)(thisObject, function, argumentCount, arguments);
           //return JSValueMakeNull(ctx);
        }
    };

    return &funcWrapper::call;
}

static JSValueRef onGetModuleProperty(JSContextRef ctx, JSObjectRef target, JSStringRef propertyName, JSValueRef* execption) {
    char name[1024];
    JSStringGetUTF8CString(propertyName, name, 1024);

    JsExecutor* mJsExecutor = JsExecutor::getInstance();
    std::map<char*, NativeModule*> list =  mJsExecutor->getNativeModules();

    JSObjectRef ref = NULL;
    std::map<char*, NativeModule*>::iterator iter;
        for (iter = list.begin(); iter != list.end(); iter++) {
             if (strcmp(iter->first, name) == 0) {
                 ref = iter->second->moduleId;
                 break;
             }
        }

    if (ref == NULL) {
        return JSValueMakeNull(ctx);
    }

    return ref;
}

static JSValueRef onGetSubProperty(JSContextRef ctx, JSObjectRef target, JSStringRef propertyName, JSValueRef* execption) {
    char name[1024];
    JSStringGetUTF8CString(propertyName, name, 1024);

    JsExecutor* mJsExecutor = JsExecutor::getInstance();
    std::map<char*, NativeModule*> list =  mJsExecutor->getNativeModules();

    JSObjectRef ref = NULL;
    std::map<char*, NativeModule*>::iterator iter;
        for (iter = list.begin(); iter != list.end(); iter++) {
             if (target == iter->second->moduleId) {
                 Method** params = iter->second->params;
                         Method** p;

                         int i = 0;
                         int len = iter->second->number;
                         for (p = params; p != NULL && i < len; p++, i++) {
                              if (strcmp((*p)->name, name) == 0) {
                                  ref = (*p)->functionId;
                                  break;
                              }
                         }
             }
        }

    if (ref == NULL) {
        return JSValueMakeNull(ctx);
    }
    return ref;
}

JsExecutor::JsExecutor() {
    globalContext = JSGlobalContextCreate(NULL);
    globalObject = JSContextGetGlobalObject(globalContext);
}

JsExecutor::~JsExecutor() {
}

void JsExecutor::loadJs(const char* js, JavaVM* vm) {
    mVm = vm;

    if (globalContext == NULL) {
        LOGD("globalContext is NULL");
        return;
    }

    JSValueRef exception;
    JSStringRef scriptJS = JSStringCreateWithUTF8CString(js);
    JSValueRef ret = JSEvaluateScript(globalContext, scriptJS, NULL, NULL, 0, &exception);

    if (ret <= 0) {
        char err[1024];
        this->printErrors(exception, err, 1024);
        return ;
    }

    JSStringRelease(scriptJS);

    //get bridge object (bridge object name is "global")
    bridgeObject = this->getProperty(globalObject, "global");
    if (bridgeObject == NULL) {
        LOGD("bridgeObject get NULL");
        return ;
    }

    batchObject = this->getProperty(bridgeObject, "BridgeBatch");
}

/**
* 获取属性对象
**/
JSObjectRef JsExecutor::getProperty(JSObjectRef ref, const char* name) {
    JSStringRef objName=JSStringCreateWithUTF8CString(name);
    JSValueRef obj = JSObjectGetProperty(globalContext, ref, objName,NULL);
    if (obj == NULL) {
        LOGD("failed to get property %s", name);
    }
    JSStringRelease(objName);
    return JSValueToObject(globalContext, obj, NULL);
}

JSObjectRef JsExecutor::getNativeModulesProperty() {
    return nativeModuleId;
}

void JsExecutor::printErrors(JSValueRef exception, char* errs, int len) {
     if (globalContext == NULL) {
         LOGD("globalContext is NULL");
         return;
     }

     JSStringRef errStr = JSValueToStringCopy(globalContext, exception, NULL);
     JSStringGetUTF8CString(errStr, errs, len);
     LOGD("%s", errs);
     JSStringRelease(errStr);
}

/**
* 注册NativeModules节点
*/
void JsExecutor::registerBaseModule() {

    if (globalContext == NULL) {
        LOGD("globalContext is NULL");
        return;
    }

    if (bridgeObject == NULL) {
        LOGD("bridgeObject is NULL");
    }

    //not use any more
    /*JSClassRef proxyClass = JSClassCreate(&jsProxyDefinition);
    JSObjectRef proxyObj = JSObjectMake(globalContext, proxyClass, NULL);
    JSStringRef name = JSStringCreateWithUTF8CString("NativeModules");
    JSObjectSetProperty(globalContext, bridgeObject, name, proxyObj, kJSPropertyAttributeNone, 0);
    JSStringRelease(name);
    JSClassRelease(proxyClass);*/
    nativeModuleId = this->installModule(bridgeObject, "NativeModules", onGetModuleProperty);

    //this->installFunction(bridgeObject, "aaa", exceptionWrapMethod<&JsExecutor::runNativeMethod>());
}

/**
* 添加函数节点到相应的对象中
**/
JSObjectRef JsExecutor::installFunction(JSObjectRef object, const char* name, JSObjectCallAsFunctionCallback callback) {
    if (globalContext == NULL) {
        LOGD("globalContext is NULL");
        return NULL;
    }

    JSStringRef jsName = JSStringCreateWithUTF8CString(name);
    JSObjectRef functionObj = JSObjectMakeFunctionWithCallback(globalContext, jsName, callback);
    JSObjectSetProperty(globalContext, object, jsName, functionObj, 0, NULL);
    JSStringRelease(jsName);
    return functionObj;
}

/**
* 添加对象到相应的对象中
**/
JSObjectRef JsExecutor::installModule(JSObjectRef object, const char* name, JSObjectGetPropertyCallback callback) {
    if (globalContext == NULL) {
        LOGD("globalContext is NULL");
        return NULL;
    }

    JSClassDefinition definition = kJSClassDefinitionEmpty;
    definition.className = name;
    definition.getProperty = callback;

    JSClassRef proxyClass = JSClassCreate(&definition);
    JSObjectRef proxyObj = JSObjectMake(globalContext, proxyClass, NULL);
    JSStringRef jsName = JSStringCreateWithUTF8CString(name);
    JSObjectSetProperty(globalContext, object, jsName, proxyObj, 0, NULL);
    JSStringRelease(jsName);
    JSClassRelease(proxyClass);


    std::map<char*, NativeModule*>::iterator iter;
    for (iter = modulesVec.begin(); iter != modulesVec.end(); iter++) {
         if (strcmp(iter->first, name) == 0) {
             iter->second->moduleId = proxyObj;
         }
    }

    return proxyObj;
}

void JsExecutor::callJsFunction(const char* module, int count, const JSValueRef args[], const char* fun, char* retStr, int len) {
    if (globalContext == NULL) {
        LOGD("globalContext is NULL");
        return ;
    }
    if (bridgeObject == NULL) {
        LOGD("bridgeObject is NULL");
        return ;
    }

    if (batchObject == NULL) {
        LOGD("batchObject is NULL");
        return ;
    }

    JSObjectRef target = this->getProperty(batchObject, module);
    if (target == NULL) {
        LOGD("no such module %s", module);
        sprintf(retStr, "no sunch module [%s]", module);
        return ;
    }

    JSStringRef nameJS = JSStringCreateWithUTF8CString(fun);
    bool hasFunc = JSObjectHasProperty(globalContext, target, nameJS);
    if (!hasFunc) {
         LOGD("no sunch function %s", fun);
         JSStringRelease(nameJS);
         sprintf(retStr, "no sunch function [%s]", fun);
         return ;
    }

    JSObjectRef function = (JSObjectRef)JSObjectGetProperty(globalContext, target, nameJS, NULL);
    JSStringRelease(nameJS);


    JSValueRef exception;
    JSValueRef result = JSObjectCallAsFunction(globalContext, function, NULL, count, args, &exception);
    if (result <= 0) {
        this->printErrors(exception, retStr, len);
        return ;
    }

    JSStringRef resultStr = JSValueToStringCopy(globalContext, result, NULL);

    JSStringGetUTF8CString(resultStr, retStr, len);
    LOGD("callFunction result %s", retStr);

    JSStringRelease(resultStr);
}

void JsExecutor::addModules(NativeModule* m) {
    modulesVec.insert(std::pair<char*, NativeModule*>(m->name, m));

    Method** params = m->params;
    Method** p;

    JSObjectRef ref = this->getNativeModulesProperty();
    this->installModule(ref, m->name, onGetSubProperty);

    int i = 0;
    int len = m->number;
    for (p = params; p != NULL && i < len; p++, i++) {
         (*p)->functionId = this->installFunction(ref, (*p)->name, wrapMethod<&JsExecutor::runNativeMethod>());
    }
}

std::map<char*, NativeModule*> JsExecutor::getNativeModules() {
    return modulesVec;
}

 JSGlobalContextRef JsExecutor::getContext() {
     return globalContext;
 }

 JavaVM* JsExecutor::getVm() {
     return mVm;
 }

JSValueRef JsExecutor::runNativeMethod(JSObjectRef thisObject, JSObjectRef function, size_t count, const JSValueRef args[]) {

    jobject call_module = NULL;
    jmethodID call_methodId = NULL;
    char* call_sign = NULL;

    JsExecutor* mJsExecutor = JsExecutor::getInstance();

    std::map<char*, NativeModule*> list =  mJsExecutor->getNativeModules();

    std::map<char*, NativeModule*>::iterator iter;
    for (iter = list.begin(); iter != list.end(); iter++) {
         if (iter->second->moduleId == thisObject) {

              call_module = iter->second->module_obj;
              Method** params = iter->second->params;
              Method** p;

              int i = 0;
              int len = iter->second->number;
              for (p = params; p != NULL && i < len; p++, i++) {
                  if (function == (*p)->functionId) {
                      call_methodId = (*p)->methodId;
                      call_sign = (*p)->sign;
                      break;
                  }
              }
         }
    }

    if (call_module ==NULL) {
         LOGD("call_module is NULL");
         return JSValueMakeNull(mJsExecutor->getContext());
    }

    if (call_methodId == NULL) {
        LOGD("call_methodId is NULL");
        return JSValueMakeNull(mJsExecutor->getContext());
    }

    if (call_sign == NULL) {
        LOGD("call_sign is NULL");
        return JSValueMakeNull(mJsExecutor->getContext());
    }

    return callNativeMethod(mJsExecutor->getContext(), call_module, call_methodId, call_sign, count, args);
}

JSValueRef callNativeMethod(JSContextRef ctx, jobject module, jmethodID method, const char* sign, size_t count, const JSValueRef args[]) {

    JsExecutor* mJsExecutor = JsExecutor::getInstance();
    JavaVM* vm = mJsExecutor->getVm();
        if (vm == NULL) {
            LOGD("javaVM is NULL");
            return JSValueMakeNull(ctx);
        }

        JNIEnv *env = NULL;
        if (vm->AttachCurrentThread(&env, NULL) != 0) {
            LOGD("getEnv is NULL");
            return JSValueMakeNull(ctx);
        }


    int len = strlen(sign);
    char *p = strchr(sign, ')');
    if (p == NULL) {
        LOGD("error sign");
        return JSValueMakeNull(ctx);
    }

    jvalue params[count];
    makeJniParam(env, ctx, params, args, count, sign);

    p++;
    if (p[0] == 'V') {
        env->CallVoidMethodA(module, method, params);
    } else if (p[0] == 'I') {
        int res = env->CallIntMethodA(module, method, params);
        return JSValueMakeNumber(ctx, (double) res);
    } else if (p[0] == 'J') {
        int res = env->CallLongMethodA(module, method, params);
        return JSValueMakeNumber(ctx, (double) res);
    } else if (p[0] == 'B') {
        int res = env->CallByteMethodA(module, method, params);
        return JSValueMakeNumber(ctx, (double) res);
    } else if (p[0] == 'C') {
        int res = env->CallCharMethodA(module, method, params);
        return JSValueMakeNumber(ctx, (double) res);
    } else if (p[0] == 'S') {
        int res = env->CallShortMethodA(module, method, params);
        return JSValueMakeNumber(ctx, (double) res);
    } else if (p[0] == 'F') {
        float res = env->CallFloatMethodA(module, method, params);
        return JSValueMakeNumber(ctx, (double) res);
    } else if (p[0] == 'D') {
        float res = env->CallDoubleMethodA(module, method, params);
        return JSValueMakeNumber(ctx, (double) res);
    } else if (p[0] == 'Z') {
        bool res = env->CallBooleanMethodA(module, method, params);
        return JSValueMakeBoolean(ctx, res);
    } else if (p[0] == 'L' && strcmp(p, "Ljava/lang/String;") == 0) {
        jstring jnistr = (jstring) env->CallObjectMethodA(module, method, params);
        char* cstr = (char*) env->GetStringUTFChars(jnistr, NULL);
        JSStringRef jsstr = JSStringCreateWithUTF8CString(cstr);
        env->ReleaseStringUTFChars(jnistr, cstr);
        return JSValueMakeString(ctx, jsstr);
    } else if (p[0] == '[') {
        p++;
        if (p[0] == 'L' && strcmp(p, "Ljava/lang/String;") == 0) {
            jobjectArray res = (jobjectArray) env->CallObjectMethodA(module, method, params);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];

            for (int i = 0; i < arr_size; i++) {
                 jstring str = (jstring) env->GetObjectArrayElement(res, i);
                 char* c_str = (char*) env->GetStringUTFChars(str, NULL);
                 JSStringRef objName = JSStringCreateWithUTF8CString(c_str);

                 jsArr[i] = JSValueMakeString(ctx, objName);
                 JSStringRelease(objName);
                 env->ReleaseStringUTFChars(str, c_str);
            }

            JSValueRef exception;
            return JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else if (p[0] == 'I') {
            jintArray res = (jintArray) env->CallObjectMethodA(module, method, params);
            jint* i_arr = env->GetIntArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, i_arr[i]);
            }

            JSValueRef exception;
            env->ReleaseIntArrayElements(res, i_arr, NULL);
            return JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else if (p[0] == 'J') {
            jlongArray res = (jlongArray) env->CallObjectMethodA(module, method, params);
            jlong* j_arr = env->GetLongArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            env->ReleaseLongArrayElements(res, j_arr, NULL);
            return JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else if (p[0] == 'B') {
            jbyteArray res = (jbyteArray) env->CallObjectMethodA(module, method, params);
            jbyte* j_arr = env->GetByteArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            env->ReleaseByteArrayElements(res, j_arr, NULL);
            return JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else if (p[0] == 'S') {
            jshortArray res = (jshortArray) env->CallObjectMethodA(module, method, params);
            jshort* j_arr = env->GetShortArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            env->ReleaseShortArrayElements(res, j_arr, NULL);
            return JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else if (p[0] == 'C') {
            jcharArray res = (jcharArray) env->CallObjectMethodA(module, method, params);
            jchar* j_arr = env->GetCharArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            env->ReleaseCharArrayElements(res, j_arr, NULL);
            return JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else if (p[0] == 'Z') {
            jbooleanArray res = (jbooleanArray) env->CallObjectMethodA(module, method, params);
            jboolean* b_arr = env->GetBooleanArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeBoolean(ctx, b_arr[i]);
            }

            JSValueRef exception;
            env->ReleaseBooleanArrayElements(res, b_arr, NULL);
            return JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else if (p[0] == 'F') {
            jfloatArray res = (jfloatArray) env->CallObjectMethodA(module, method, params);
            jfloat* j_arr = env->GetFloatArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            env->ReleaseFloatArrayElements(res, j_arr, NULL);
            return JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else if (p[0] == 'D') {
            jdoubleArray res = (jdoubleArray) env->CallObjectMethodA(module, method, params);
            jdouble* j_arr = env->GetDoubleArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            env->ReleaseDoubleArrayElements(res, j_arr, NULL);
            return JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else {
            LOGD("unsupported type error");
        }
    } else {
        LOGD("unsupported type error");
    }

    return JSValueMakeNull(ctx);
}