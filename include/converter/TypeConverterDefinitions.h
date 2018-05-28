#pragma once

#include <jni.h>
#include <typeinfo>
#include <string>
#include "../exceptions.h"

#define SPAWN_TYPE_CONVERTER_DEFINITION(type, source, target, function) \
template <> \
struct type<source, target> { \
        using TargetType = target; \
        using SourceType = source; \
        static constexpr bool supported() { return true; } \
     \
        static TargetType convert(SourceType src){ \
            function; \
        } \
};

//General conversation
#define DEFINE_TYPE_CONVERTER(source, target, function) SPAWN_TYPE_CONVERTER_DEFINITION(TypeConverter, source, target, function)
#define JNI_VALUE_CONVERTER(jniType)                                \
DEFINE_TYPE_CONVERTER(jniType, jvalue, return *(jvalue*) &src)      \
DEFINE_TYPE_CONVERTER(jvalue, jniType, return *(jniType*) &src)

#define DEFINE_JNI_CPP_CONVERTER(source, target, function) SPAWN_TYPE_CONVERTER_DEFINITION(JniToCppConverter, source, target, function)
#define DEFINE_PASSTHRU_JNI_CPP_CONVERTER(source, target) DEFINE_JNI_CPP_CONVERTER(source, target, return src;)

#define DEFINE_CPP_JNI_CONVERTER(source, target, function) \
template <> \
struct CppToJniConverter<source> { \
        using TargetType = target; \
        using SourceType = source; \
     \
        static TargetType convert(SourceType& src){ \
            function; \
        } \
};
#define DEFINE_PASSTHRU_CPP_JNI_CONVERTER(type) DEFINE_CPP_JNI_CONVERTER(type, type, return src)

namespace JNIHelper {
    template <typename Source, typename Target>
    struct TypeConverter {
        using TargetType = Target;
        using SourceType = Source;

#ifdef STATIC_ASSERT
        static constexpr bool supported() { return false; }
#endif

        static TargetType convert(SourceType src){
            static_assert(TypeConverter<Source, Target>::supported() == true, "Type cast not supported!");
            throw Exceptions::TypeConversationNotSupportedException(std::string("TypeConverter -> Cant convert from ") + typeid(Source).name() + " to " + typeid(Target).name());
        }
    };


    template <typename Source>
    struct TypeConverter<Source, void> {
        using TargetType = void;
        using SourceType = Source;
        static constexpr bool supported() { return true; }
        static TargetType convert(SourceType src){  }
    };

    template <typename Source>
    struct CppToJniConverter {
        using SourceType = Source;
        //using TargetType = void*;

#ifdef STATIC_ASSERT
        static constexpr bool supported() { return false; }
#endif

        static void convert(Source src){
            static_assert(CppToJniConverter<Source>::supported() == true, "Type cast not supported!");
            throw Exceptions::TypeConversationNotSupportedException(std::string("CppToJniConverter -> Cant convert from cpp type '") + typeid(Source).name() + "' to jni type");
        }
    };

    template <>
    struct CppToJniConverter<void> {
        using SourceType = void;
        using TargetType = void*;
        static void* convert(void){ return nullptr; }
    };

    template <typename Source, typename Target>
    struct JniToCppConverter {
        using TargetType = Target;
        using SourceType = Source;

#ifdef STATIC_ASSERT
        static constexpr bool supported() { return false; }
#endif
        static Target convert(Source src){
#ifdef STATIC_ASSERT
            static_assert(JniToCppConverter<Source, Target>::supported() == true, "Type cast not supported!");
#else
            JniToCppConverter<Source, Target>::supported(); //Compile error here if convert isnt supported
#endif
            throw Exceptions::TypeConversationNotSupportedException(std::string("JniToCppConverter -> Cant convert from jni type '") + typeid(Source).name() + "' to cpp '" + typeid(Target).name() + "'");
        }
    };
}