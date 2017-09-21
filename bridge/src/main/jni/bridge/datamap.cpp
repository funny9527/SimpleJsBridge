#include "datamap.h"

jclass map_class = NULL;
jmethodID getType = NULL;
jmethodID getSize = NULL;
jmethodID getIntValue = NULL;
jmethodID getStringValue = NULL;
jmethodID getBoolValue = NULL;
jmethodID getLongValue = NULL;
jmethodID getCharValue = NULL;
jmethodID getShortValue = NULL;
jmethodID getByteValue = NULL;
jmethodID getDoubleValue = NULL;
jmethodID getFloatValue = NULL;

jmethodID getIntArray = NULL;
jmethodID getStringArray = NULL;
jmethodID getBoolArray = NULL;
jmethodID getLongArray = NULL;
jmethodID getCharArray = NULL;
jmethodID getShortArray = NULL;
jmethodID getFloatArray = NULL;
jmethodID getDoubleArray = NULL;
jmethodID getByteArray = NULL;

const char* STRING_SIGN = "Ljava/lang/String;";
const char* CALLBACK_SIGN = "Lcom/szy/bridge/javamodule/Callback;";

typedef enum {
    intType,
    stringType,
    boolType,
    longType,
    charType,
    shortType,
    floatType,
    doubleType,
    byteType,
    callbackType,
    intArrType,
    stringArrType,
    boolArrType,
    longArrType,
    charArrType,
    shortArrType,
    floatArrType,
    doubleArrType,
    byteArrType,
    callbackArrType,
} MapType;

int findType(char** p) {
    char first = **p;
    if (first == 'L') {
        int slen = strlen(STRING_SIGN);
        char value[1024];
        memset(value, 0, 1024);
        strncpy(value, *p, slen);
        int res = strcmp(value, STRING_SIGN);
        if (res == 0) {
            *p = *p + slen;
            return stringType;
        }

        slen = strlen(CALLBACK_SIGN);
        memset(value, 0, 1024);
        strncpy(value, *p, slen);
        res = strcmp(value, CALLBACK_SIGN);
        if (res == 0) {
            return callbackType;
        }
    } else if (first == 'I') {
        (*p)++;
        return intType;
    } else if (first == 'J') {
        (*p)++;
        return longType;
    } else if (first == 'Z') {
        (*p)++;
        return boolType;
    } else if (first == 'B') {
        (*p)++;
        return byteType;
    } else if (first == 'C') {
        (*p)++;
        return charType;
    } else if (first == 'S') {
        (*p)++;
        return shortType;
    } else if (first == 'F') {
        (*p)++;
        return floatType;
    } else if (first == 'D') {
        (*p)++;
        return doubleType;
    } else if (first == '[') {
        (*p)++;
        char head = **p;
        if (head == 'I') {
            (*p)++;
            return intArrType;
        } else if (head == 'L') {
            (*p)++;
            return stringArrType;
        } else if (head == 'Z') {
            (*p)++;
            return boolArrType;
        } else if (head == 'J') {
            (*p)++;
            return longArrType;
        } else if (head == 'C') {
            (*p)++;
            return charArrType;
        } else if (head == 'S') {
            (*p)++;
            return shortArrType;
        } else if (head == 'F') {
            (*p)++;
            return floatArrType;
        } else if (head == 'D') {
            (*p)++;
            return doubleArrType;
        } else if (head == 'B') {
            (*p)++;
            return byteArrType;
        }
    }

    return -1;
}

void makeJsParam(JNIEnv *env, jobject map, JSContextRef ctx, JSValueRef ref[]) {
    if (map_class == NULL) {
         jclass cls = env->GetObjectClass(map);
         map_class = (jclass) env->NewGlobalRef(cls);
    }

    if (getSize == NULL) {
        getSize = env->GetMethodID(map_class, "getSize", "()I");
    }

    int len = env->CallIntMethod(map, getSize);

    for (int i = 0; i < len; i++) {
        if (getType == NULL) {
                getType = env->GetMethodID(map_class, "getType", "(I)I");
        }

        int type = env->CallIntMethod(map, getType, i);

        if (type < 0) {
            LOGD("type not support error");

        }

        //LOGD("get type value ======= %d", type);

        if (type == intType) {
            if (getIntValue == NULL) {
                    getIntValue = env->GetMethodID(map_class, "getIntValue", "(I)I");
            }

            int res = env->CallIntMethod(map, getIntValue, i);
            ref[i] = JSValueMakeNumber(ctx, res);
        } else if (type == stringType) {
            if (getStringValue == NULL) {
                getStringValue = env->GetMethodID(map_class, "getStringValue", "(I)Ljava/lang/String;");
            }

            jstring res = (jstring) env->CallObjectMethod(map, getStringValue, i);
            char* str = (char*) env->GetStringUTFChars(res, NULL);
            JSStringRef jsstr = JSStringCreateWithUTF8CString(str);
            ref[i] = JSValueMakeString(ctx, jsstr);
        } else if (type == boolType) {
            if (getBoolValue == NULL) {
                getBoolValue = env->GetMethodID(map_class, "getBooleanValue", "(I)Z");
            }

            bool res = env->CallBooleanMethod(map, getBoolValue, i);
            ref[i] = JSValueMakeBoolean(ctx, res);
        } else if (type == longType) {
            if (getLongValue == NULL) {
                getLongValue = env->GetMethodID(map_class, "getLongValue", "(I)J");
            }
             long res = env->CallLongMethod(map, getLongValue, i);
             ref[i] = JSValueMakeNumber(ctx, res);
        } else if (type == charType) {
            if (getCharValue == NULL) {
                getCharValue = env->GetMethodID(map_class, "getCharValue", "(I)C");
            }
            char res = env->CallCharMethod(map, getCharValue, i);
            ref[i] = JSValueMakeNumber(ctx, res);
        } else if (type == shortType) {
            if (getShortValue == NULL) {
                getShortValue = env->GetMethodID(map_class, "getShortValue", "(I)S");
            }
            short res = env->CallShortMethod(map, getShortValue, i);
            ref[i] = JSValueMakeNumber(ctx, res);
        } else if (type == byteType) {
            if (getByteValue == NULL) {
                getByteValue = env->GetMethodID(map_class, "getByteValue", "(I)B");
            }
            char res = env->CallByteMethod(map, getByteValue, i);
            ref[i] = JSValueMakeNumber(ctx, res);
        } else if (type == doubleType) {
            if (getDoubleValue == NULL) {
                getDoubleValue = env->GetMethodID(map_class, "getDoubleValue", "(I)D");
            }
            double res = env->CallDoubleMethod(map, getDoubleValue, i);
            ref[i] = JSValueMakeNumber(ctx, res);
        } else if (type == floatType) {
            if (getFloatValue == NULL) {
                getFloatValue = env->GetMethodID(map_class, "getFloatValue", "(I)F");
            }
            float res = env->CallFloatMethod(map, getFloatValue, i);
            ref[i] = JSValueMakeNumber(ctx, res);
        } else if (type == intArrType) {
            if (getIntArray == NULL) {
                getIntArray = env->GetMethodID(map_class, "getIntArray", "(I)[I");
            }

            jintArray res = (jintArray) env->CallObjectMethod(map, getIntArray, i);
            jint* i_arr = env->GetIntArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, i_arr[i]);
            }

            JSValueRef exception;
            ref[i] = JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
            env->ReleaseIntArrayElements(res, i_arr, NULL);
        } else if (type == stringArrType) {
            if (getStringArray == NULL) {
                getStringArray = env->GetMethodID(map_class, "getStringArray", "(I)[Ljava/lang/String;");
            }

            jobjectArray res = (jobjectArray) env->CallObjectMethod(map, getStringArray, i);
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
            ref[i] = JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
        } else if (type == boolArrType) {
            if (getBoolArray == NULL) {
                getBoolArray = env->GetMethodID(map_class, "getBooleanArray", "(I)[Z");
            }

            jbooleanArray res = (jbooleanArray) env->CallObjectMethod(map, getBoolArray, i);
            jboolean* b_arr = env->GetBooleanArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeBoolean(ctx, b_arr[i]);
            }

            JSValueRef exception;
            ref[i] = JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
            env->ReleaseBooleanArrayElements(res, b_arr, NULL);
        } else if (type == longArrType) {
            if (getLongArray == NULL) {
                getLongArray = env->GetMethodID(map_class, "getLongArray", "(I)[J");
            }

            jlongArray res = (jlongArray) env->CallObjectMethod(map, getLongArray, i);
            jlong* j_arr = env->GetLongArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            ref[i] = JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
            env->ReleaseLongArrayElements(res, j_arr, NULL);
        } else if (type == charArrType) {
            if (getCharArray == NULL) {
                getCharArray = env->GetMethodID(map_class, "getCharArray", "(I)[C");
            }

            jcharArray res = (jcharArray) env->CallObjectMethod(map, getCharArray, i);
            jchar* j_arr = env->GetCharArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            ref[i] = JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
            env->ReleaseCharArrayElements(res, j_arr, NULL);
        } else if (type == shortArrType) {
            if (getShortArray == NULL) {
                getShortArray = env->GetMethodID(map_class, "getShortArray", "(I)[S");
            }

            jshortArray res = (jshortArray) env->CallObjectMethod(map, getShortArray, i);
            jshort* j_arr = env->GetShortArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            ref[i] = JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
            env->ReleaseShortArrayElements(res, j_arr, NULL);
        } else if (type == floatArrType) {
            if (getFloatArray == NULL) {
                getFloatArray = env->GetMethodID(map_class, "getFloatArray", "(I)[F");
            }

            jfloatArray res = (jfloatArray) env->CallObjectMethod(map, getFloatArray, i);
            jfloat* j_arr = env->GetFloatArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            ref[i] = JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
            env->ReleaseFloatArrayElements(res, j_arr, NULL);
        } else if (type == doubleArrType) {
            if (getDoubleArray == NULL) {
                getDoubleArray = env->GetMethodID(map_class, "getDoubleArray", "(I)[D");
            }

            jdoubleArray res = (jdoubleArray) env->CallObjectMethod(map, getDoubleArray, i);
            jdouble* j_arr = env->GetDoubleArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            ref[i] = JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
            env->ReleaseDoubleArrayElements(res, j_arr, NULL);
        } else if (type == byteArrType) {
            if (getByteArray == NULL) {
                getByteArray = env->GetMethodID(map_class, "getByteArray", "(I)[B");
            }

            jbyteArray res = (jbyteArray) env->CallObjectMethod(map, getByteArray, i);
            jbyte* j_arr = env->GetByteArrayElements(res, NULL);
            int arr_size = env->GetArrayLength(res);
            JSValueRef jsArr[arr_size];
            for (int i = 0; i < arr_size; i++) {
                jsArr[i] = JSValueMakeNumber(ctx, j_arr[i]);
            }

            JSValueRef exception;
            ref[i] = JSObjectMakeArray(ctx, arr_size, jsArr, &exception);
            env->ReleaseByteArrayElements(res, j_arr, NULL);
        }
    }
}

void makeJniParam(JNIEnv *env, JSContextRef ctx, jvalue params[], const JSValueRef args[], int count, const char* sign) {

    sign++;
    int len = strlen(sign);
    char* p = (char*) malloc (sizeof(char) * len);
    memset(p, 0, sizeof(char) * len);
    strncpy(p, sign, len - 1);

    for (int i = 0; i < count; i++) {
         JSValueRef temp = args[i];

         int type = findType(&p);

         if (type < 0) {
             LOGD("type not support error!");
             return ;
         }

         if (type == stringType) {
              char values[1024];
              JSStringRef str = JSValueToStringCopy(ctx, temp, NULL);
              JSStringGetUTF8CString(str, values, 1024);
              JSStringRelease(str);
             params[i].l = env->NewStringUTF(values);
         } else if (type == callbackType) {
              jclass cls = env->FindClass("com/szy/bridge/javamodule/Callback");
              jmethodID con = env->GetMethodID(cls, "<init>", "(I)V");
              jobject callobj = env->NewObject(cls, con, temp);
              params[i].l = callobj;
         } else if (type == boolType) {
              bool b = JSValueToBoolean(ctx, temp);
              params[i].z = (jboolean) b;
         } else if (type == doubleType) {
               JSValueRef exception;
               double ddd = JSValueToNumber(ctx, temp, &exception);
               params[i].d = (jdouble) ddd;

         } else if (type == intType) {
               JSValueRef exception;
               int ddd = JSValueToNumber(ctx, temp, &exception);
               params[i].i = (jint) ddd;
         } else if (type == longType) {
               JSValueRef exception;
               long ddd = JSValueToNumber(ctx, temp, &exception);
               params[i].j = (jlong) ddd;
         } else if (type == shortType) {
               JSValueRef exception;
               short ddd = JSValueToNumber(ctx, temp, &exception);
               params[i].s = (jshort) ddd;
         } else if (type == charType) {
               JSValueRef exception;
               char ddd = JSValueToNumber(ctx, temp, &exception);
               params[i].c = (jchar) ddd;
         } else if (type == byteType) {
               JSValueRef exception;
               jbyte ddd = JSValueToNumber(ctx, temp, &exception);
               params[i].b = (jbyte) ddd;
         } else if (type == floatType) {
               JSValueRef exception;
               float ddd = JSValueToNumber(ctx, temp, &exception);
               params[i].f = (jfloat) ddd;
         } else if (type == intArrType) {
             int size = getJsArraySize(ctx, temp);
             int i_data[size];
             for (int i = 0; i < size; i++) {
                 i_data[i] = getJSArrayItem(ctx, temp, i);
             }

             jintArray i_arr =env->NewIntArray(size);
             env->SetIntArrayRegion(i_arr, 0, size, i_data);
             params[i].l = i_arr;
         } else if (type == stringArrType) {
              int size = getJsArraySize(ctx, temp);

              jclass cls = env->FindClass("java/lang/String");

              jobjectArray o_arr = env->NewObjectArray(size, cls, NULL);
              for (int i = 0; i < size; i++) {
                  char c_str[1024];
                  getJSArrayStringItem(ctx, temp, i, c_str);
                  jstring str = env->NewStringUTF(c_str);
                  env->SetObjectArrayElement(o_arr, i, str);
              }

              params[i].l = o_arr;
         } else if (type == boolArrType) {
             int size = getJsArraySize(ctx, temp);
             jboolean b_data[size];
             for (int i = 0; i < size; i++) {
                 b_data[i] = getJSArrayItem(ctx, temp, i);
             }

             jbooleanArray b_arr = env->NewBooleanArray(size);
             env->SetBooleanArrayRegion(b_arr, 0, size, b_data);
             params[i].l = b_arr;
         } else if (type == longArrType) {
             int size = getJsArraySize(ctx, temp);
             jlong j_data[size];
             for (int i = 0; i < size; i++) {
                 j_data[i] = getJSArrayItem(ctx, temp, i);
             }

             jlongArray j_arr =env->NewLongArray(size);
             env->SetLongArrayRegion(j_arr, 0, size, j_data);
             params[i].l = j_arr;
         } else if (type == charArrType) {
             int size = getJsArraySize(ctx, temp);
             jchar j_data[size];
             for (int i = 0; i < size; i++) {
                 j_data[i] = getJSArrayItem(ctx, temp, i);
             }

             jcharArray j_arr =env->NewCharArray(size);
             env->SetCharArrayRegion(j_arr, 0, size, j_data);
             params[i].l = j_arr;
         } else if (type == shortArrType) {
             int size = getJsArraySize(ctx, temp);
             jshort j_data[size];
             for (int i = 0; i < size; i++) {
                 j_data[i] = getJSArrayItem(ctx, temp, i);
             }

             jshortArray j_arr =env->NewShortArray(size);
             env->SetShortArrayRegion(j_arr, 0, size, j_data);
             params[i].l = j_arr;
         } else if (type == floatArrType) {
             int size = getJsArraySize(ctx, temp);
             jfloat j_data[size];
             for (int i = 0; i < size; i++) {
                 j_data[i] = getJSArrayItem(ctx, temp, i);
             }

             jfloatArray j_arr =env->NewFloatArray(size);
             env->SetFloatArrayRegion(j_arr, 0, size, j_data);
             params[i].l = j_arr;
         } else if (type == doubleArrType) {
             int size = getJsArraySize(ctx, temp);
             jdouble j_data[size];
             for (int i = 0; i < size; i++) {
                 j_data[i] = getJSArrayItem(ctx, temp, i);
             }

             jdoubleArray j_arr =env->NewDoubleArray(size);
             env->SetDoubleArrayRegion(j_arr, 0, size, j_data);
             params[i].l = j_arr;
         } else if (type == byteArrType) {
             int size = getJsArraySize(ctx, temp);
             jbyte j_data[size];
             for (int i = 0; i < size; i++) {
                 j_data[i] = getJSArrayItem(ctx, temp, i);
             }

             jbyteArray j_arr =env->NewByteArray(size);
             env->SetByteArrayRegion(j_arr, 0, size, j_data);
             params[i].l = j_arr;
         }
    }
}

int getJsArraySize(JSContextRef ctx, JSValueRef array) {
        JSStringRef objName=JSStringCreateWithUTF8CString("length");
        JSValueRef obj = JSObjectGetProperty(ctx, (JSObjectRef) array, objName, NULL);
        JSStringRelease(objName);

        JSValueRef exception;
        return JSValueToNumber(ctx, obj, &exception);
    }

double getJSArrayItem(JSContextRef ctx, JSValueRef array, int index) {
    char str[10];
    sprintf(str, "%d", index);
    JSStringRef objName=JSStringCreateWithUTF8CString(str);
    JSValueRef obj = JSObjectGetProperty(ctx, (JSObjectRef) array, objName, NULL);
    JSStringRelease(objName);

    JSValueRef exception;
    return JSValueToNumber(ctx, obj, &exception);
}

void getJSArrayStringItem(JSContextRef ctx, JSValueRef array, int index, char* v) {
    char str[10];
    sprintf(str, "%d", index);
    JSStringRef objName=JSStringCreateWithUTF8CString(str);
    JSValueRef obj = JSObjectGetProperty(ctx, (JSObjectRef) array, objName, NULL);
    JSStringRelease(objName);

    JSValueRef exception;
    JSStringRef result = JSValueToStringCopy(ctx, obj, &exception);
    JSStringGetUTF8CString(result, v, 1024);
    JSStringRelease(result);
}