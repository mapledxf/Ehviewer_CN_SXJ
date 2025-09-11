package com.vwm.encryption;

public class Encryption {
    static {
        System.loadLibrary("encryption-jni");
    }

    public static native String Encrypt(String input);

    public static native String Decrypt(String input);

}
