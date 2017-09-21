#ifndef __DATAMAP_H__
#define __DATAMAP_H__

#include <jni.h>
#include "log.h"
#include "stdio.h"
#include <string.h>
#include <JavaScriptCore/JavaScript.h>

int findType(char** p);
double getJSArrayItem(JSContextRef ctx, JSValueRef array, int index);
void getJSArrayStringItem(JSContextRef ctx, JSValueRef array, int index, char* p);
int getJsArraySize(JSContextRef ctx, JSValueRef array);
void makeJsParam(JNIEnv *env, jobject map, JSContextRef, JSValueRef[]);
void makeJniParam(JNIEnv *env, JSContextRef ctx, jvalue data[], const JSValueRef args[], int count, const char* sign);

#endif