#include <string>
#include <jni.h>
#include <cstring>
#include <memory>
#include "../include/Helper.h"

using namespace std;

namespace JNIHelper {
    namespace DefaultConverterImpl {
        jstring convertString(std::string str){
            auto env = JNIHelper::getAttachedEnv();
            jchar* buffer = new jchar[str.length()];

            int index = 0;
            for(auto _char : str)
                buffer[index++] = (jchar) _char;

            auto jstr = env->NewString(buffer, (int) str.length());

            delete buffer;

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
                auto env = JNIHelper::getAttachedEnv();
                env->DeleteGlobalRef((jobject) obj);
            });
        }
    }
}