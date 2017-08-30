#pragma once

#include <jni.h>
#include <pthread.h>
#include <string>

namespace JNIHelper {
    class JavaObject;
    class JavaClass;
    namespace Helper {
        jobject getJavaObject(const JavaObject*);
        jclass getJavaClass(JavaClass*);
        std::string getJavaClassSignature(JavaClass*);
    }

    extern JNIEnv* getAttachedEnv();
};