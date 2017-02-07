package com.szy.simple;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import com.szy.simple.bridge.JsExecutor;
import com.szy.simple.javamodule.NativeModule;
import com.szy.simple.test.Test1NativeModule;
import com.szy.simple.test.TestJsModule;
import com.szy.simple.test.TestNativeModule;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private static final String TAG = "react";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.btn1).setOnClickListener(this);
        findViewById(R.id.btn2).setOnClickListener(this);
        init();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn1:
                String value1 = JsExecutor.getInstance().getJsModule(TestJsModule.class).function2(1000, "test java call js");
                Log.v(TAG, value1);
                break;
            case R.id.btn2:
                byte vvv = 127;
                String value2 = JsExecutor.getInstance().getJsModule(TestJsModule.class).test("00000", 5, vvv, true, new byte[] {'a', 'c', 'f', 'g', 'k'});
                Log.v(TAG, "result = " + value2);
                break;
        }
    }

    /**
     * init js code to native, and init native modules
     */
    public void init() {
        JsExecutor.getInstance().init(this);
        testNativeModules();
    }

    /**
     * test init native modules
     */
    public void testNativeModules() {
        NativeModule module1 = new TestNativeModule();
        NativeModule module2 = new Test1NativeModule();
        ArrayList<NativeModule> list = new ArrayList<>(2);
        list.add(module1);
        list.add(module2);
        JsExecutor.getInstance().createNativeModules(list);
    }
}
