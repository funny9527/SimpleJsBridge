package com.szy.bridge.javamodule;


import com.szy.bridge.bridge.JsExecutor;

/**
 * Created by szy on 17/1/20.
 */
public class Callback {
    private int mObject = 0;
    public Callback(int id) {
        mObject = id;
    }

    public String invoke(Object ...params) {
        Object[] arr = params;
        return JsExecutor.getInstance().runCallback(mObject, arr);
    }
}
