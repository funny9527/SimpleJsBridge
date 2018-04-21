#include <string.h>
#include <jni.h>
#include "log.h"
#include "JsExecutor.h"
#include "datamap.h"

#ifdef __cplusplus
extern "C" {
#endif

JavaVM* mGlbVm;

int nativeLoadJs(JNIEnv *env, jobject self, jstring code) {
    char* js_bundle = (char*) env->GetStringUTFChars(code, NULL);

    JsExecutor* mJsExecutor = JsExecutor::getInstance();
    mJsExecutor->loadJs(js_bundle, mGlbVm);
    return 0;
}

jstring nativeExecute(JNIEnv *env, jobject self, jstring module, jstring method, jobject map, jint size) {
    char* c_module = (char*) env->GetStringUTFChars(module, NULL);
    char* c_method = (char*) env->GetStringUTFChars(method, NULL);
    LOGD("js c_module :  %s  %s", c_module, c_method);

    JsExecutor* mJsExecutor = JsExecutor::getInstance();
    JSValueRef ref[size];
    makeJsParam(env, map, mJsExecutor->getContext(), ref);

    char jsValue[128];
    mJsExecutor->callJsFunction(c_module, size, ref, c_method, jsValue, 128);

    LOGD("get result data : %s", jsValue);

    return env->NewStringUTF(jsValue);
}

jstring nativeExecuteCallback(JNIEnv *env, jobject self, JSObjectRef fun, jobject map, jint size) {
    JsExecutor* mJsExecutor = JsExecutor::getInstance();
    JSValueRef ref[size];
    makeJsParam(env, map, mJsExecutor->getContext(), ref);

    char jsValue[1024];

    JSValueRef exception;
    JSValueRef result = JSObjectCallAsFunction((JSContextRef) mJsExecutor->getContext(), fun, NULL, size, ref, &exception);
    if (result == NULL) {
        mJsExecutor->printErrors(exception, jsValue, 1024);
    } else {
        JSStringRef resultStr = JSValueToStringCopy(mJsExecutor->getContext(), result, NULL);
        JSStringGetUTF8CString(resultStr, jsValue, 1024);
        JSStringRelease(resultStr);
    }

    LOGD("callback result ============== %s", jsValue);

    return env->NewStringUTF(jsValue);
}

void nativeInject(JNIEnv *env, jobject self) {
    JsExecutor* mJsExecutor = JsExecutor::getInstance();
    mJsExecutor->registerBaseModule();
}

void nativeSetNativeModules(JNIEnv *env, jobject self, jobject module, jstring name, jobjectArray methods, jobjectArray types) {
    jclass module_class = env->GetObjectClass(module);

    //get module name
    char* m_name = (char*) env->GetStringUTFChars(name, NULL);
    char* module_name = (char*) malloc (sizeof(char) * (strlen(m_name) + 1));
    strcpy(module_name, m_name);
    env->ReleaseStringUTFChars(name, m_name);


    int size = env->GetArrayLength(methods);
    Method** params = new Method*[size];
    for (int i = 0; i < size; i++) {
        params[i] = new Method;

        //get method name
        jstring str = (jstring) env->GetObjectArrayElement(methods, i);
        char* value = (char*) env->GetStringUTFChars(str, NULL);
        char* c_value = (char*) malloc (sizeof(char) * (strlen(value) + 1));
        strcpy(c_value, value);
        env->ReleaseStringUTFChars(str, value);

        params[i]->name = c_value;

        //get method sign
        str = (jstring) env->GetObjectArrayElement(types, i);
        value = (char*) env->GetStringUTFChars(str, NULL);
        c_value = (char*) malloc (sizeof(char) * (strlen(value) + 1));
        strcpy(c_value, value);
        env->ReleaseStringUTFChars(str, value);

        params[i]->sign = c_value;

        params[i]->methodId = env->GetMethodID(module_class, params[i]->name, params[i]->sign);
    }

    jobject globalModule = env->NewGlobalRef(module);
    NativeModule* nativeModule = new NativeModule(module_name, module_class, globalModule, params, size);
    JsExecutor* mJsExecutor = JsExecutor::getInstance();
    mJsExecutor->addModules(nativeModule);
}

static JNINativeMethod mMethods[] =
{
    {"loadJs", "(Ljava/lang/String;)I", (void *) nativeLoadJs},
    {"execute", "(Ljava/lang/String;Ljava/lang/String;Lcom/szy/bridge/bridge/DataMap;I)Ljava/lang/String;", (jstring *) nativeExecute},
    {"inject", "()V", (void *) nativeInject},
    {"setNativeModules", "(Lcom/szy/bridge/javamodule/NativeModule;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;)V",
        (void *) nativeSetNativeModules},
    {"executeCallback", "(ILcom/szy/bridge/bridge/DataMap;I)Ljava/lang/String;", (jstring *) nativeExecuteCallback}
};


int register_methods(JNIEnv *env)
{
    jclass clazz;

    clazz = env->FindClass(className );

    if (clazz == NULL)
    {
        LOGE("Can't find class %s\n", className);
        return -1;
    }

    LOGD("register native methods");

    if (env->RegisterNatives(clazz, mMethods, sizeof(mMethods) / sizeof(mMethods[0])) != JNI_OK)
    {
        LOGE("Failed registering methods for %s\n", className);
        return -1;
    }

    return 0;
}

jint JNI_OnLoad(JavaVM* vm, void *reserved)
{
	mGlbVm = vm;
    JNIEnv* env = NULL;
    jint result = -1;

    LOGD("%s: +", __FUNCTION__);

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        LOGE("ERROR: GetEnv failed.\n");
        return result;
    }

    if (register_methods(env) < 0)
    {
        LOGE("ERROR: register methods failed.\n");
        return result;
    }

    return JNI_VERSION_1_6;
}



void JNI_OnUnload(JavaVM* vm, void *reserved)
{
    return;
}

#ifdef __cplusplus
}
#endif
