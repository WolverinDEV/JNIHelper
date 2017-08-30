#pragma once

#include <jni.h>
#include "ClassDefinitions.h"

namespace JNIHelper {
    extern void deleteEnvKey(void *keyValuePtr);
    extern void initializeLibrary(JavaVM *);
    extern JavaVM* getJVM();
    extern JNIEnv* getAttachedEnv();
    extern JNIEnv* getCurrentEnv();

    extern void finalizeLibrary(JavaVM *);
}