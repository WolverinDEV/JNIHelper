package dev.wolveringer;

public class MainClass {
    public static void main(String... args) {
        System.out.println("main(...)");
        System.load("/home/wolverindev/git/JNIHelper/cmake-build-debug/libJavaJNI.so");
        System.runFinalizersOnExit(true);
        TestClass.staticCallNative();
        TestClass.staticCallNative2(2);
        System.out.println("Return: " + TestClass.staticCallNative3());
    }
}
