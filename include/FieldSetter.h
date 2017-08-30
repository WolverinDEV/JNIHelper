#pragma once

#include <jni.h>

namespace JNIHelper {
    namespace FieldHelper {
        template <typename FieldType>
        struct FieldActor {
            public:
                static bool supported(){ return false; }
                static void setStatic(JNIEnv* env, jclass clazz, jfieldID fieldID, FieldType value);
                static FieldType getStatic(JNIEnv* env, jclass clazz, jfieldID fieldID);
                static void setInstance(JNIEnv* env,jobject obj, jfieldID fieldID,FieldType value);
                static FieldType getInstance(JNIEnv* env, jobject obj, jfieldID fieldID);
        };

        #define DEFINDE_JNI_FIELD_ACTOR(type, methodeTypeName) \
        template <> \
        struct FieldActor<type> { \
                using FieldType = type; \
            public: \
                static bool supported(){ return true; } \
                static void setStatic(JNIEnv* env, jclass clazz, jfieldID fieldID, FieldType value){ env->SetStatic ##methodeTypeName ##Field(clazz, fieldID, value); } \
                static FieldType getStatic(JNIEnv* env, jclass clazz, jfieldID fieldID){ return env->GetStatic ##methodeTypeName ##Field(clazz, fieldID); } \
                static void setInstance(JNIEnv* env,jobject obj, jfieldID fieldID,FieldType value){ env->Set ##methodeTypeName ##Field(obj, fieldID, value); } \
                static FieldType getInstance(JNIEnv* env, jobject obj, jfieldID fieldID){ return env->Get ##methodeTypeName ##Field(obj, fieldID); } \
        }

        /*
Boolean
Byte
Char
SHort
Int
Long
Float
Double
         */
        DEFINDE_JNI_FIELD_ACTOR(jboolean, Boolean);
        DEFINDE_JNI_FIELD_ACTOR(jbyte, Byte);
        DEFINDE_JNI_FIELD_ACTOR(jchar, Char);
        DEFINDE_JNI_FIELD_ACTOR(jshort, Short);
        DEFINDE_JNI_FIELD_ACTOR(jint, Int);
        DEFINDE_JNI_FIELD_ACTOR(jlong, Long);
        DEFINDE_JNI_FIELD_ACTOR(jfloat, Float);
        DEFINDE_JNI_FIELD_ACTOR(jdouble, Double);


        //DEFINDE_JNI_FIELD_ACTOR(jarray, Object);

        template <>
        struct FieldActor<jarray> {
            public:
                static bool supported(){ return true; }
                static void setStatic(JNIEnv* env, jclass clazz, jfieldID fieldID, jarray value){
                    env->SetStaticObjectField(clazz, fieldID, value);
                }
                static jarray getStatic(JNIEnv* env, jclass clazz, jfieldID fieldID){
                    return (jarray) env->GetStaticObjectField(clazz, fieldID);
                }
                static void setInstance(JNIEnv* env,jobject obj, jfieldID fieldID,jarray value){
                    env->SetObjectField(obj, fieldID, value);
                }
                static jarray getInstance(JNIEnv* env, jobject obj, jfieldID fieldID){
                    return (jarray) env->GetObjectField(obj, fieldID);
                }
        };
    }
}