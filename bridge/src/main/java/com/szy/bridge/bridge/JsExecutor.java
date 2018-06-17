package com.szy.bridge.bridge;

import android.content.Context;
import android.util.Log;

import com.szy.bridge.javamodule.NativeModule;
import com.szy.bridge.javamodule.Utils;
import com.szy.bridge.jsmodule.JsModule;
import com.szy.bridge.jsmodule.JsModuleInvocationHandler;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Proxy;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by szy on 17/1/13.
 */
public class JsExecutor {
    private static final String TAG = "test";
    private Bridge mBridge;
    private static JsExecutor mInstance;

    private JsExecutor() {
    }

    public static synchronized JsExecutor getInstance() {
        if (mInstance == null) {
            mInstance = new JsExecutor();
        }

        return mInstance;
    }

    /**
     * load javascript code
     * @param context
     * @param js javascript code
     * @param list android native modules
     */
    public void init(Context context, String js, List<NativeModule> list) {
        mBridge = new Bridge();
        mBridge.loadJs(js);
        mBridge.inject();

        for (NativeModule m : list) {
            addModules(m);
        }
    }

    public String callFunction(String name, String method, Object ...args) {
        return mBridge.execute(name, method, DataMap.buildData(args), args.length);
    }

    public String runCallback(int id, Object ...args) {
        return mBridge.executeCallback(id, DataMap.buildData(args), args.length);
    }

    private void addModules(NativeModule module) {
        List<String> names = new ArrayList<String>();
        List<String> types = new ArrayList<String>();
        Utils.getSignature(module, names, types);

        String[] namesArr = new String[names.size()];
        names.toArray(namesArr);

        String[] typesArr = new String[types.size()];
        types.toArray(typesArr);
        mBridge.setNativeModules(module, module.getName(), namesArr, typesArr);
    }

    /**
     * get javascript module
     * @param jsInterface
     * @param <T>
     * @return
     */
    public synchronized <T extends JsModule> T getJsModule(Class<T> jsInterface) {
        JsExecutor executor = JsExecutor.getInstance();
        InvocationHandler handler = new JsModuleInvocationHandler(executor, jsInterface.getSimpleName());
        JsModule obj = (JsModule) Proxy.newProxyInstance(jsInterface.getClassLoader(),
                new Class[] {jsInterface}, handler);

        return (T) obj;
    }
}
