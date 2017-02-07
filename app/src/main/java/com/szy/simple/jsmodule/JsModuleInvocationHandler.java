package com.szy.simple.jsmodule;


import com.szy.simple.bridge.JsExecutor;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

/**
 * Created by szy on 17/1/13.
 */
public class JsModuleInvocationHandler implements InvocationHandler {
    private JsExecutor mProxy;
    private String name;

    public JsModuleInvocationHandler(JsExecutor p, String name) {
        mProxy = p;
        this.name = name;
    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        return mProxy.callFunction(this.name, method.getName(), args);
    }
}
