#include "../include/JNIHelper.h"
#include "../include/exceptions.h"
#include "../include/Helper.h"
#include "../include/ClassDefinitions.h"
#include <cassert>

namespace JNIHelper {
    JavaVM* jvmPtr = nullptr;
    JNIEnv* mainEnvPtr = nullptr;
    pthread_key_t JNIEnvKey = 0;

    void deleteEnvKey(void *keyValuePtr) {
        pthread_setspecific(JNIEnvKey, nullptr);
        if(!jvmPtr) return; //JVM destroyed
        if(mainEnvPtr == keyValuePtr) return;
        jvmPtr->DetachCurrentThread();
    }

    void initializeLibrary(JavaVM *jvm){
        assert(jvm);
        assert(!jvmPtr);
        jvmPtr = jvm;

        pthread_key_create(&JNIEnvKey, JNIHelper::deleteEnvKey);

        JNIEnv* env;
        if (jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)  throw Exceptions::Exception("Cant get main thread env");
        pthread_setspecific(JNIEnvKey, mainEnvPtr = env);

        JavaObject::getClass()->resolveClassInfo();
    }
    void finalizeLibrary(JavaVM *jvm){
        assert(jvm);
        jvmPtr = nullptr;
        mainEnvPtr = nullptr;
    }

    JavaVM* getJVM(){
        if(!jvmPtr) throw Exceptions::LibraryNotInitializedException("JNIHelper isn't initialized yet. (initialize with initializeLibrary(JavaVM*))");
        return jvmPtr;
    }

    JNIEnv* getAttachedEnv(){
        auto jvm = JNIHelper::getJVM();
        if(!jvm) return nullptr;

        JNIEnv* env = nullptr;
        if((env = (JNIEnv*) pthread_getspecific(JNIEnvKey)) == nullptr){
            int status = jvm->AttachCurrentThread((void**) &env, nullptr);
            if (status < 0) throw Exceptions::Exception("Could not attach the JNI environment to the current thread.");
            if(pthread_setspecific(JNIEnvKey, env) < 0) throw Exceptions::Exception("Could not set the thread specific JNIEnv ptr.");
        }
        return env;
    }

    JNIEnv* getCurrentEnv(){
        return (JNIEnv*) pthread_getspecific(JNIEnvKey); //Could be nullptr
    }
}