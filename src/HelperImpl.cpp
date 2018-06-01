#include "../include/Helper.h"
#include "../include/ClassDefinitions.h"
#include "../include/JNIHelper.h"

std::unique_ptr<void*> JNIHelper::Helper::unique_null{nullptr};
namespace JNIHelper {
    class JavaObject;
    namespace Helper {

        jobject getJavaObject(const JavaObject* obj){ return obj->getJavaObject(); }

        jclass getJavaClass(JavaClass* klass){
            return klass->getJavaClass();
        }

        std::string getJavaClassSignature(JavaClass* obj){
            return obj->getJavaSignature();
        }
    }
};