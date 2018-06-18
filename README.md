this is a lightweight js bridge for android. simplely implements the following funtions:
1. call javascript from android application with java code.
2. call android modules in android application with javascript code.
3. also, jsbridge support callback from android to javascript.

 一个轻量级的jsbridge, 实现了从javascript到c++再到android(java)之间的调用，同时也可以反过来调用。核心库使用javascriptcore,bridge的主要方法：JSObjectCallAsFunction，JSObjectSetProperty，JSClassDefinition,JSObjectGetProperty.

**how to start:**

## Step 1.

Add it in your root build.gradle at the end of repositories:

    allprojects {
    	repositories {
    		...
    		maven { url 'https://jitpack.io' }
    	}
    }

## Step 2.

Add the dependency

    dependencies {
            implementation 'com.github.funny9527:SimpleJsBridge:1.0'
    }

## Step 3.

load javascript code and register native module

1. implement native android modules with extends com.szy.bridge.javamodule.NativeModule;
2. mark methods you want to call in javascript with annotation com.szy.bridge.javamodule.NativeMethod;
```java
    public class TestNativeModule extends NativeModule {
    public static final String TAG = "react";

    @Override
    public String getName() {
        return "TestNativeModule";
    }

    @NativeMethod
    public void setData(String value, int index) {
    }
    }
```
3. define javascript module as interface while extends com.szy.bridge.jsmodule.JsModule;
```java
    public interface TestJsModule extends JsModule {
    public void function1();
    public String function2(int a, String b);
    public String test(String a, int b, byte c, boolean d, byte[] arr);
    }
```
4. implement javascript module in javascript code while add object in global.BridgeBatch, make sure global is the root object    of javascript bundle
```javascript
    var global = {
    main : function(a, b) {
        return 'js function callback <main> called ' + a + '  ' + b;
    },
    
    BridgeBatch: {
         TestJsModule : {
             function2 : function(p1, p2) {
                         var a = "js function <function2> called " + p1 + " " + p2;
                         return a;
                     },
             function1 : function() {
                         var a = "js function <function1> called";
                         return a;
                     },
             test : function(p1, p2, p3, p4, p5) {
                     var res = global.NativeModules.TestNativeModule.getData(p1, p2, p3, p4, p5, global.main);
                     return res;
                 },
         },
    }
    }
```
5. you can load javascript code from wherever from network or local, and finally load with:
```java
     /**
     * load javascript code
     * @param context
     * @param js javascript code
     * @param list android native modules
     */
      JsExecutor.getInstance().init(Context context, String js, List<NativeModule> list);
```
6. call javascript from android:
```java
    JsExecutor.getInstance().getJsModule(Class<T> jsInterface).javacript_function
    
    JsExecutor.getInstance().getJsModule(TestJsModule.class).function2(1000, "test java call js");
```
7. call android module from javascript:
```java
   global.NativeModules.android_native_modules.native_function  
   
   global.NativeModules.TestNativeModule.getData(p1, p2, p3, p4, p5, global.main);
```
