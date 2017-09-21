package com.szy.bridge.bridge;


import com.szy.bridge.javamodule.NativeModule;

/**
 * Created by szy on 16/12/22.
 */
public class Bridge {
    public native int loadJs(String text);
    public native String execute(String module, String method, DataMap map, int size);
    public native void inject();
    public native String executeCallback(int id, DataMap map, int size);

    public native void setNativeModules(NativeModule module, String moduleName, String[] methods, String[] types);

    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("bridge");
        System.loadLibrary("jsc");
    }
}
