#pragma once

#include <memory>
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


        extern std::unique_ptr<void*> unique_null;

        template <typename T>
        inline const std::unique_ptr<T>& unique_null_t() {
            return *(std::unique_ptr<T>*) &unique_null;
        }
    }

    extern JNIEnv* getAttachedEnv();

};