package com.szy.simple.test;

import android.util.Log;

import com.szy.simple.javamodule.Callback;
import com.szy.simple.javamodule.NativeModule;
import com.szy.simple.javamodule.NativeMethod;

/**
 * Created by szy on 17/1/5.
 */
public class TestNativeModule extends NativeModule {
    public static final String TAG = "react";

    @Override
    public String getName() {
        return "TestNativeModule";
    }

    @NativeMethod
    public int getData(String name, int a, byte b, boolean c, char[] arr, Callback callback) {
        Log.v(TAG, "getData() called " + arr.length);
        for (int i = 0; i < arr.length; i++) {
            Log.v(TAG, "arr = " + arr[i]);
        }
        String callRes = callback.invoke(123456789, "abcdefg");
        Log.v(TAG, " callback= " + callRes);
//        return "java " + name + " " + a + " " + b + " " + c;
        return 1000;
    }

    @NativeMethod
    public void setData(String value, int index) {
        Log.v(TAG, "setData() called " + value + "  " + index);
    }
}
