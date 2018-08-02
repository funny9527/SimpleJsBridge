package com.szy.bridge.bridge;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by szy on 17/1/13.
 */
public class DataMap {
    private static final String TAG = "react";
    public static final int intType = 0;
    public static final int stringType = 1;
    public static final int boolType = 2;
    public static final int longType = 3;
    public static final int charType = 4;
    public static final int shortType = 5;
    public static final int floatType = 6;
    public static final int doubleType = 7;
    public static final int byteType = 8;
    public static final int callbackType = 9;

    public static final int intArrType = 10;
    public static final int stringArrType = 11;
    public static final int boolArrType = 12;
    public static final int longArrType = 13;
    public static final int charArrType = 14;
    public static final int shortArrType = 15;
    public static final int floatArrType = 16;
    public static final int doubleArrType = 17;
    public static final int byteArrType = 18;
    public static final int callbackArrType = 19;

    private List<Integer> types = new ArrayList<Integer>();
    private List<Object> values = new ArrayList<Object>();

    public void putString(String v) {
        types.add(stringType);
        values.add(v);
    }

    public void putInt(int v) {
        types.add(intType);
        values.add(v);
    }

    public void putLong(long v) {
        types.add(longType);
        values.add(v);
    }

    public void putByte(byte v) {
        types.add(byteType);
        values.add(v);
    }

    public void putChar(char v) {
        types.add(charType);
        values.add(v);
    }

    public void putShort(short v) {
        types.add(shortType);
        values.add(v);
    }

    public void putFloat(float v) {
        types.add(floatType);
        values.add(v);
    }

    public void putDouble(double v) {
        types.add(doubleType);
        values.add(v);
    }

    public void putBoolean(boolean v) {
        types.add(boolType);
        values.add(v);
    }

    public int getSize() {
        return types == null ? 0 : types.size();
    }

    public int getType(int index) {
        return types.get(index);
    }

    public String getStringValue(int index) {
        return (String) values.get(index);
    }

    public int getIntValue(int index) {
        return (int) values.get(index);
    }

    public long getLongValue(int index) {
        return (long) values.get(index);
    }

    public boolean getBooleanValue(int index) {
        boolean b = (boolean) values.get(index);
        return b;
    }

    public byte getByteValue(int index) {
        return (byte) values.get(index);
    }

    public char getCharValue(int index) {
        return (char) values.get(index);
    }

    public short getShortValue(int index) {
        return (short) values.get(index);
    }

    public float getFloatValue(int index) {
        return (float) values.get(index);
    }

    public double getDoubleValue(int index) {
        return (double) values.get(index);
    }

    public void putIntArray(int[] v) {
        types.add(intArrType);
        values.add(v);
    }

    public int[] getIntArray(int index) {
        return (int[]) values.get(index);
    }

    public void putLongArray(long[] v) {
        types.add(longArrType);
        values.add(v);
    }

    public long[] getLongArray(int index) {
        return (long[]) values.get(index);
    }

    public void putByteArray(byte[] v) {
        types.add(byteArrType);
        values.add(v);
    }

    public byte[] getByteArray(int index) {
        return (byte[]) values.get(index);
    }

    public void putShortArray(short[] v) {
        types.add(shortArrType);
        values.add(v);
    }

    public short[] getShortArray(int index) {
        return (short[]) values.get(index);
    }

    public void putCharArray(char[] v) {
        types.add(charArrType);
        values.add(v);
    }

    public char[] getCharArray(int index) {
        return (char[]) values.get(index);
    }

    public void putFloatArray(float[] v) {
        types.add(floatArrType);
        values.add(v);
    }

    public float[] getFloatArray(int index) {
        return (float[]) values.get(index);
    }

    public void putDoubleArray(double[] v) {
        types.add(doubleArrType);
        values.add(v);
    }

    public double[] getDoubleArray(int index) {
        return (double[]) values.get(index);
    }

    public void putBooleanArray(boolean[] v) {
        types.add(boolArrType);
        values.add(v);
    }

    public boolean[] getBooleanArray(int index) {
        return (boolean[]) values.get(index);
    }

    public void putStringArray(String[] v) {
        types.add(stringArrType);
        values.add(v);
    }

    public String[] getStringArray(int index) {
        return (String[]) values.get(index);
    }

    public static DataMap buildData(Object ...args) {
        DataMap map = new DataMap();
        if (args == null) {
            return map;
        }
        int len = args.length;
        for (int i = 0; i < len; i++) {
            Object temp = args[i];
            Class c = temp.getClass();

            if (c == int.class || c == Integer.class) {
                map.putInt((Integer) temp);
            } else if (c == String.class) {
                map.putString((String) temp);
            } else if (c == long.class || c == Long.class) {
                map.putLong((long) temp);
            } else if (c == boolean.class || c == Boolean.class) {
                map.putBoolean((boolean) temp);
            } else if (c == byte.class || c == Byte.class) {
                map.putByte((byte) temp);
            } else if (c == char.class || c == Character.class) {
                map.putChar((char) temp);
            } else if (c == short.class || c == Short.class) {
                map.putShort((short) temp);
            } else if (c == float.class || c == Float.class) {
                map.putFloat((float) temp);
            } else if (c == double.class || c == Double.class) {
                map.putDouble((double) temp);
            } else if (c == int[].class) {
                map.putIntArray((int[]) temp);
            } else if (c == String[].class) {
                map.putStringArray((String[]) temp);
            } else if (c == boolean[].class) {
                map.putBooleanArray((boolean[]) temp);
            } else if (c == byte[].class) {
                map.putByteArray((byte[]) temp);
            } else if (c == char[].class) {
                map.putCharArray((char[]) temp);
            } else if (c == short[].class) {
                map.putShortArray((short[]) temp);
            } else if (c == long[].class) {
                map.putLongArray((long[]) temp);
            } else if (c == float[].class) {
                map.putFloatArray((float[]) temp);
            } else if (c == double[].class) {
                map.putDoubleArray((double[]) temp);
            }
        }

        return map;
    }
}
