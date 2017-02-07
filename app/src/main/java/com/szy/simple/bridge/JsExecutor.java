package com.szy.simple.bridge;

import android.content.Context;
import android.util.Log;

import com.szy.simple.javamodule.NativeModule;
import com.szy.simple.javamodule.Utils;
import com.szy.simple.jsmodule.JsModule;
import com.szy.simple.jsmodule.JsModuleInvocationHandler;

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

    private String getBoudle(Context c) {
        InputStream in = null;
        try {
            in = c.getResources().getAssets().open("boudle.js");
            int length = in.available();
            byte[]  buffer = new byte[length];
            in.read(buffer);
            String str = new String(buffer, "utf-8");

            return str;
        } catch (Exception e) {
            Log.d(TAG, "get js error " + e);
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return null;
    }

    public void init(Context context) {
        mBridge = new Bridge();
        mBridge.loadJs(getBoudle(context));
        mBridge.inject();
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

    public void createNativeModules(ArrayList<NativeModule> list) {
        for (NativeModule m : list) {
            addModules(m);
        }
    }

    public synchronized <T extends JsModule> T getJsModule(Class<T> jsInterface) {
        JsExecutor executor = JsExecutor.getInstance();
        InvocationHandler handler = new JsModuleInvocationHandler(executor, jsInterface.getSimpleName());
        JsModule obj = (JsModule) Proxy.newProxyInstance(jsInterface.getClassLoader(),
                new Class[] {jsInterface}, handler);

        return (T) obj;
    }
}
