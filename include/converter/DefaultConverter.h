#pragma once

#if !defined(JNI_VALUE_CONVERTER) || !defined(DEFINE_PASSTHRU_CPP_JNI_CONVERTER) || !defined(DEFINE_CPP_JNI_CONVERTER)
#error Please not import this header directly!
#define JNI_VALUE_CONVERTER(...)
#define DEFINE_PASSTHRU_CPP_JNI_CONVERTER(...)
#define DEFINE_CPP_JNI_CONVERTER(...)
#endif

#include <string>
#include <memory>
#include <vector>

namespace JNIHelper {
    //Default jni value conversation
    JNI_VALUE_CONVERTER(jboolean);
    JNI_VALUE_CONVERTER(jbyte);
    JNI_VALUE_CONVERTER(jchar);
    JNI_VALUE_CONVERTER(jshort);
    JNI_VALUE_CONVERTER(jint);
    JNI_VALUE_CONVERTER(jlong);
    JNI_VALUE_CONVERTER(jfloat);
    JNI_VALUE_CONVERTER(jdouble);
    JNI_VALUE_CONVERTER(jobject);
    JNI_VALUE_CONVERTER(jclass);
    JNI_VALUE_CONVERTER(jstring);
    JNI_VALUE_CONVERTER(jthrowable);
    JNI_VALUE_CONVERTER(jarray);
    JNI_VALUE_CONVERTER(jbooleanArray);
    JNI_VALUE_CONVERTER(jbyteArray);
    JNI_VALUE_CONVERTER(jcharArray);
    JNI_VALUE_CONVERTER(jshortArray);
    JNI_VALUE_CONVERTER(jintArray);
    JNI_VALUE_CONVERTER(jlongArray);
    JNI_VALUE_CONVERTER(jfloatArray);
    JNI_VALUE_CONVERTER(jdoubleArray);
    JNI_VALUE_CONVERTER(jobjectArray);

    //DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jboolean); -> Defined over byte
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jboolean, bool);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jbyte);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jbyte, uint8_t);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jbyte, int8_t);

    //DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jchar); -> Defined over short
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jshort);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jshort, uint16_t);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jshort, int16_t);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jint);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jint, uint32_t);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jint, int32_t);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jlong);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jlong, uint64_t);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jlong, int64_t);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jfloat);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jfloat, float);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jdouble);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jdouble, double);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jobject);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jclass);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jstring);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jthrowable);

    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jarray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jarray, jarray);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jbooleanArray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jbooleanArray, jbooleanArray);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jbyteArray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jbyteArray, jbyteArray);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jcharArray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jcharArray, jcharArray);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jshortArray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jshortArray, jshortArray);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jintArray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jintArray, jintArray);
    DEFINE_JNI_CPP_CONVERTER(jintArray, std::vector<jint>, return std::vector<jint>());

    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jlongArray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jlongArray, jlongArray);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jfloatArray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jfloatArray, jfloatArray);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jdoubleArray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jdoubleArray, jdoubleArray);
    DEFINE_PASSTHRU_CPP_JNI_CONVERTER(jobjectArray);
    DEFINE_PASSTHRU_JNI_CPP_CONVERTER(jobjectArray, jobjectArray);

    DEFINE_CPP_JNI_CONVERTER(bool, jboolean, return (jboolean) src);
    DEFINE_CPP_JNI_CONVERTER(uint8_t, jbyte, return (jbyte) src);
    DEFINE_CPP_JNI_CONVERTER(char, jbyte, return (jbyte) src);
    DEFINE_CPP_JNI_CONVERTER(uint16_t, jshort, return (jshort) src);
    DEFINE_CPP_JNI_CONVERTER(uint32_t, jint, return (jint) src);
    DEFINE_CPP_JNI_CONVERTER(uint64_t, jlong, return (jlong) src);
    DEFINE_CPP_JNI_CONVERTER(void*, jlong, return (jlong) src);

    DEFINE_JNI_CPP_CONVERTER(void*, void, );
    DEFINE_TYPE_CONVERTER(jvalue, void*, );

    class JavaObject;
    namespace DefaultConverterImpl {
        extern jstring convertString(const std::string);

        extern std::string convertString(jstring);

        extern const char *convertCString(jstring);

        extern jobject convertJObject(JavaObject *);

        extern std::shared_ptr<void *> createJavaObjectPtr(jobject);
    }

    DEFINE_CPP_JNI_CONVERTER(std::string, jstring, return DefaultConverterImpl::convertString(src));
    DEFINE_JNI_CPP_CONVERTER(jstring, std::string, return DefaultConverterImpl::convertString(src));

    DEFINE_CPP_JNI_CONVERTER(const char*, jstring, return DefaultConverterImpl::convertString(src));
    DEFINE_JNI_CPP_CONVERTER(jstring, const char*, return DefaultConverterImpl::convertCString(src));
}