package dev.wolveringer;

public class TestClass {
    TestClass() {
        System.out.println("[TestClass] Constructor called!");
    }

    @Override
    protected void finalize() throws Throwable {
        System.out.println("[TestClass] Destructor called!");
    }

    public int mul(int a, int b) {
        return a  * b;
    }

    public static void staticCall(int number) {
        System.out.println("[TestClass] staticCall(" + number + ")");
    }

    public void instanceCall(int number) {
        System.out.println("[TestClass] instanceCall(" + number + ")");
    }

    public Object createObject() {
        return "Hello world";
    }

    public static native void staticCallNative();
    public static native void staticCallNative2(int _);
    public static native int staticCallNative3();
    public static native void staticCallNativeString(String _);
}
