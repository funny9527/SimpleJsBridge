package com.szy.simple.test;

import com.szy.simple.jsmodule.JsModule;

/**
 * Created by szy on 17/1/13.
 */
public interface TestJsModule extends JsModule {
    public void function1();
    public String function2(int a, String b);
    public String test(String a, int b, byte c, boolean d, byte[] arr);
}
