package com.szy.simple.test;

import android.util.Log;

import com.szy.simple.javamodule.NativeModule;
import com.szy.simple.javamodule.NativeMethod;

/**
 * Created by szy on 17/1/6.
 */
public class Test1NativeModule extends NativeModule {
    public static final String TAG = "react";

    @Override
    public String getName() {
        return "Test1NativeModule";
    }

    @NativeMethod
    public String fun1() {
        Log.v(TAG, "fun1() called");
        return "fun1 from Test1NativeModule";
    }

    @NativeMethod
    public String fun3(int value) {
        Log.v(TAG, "fun1() called");
        return "fun3 from Test1NativeModule";
    }

    @NativeMethod
    public void fun2(String value, int index) {
        Log.v(TAG, "fun1() called " + value + "  " + index);
    }

    @NativeMethod
    public void fun4(String value, String index) {
        Log.v(TAG, "fun4() called " + value + "  " + index);
    }
}
