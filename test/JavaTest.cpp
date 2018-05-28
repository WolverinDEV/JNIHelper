#include <jni.h>
#include <iostream>
#include "../include/converter/TypeConverter.h"
#include "../include/signature/Signature.h"
#include "../include/ClassDefinitions.h"
#include "../include/JNIHelper.h"

DEFINE_JAVA_CLASS(java_util, List, JNIHelper::JavaObject,
            DEFINE_DEFAULT_CONSTRUCTOR(init);

            DEFINE_METHODE(size, int);
            DEFINE_METHODE(isEmpty, bool);
            DEFINE_METHODE(add, bool, jobject);
            DEFINE_METHODE(add, bool, JNIHelper::JavaObject*);
            DEFINE_METHODE(add, bool, JNIHelper::JavaObject&);
);

DEFINE_JAVA_CLASS(java_util, ArrayList, java_util::List,

);

DEFINE_JAVA_CLASS(dev_wolveringer, TestClass, JNIHelper::JavaObject,
              DEFINE_DEFAULT_CONSTRUCTOR(init);
              DEFINE_METHODE(mul, int, int, int);

              DEFINE_STATIC_METHODE(staticCall, void, int);
              DEFINE_METHODE(instanceCall, void, int);
              DEFINE_METHODE(createObject, JNIHelper::JavaObject);
);

using namespace std;

extern "C" {
    jint JNI_OnLoad(JavaVM *vm, void *reserved) {
        printf("initialising EasyJNITest libary!\n");
        JNIHelper::Debug::setDebugMask(JNIHelper::Debug::Type::NONE);
        JNIHelper::initializeLibrary(vm);

        dev_wolveringer::TestClass::staticCall(1);
        dev_wolveringer::TestClass instance;
        instance.init();
        instance.instanceCall(2);

        auto object = instance.createObject();
        printf("Object class: %s\n", object.getObjectClass()->name.c_str());
        printf("Mul 2 * 2 = %d\n", instance.mul(2, 2));
        return JNI_VERSION_1_6;
    }

    void JNI_OnUnload(JavaVM *vm, void *reserved){
        JNIHelper::finalizeLibrary(vm);
    }
}