#include <string>
#include <jni.h>
#include <cstring>
#include <memory>
#include "../include/Helper.h"

using namespace std;

namespace JNIHelper {
    namespace DefaultConverterImpl {
        jstring convertString(const std::string& str){
            auto env = JNIHelper::getAttachedEnv();
            auto jstr = env->NewString((jchar*) str.data(), (int) str.length());
            return jstr;
        }

        std::string convertString(jstring jstr){
            auto env = JNIHelper::getAttachedEnv();
            const char* charBuffer = env->GetStringUTFChars(jstr, nullptr);
            string str(charBuffer, (size_t) env->GetStringUTFLength(jstr));
            env->ReleaseStringUTFChars(jstr, charBuffer);
            return str;
        }

        const char* convertCString(jstring jstr){
            auto env = JNIHelper::getAttachedEnv();

            auto strLength = (size_t) env->GetStringUTFLength(jstr);
            char* buffer = (char*) malloc(strLength + 1);
            const char* jbuffer = env->GetStringUTFChars(jstr, nullptr);

            memcpy(buffer, jbuffer, strLength);

            env->ReleaseStringUTFChars(jstr, jbuffer);

            buffer[strLength] = '\0';
            return buffer;
        }

        jobject convertJObject(JavaObject* obj){
            return Helper::getJavaObject(obj);
        }

        std::shared_ptr<void*> createJavaObjectPtr(jobject object){
            auto env = JNIHelper::getAttachedEnv();
            env->NewGlobalRef(object);

            return shared_ptr<void *>((void**) object, [](void *obj) {
                if (!obj) return;
                auto local_env = JNIHelper::getAttachedEnv();
                local_env->DeleteGlobalRef((jobject) obj);
            });
        }
    }
}