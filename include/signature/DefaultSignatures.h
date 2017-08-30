#pragma once

#ifndef DEFINE_TYPE_SIGNATURE
    #error Dont include this file directly!
    #define DEFAULT_TYPE_SIGNATURE(type, signature)
#endif

#include <jni.h>

namespace JNIHelper {
    namespace Signature {
        DEFINE_TYPE_SIGNATURE(jboolean, "Z");
        DEFINE_TYPE_SIGNATURE(jbyte, "B");
        DEFINE_TYPE_SIGNATURE(jshort, "S");
        DEFINE_TYPE_SIGNATURE(jint, "I");
        DEFINE_TYPE_SIGNATURE(jlong, "J");
        DEFINE_TYPE_SIGNATURE(jfloat, "F");
        DEFINE_TYPE_SIGNATURE(jdouble, "D");
        DEFINE_TYPE_SIGNATURE(void, "V");

        DEFINE_TYPE_SIGNATURE(jbooleanArray, "[Z");
        DEFINE_TYPE_SIGNATURE(jbyteArray, "[B");
        DEFINE_TYPE_SIGNATURE(jshortArray, "[S");
        DEFINE_TYPE_SIGNATURE(jintArray, "[I");
        DEFINE_TYPE_SIGNATURE(jlongArray, "[J");
        DEFINE_TYPE_SIGNATURE(jfloatArray, "[F");
        DEFINE_TYPE_SIGNATURE(jdoubleArray, "[D");
        DEFINE_TYPE_SIGNATURE(jobjectArray, "[Ljava/lang/Object;");

        DEFINE_TYPE_SIGNATURE(jobject, "Ljava/lang/Object;");
        DEFINE_TYPE_SIGNATURE(jstring, "Ljava/lang/String;");
    }
}