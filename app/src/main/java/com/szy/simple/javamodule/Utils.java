package com.szy.simple.javamodule;

import java.lang.reflect.Method;
import java.util.List;

/**
 * Created by szy on 17/1/5.
 */
public class Utils {
    public static String getTypeChar(Class c) {
        if (c == void.class) {
            return "V";
        } else if (c == int.class) {
            return "I";
        } else if (c == String.class) {
            return "Ljava/lang/String;";
        } else if (c == Callback.class) {
            return "Lcom/szy/simple/javamodule/Callback;";
        } else if (c == long.class) {
            return  "J";
        } else if (c == boolean.class) {
            return "Z";
        } else if (c == byte.class) {
            return "B";
        } else if (c == char.class) {
            return "C";
        } else if (c == short.class) {
            return "S";
        } else if (c == float.class) {
            return "F";
        } else if (c == double.class) {
            return "D";
        } else if (c == int[].class) {
            return "[I";
        } else if (c == String[].class) {
            return "[Ljava/lang/String;";
        } else if (c == boolean[].class) {
            return "[Z";
        } else if (c == long[].class) {
            return "[J";
        } else if (c == char[].class) {
            return "[C";
        } else if (c == short[].class) {
            return "[S";
        } else if (c == float[].class) {
            return "[F";
        } else if (c == double[].class) {
            return "[D";
        } else if (c == byte[].class) {
            return "[B";
        }

        return "";
    }

    public static void getSignature(NativeModule module, List<String> methods, List<String> types) {
        Method[] arr = module.getClass().getDeclaredMethods();

        for (Method m : arr) {
            String name = m.getName();

            if (m.getAnnotation(NativeMethod.class) != null) {
                methods.add(name);
                Class[] params = m.getParameterTypes();

                StringBuilder sb = new StringBuilder();
                sb.append("(");
                for (Class c : params) {
                    sb.append(Utils.getTypeChar(c));
                }
                sb.append(")")
                        .append(Utils.getTypeChar(m.getReturnType()));
                types.add(sb.toString());
            }
        }
    }
}
