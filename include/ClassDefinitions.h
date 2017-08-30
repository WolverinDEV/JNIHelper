#pragma once

#include <string>
#include <iostream>
#include <deque>
#include <cassert>
#include <memory>
#include "converter/TypeConverter.h"
#include "MethodeInfo.h"
#include "FieldInfo.h"

#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

namespace JNIHelper {
    class JavaObject;

    class JavaClass {
            friend class JavaObject;
        public:
            const std::string package;
            const std::string name;

            JavaClass *getSuperClass() const {
                return superClass;
            }

            template<typename ReturnType, typename... Arguments>
            MethodeInfoBase<ReturnType, Arguments...> *registerMethode(std::string name, bool _static) {
                if (findMethode<ReturnType, Arguments...>(name)) throw Exceptions::ClassRegisterException("Methode " + name + "(Signature: " + findMethode<ReturnType, Arguments...>(name)->signature() + ") alredy registered!");

                MethodeInfoBase<ReturnType, Arguments...> *methode = nullptr;
                if(_static){
                    methode = new StaticMethodeInfo<ReturnType, Arguments...>(this, name);
                } else {
                    methode = new MethodeInfoBase<ReturnType, Arguments...>(this, name, false); //Instance methode will be generated by class
                }

                methods.push_back(methode);
                return methode;
            };


            template<typename ReturnType, typename... Arguments>
            MethodeInfoBase<ReturnType, Arguments...> *findMethode(std::string name) {
                for (auto methode : methods)
                    if (methode->name().compare(name) == 0)
                        if (dynamic_cast<MethodeInfoBase<ReturnType, Arguments...> *>(methode)) return dynamic_cast<MethodeInfoBase<ReturnType, Arguments...> *>(methode);
                return nullptr;
            };

            template<typename ReturnType, typename... Arguments>
            MethodeInfoBase<ReturnType, Arguments...> *findOrGetMethode(std::string name, bool _static) {
                auto result = findMethode<ReturnType, Arguments...>(name);
                if(result) return result;
                return registerMethode<ReturnType, Arguments...>(name, _static);
            };

            std::deque<AbstractMethodeInfo *> getMethods(){
                return this->methods;
            }

            JavaField* findField(std::string name){
                for(auto field : fields)
                    if(field->name().compare(name) == 0) return field;
                return nullptr;
            }

            template <typename Type>
            JavaField* registerField(std::string name, bool _static){
                if(findField(name)) throw Exceptions::ClassRegisterException("Field " + name + " alredy registered!");

                Debug::debug(Debug::Type::RESOLVE, "Resolving field " + name + " (" + (_static ? "static" : "non-static") + ")" + " in class " + getJavaSignature());
                JavaField* field;

                std::string signature;
                if(Signature::TypeSignature<Type>::avariable()) signature = Signature::TypeSignature<Type>::signature();
                else signature = Signature::TypeSignature<typename CppToJniConverter<Type>::TargetType>::signature();
                if(_static) field = new StaticJavaField<Type>(this, name, signature);
                else field = new JavaField(this, name, signature, false);
                fields.push_back(field);
                return field;
            }

            template <typename Type>
            JavaField* getOrRegisterField(std::string name, bool _static){
                auto result = findField(name);
                if(result) return result;
                return registerField<Type>(name, _static);
            }

            void resolveClassInfo(){ //TODO make this private
                auto env = JNIHelper::getAttachedEnv();

                std::string package = this->package;
                size_t index = 0;
                while((index = package.find(".", index)) != std::string::npos){
                    package = package.replace(index, 1, "/");
                    index += 1;
                }

                Debug::debug(Debug::Type::RESOLVE, "Resolving class " + package + "/" + name);
                javaClass = env->FindClass((package + "/" + name).c_str());
                if(!javaClass) throw Exceptions::Exception("Cant resolve java class for " + package + "." + name + " (" + package + "/" + name + ")");

                jsig = "L" + package + "/" + name + ";";
            }

            jclass getJavaClass(){
                return javaClass;
            }

            std::string getName(){
                return package + "." + name;
            };

            std::string getJavaSignature(){
                return jsig;
            }
        private:
            JavaClass *superClass;
            std::deque<AbstractMethodeInfo *> methods; //JavaField
            std::deque<JavaField *> fields;
            jclass javaClass = nullptr;
            std::string jsig;

            JavaClass(std::string package, std::string name, JavaClass *superClass);
    };

    class JavaObject {
        public:
            static JNIHelper::JavaClass *getClass() { return getDescription("java_lang", "Object", nullptr); }; //As super class nullptr becorde java lang object should be inizalisized
        protected:
            static JavaClass *getDescription(std::string package, std::string name, JavaClass *superClass);

        private:
            static JavaClass *buildObjectClass();
            static std::deque<JavaClass *> classes;

        public:
            JavaObject(std::shared_ptr<void*> javaInstance);
            JavaObject(const JavaObject& ref);
            JavaObject();

            virtual ~JavaObject();

            template<typename ReturnType, typename... Arguments>
            InstanceMethodeInfo<ReturnType, Arguments...> *getMethode(std::string name) {
                for (auto methode : methods)
                    if (methode->name().compare(name) == 0)
                        if (dynamic_cast<InstanceMethodeInfo<ReturnType, Arguments...> *>(methode)) return dynamic_cast<InstanceMethodeInfo<ReturnType, Arguments...> *>(methode);
                auto methodeInfo = getObjectClass()->findOrGetMethode<ReturnType, Arguments...>(name, false);
                assert(methodeInfo != nullptr); //TODO throw exception
                InstanceMethodeInfo<ReturnType, Arguments...>* methode = new InstanceMethodeInfo<ReturnType, Arguments...>(methodeInfo, (jobject) javaInstance.get());
                methods.push_back(methode);
                return methode;
            };

            jobject getJavaObject() const {
                return (jobject) javaInstance.get();
            }

            virtual JNIHelper::JavaClass * getObjectClass() { return JavaObject::getClass(); }
        protected:
            void constructNewObject();
        private:
            std::deque<AbstractMethodeInfo *> methods;
            std::shared_ptr<void*> javaInstance = nullptr;
    };
}

#define PREDEFINE_JAVA_CLASS(package, name) \
namespace package { \
    class name;     \
}

#define DEFINE_JAVA_CLASS(package, name, superClass, content)           \
namespace package { \
    class name : public superClass {       \
        public:     \
            static JNIHelper::JavaClass* getClass(){ return JNIHelper::JavaObject::getDescription(STRINGIZE(package), STRINGIZE(name), superClass::getClass()); }       \
        public: \
            name() : superClass() { constructNewObject(); } \
            name(std::shared_ptr<void*> javaInstance) : superClass(javaInstance) {} \
            virtual JNIHelper::JavaClass * getObjectClass() override { return name::getClass(); } \
            content \
    }; \
} \
namespace JNIHelper { \
    DEFINE_CPP_JNI_CONVERTER(package::name, jobject, return JNIHelper::DefaultConverterImpl::convertJObject((JavaObject*) &src)); \
    DEFINE_CPP_JNI_CONVERTER(package::name*, jobject, return JNIHelper::DefaultConverterImpl::convertJObject((JavaObject*) src)); \
    DEFINE_CPP_JNI_CONVERTER(package::name&, jobject, return JNIHelper::DefaultConverterImpl::convertJObject((JavaObject*) &src)); \
    DEFINE_CPP_JNI_CONVERTER(const package::name&, jobject, return JNIHelper::DefaultConverterImpl::convertJObject((JavaObject*) &src)); \
    DEFINE_JNI_CPP_CONVERTER(jobject, package::name*, return new package::name(JNIHelper::DefaultConverterImpl::createJavaObjectPtr(src))) \
    DEFINE_JNI_CPP_CONVERTER(jobject, package::name, return package::name(JNIHelper::DefaultConverterImpl::createJavaObjectPtr(src))) \
    namespace Signature { \
        DEFINE_TYPE_SIGNATURE(package::name, package::name::getClass()->getJavaSignature()); \
        DEFINE_TYPE_SIGNATURE(package::name*, package::name::getClass()->getJavaSignature()); \
        DEFINE_TYPE_SIGNATURE(package::name&, package::name::getClass()->getJavaSignature()); \
        DEFINE_TYPE_SIGNATURE(const package::name&, package::name::getClass()->getJavaSignature()); \
    }\
}

#define DEFINE_STATIC_METHODE(methodeName, return_type, ...)                                                                                        \
public:                                                                                                                                             \
    static return_type methodeName(__VA_ARGS__){                                                                                                    \
        __asm__ ("\tmovq %0, %%rax;" ::"r"((long) JNIHelper::CPPJValueArgsConverter<return_type, ##__VA_ARGS__>::expendArgumentsS));                \
        __asm__ ("\tcall %rax");                                                                                                                    \
        std::vector<jvalue>* result = nullptr;                                                                                                      \
        __asm__("\rmovq %%rax, %0" : "=r"(result));                                                                                                 \
        assert(result != nullptr);                                                                                                                  \
        std::vector<jvalue> arguments = std::vector<jvalue>(*result);                                                                               \
        delete result;                                                                                                                              \
        auto methode = getClass()->findOrGetMethode<return_type, ##__VA_ARGS__>(STRINGIZE(methodeName), true);                                      \
        assert(methode != nullptr);                                                                                                                 \
        return methode->invokeStatic(arguments);                                                                                                    \
    }

#define DEFINE_METHODE(methodeName, return_type, ...)                                                                                               \
public:                                                                                                                                             \
    virtual return_type methodeName(__VA_ARGS__){                                                                                                   \
        __asm__ ("\tmovq %0, %%rax;" ::"r"((long) JNIHelper::CPPJValueArgsConverter<return_type, ##__VA_ARGS__>::expendArgumentsI));                \
        __asm__ ("\tcall %rax");                                                                                                                    \
        std::vector<jvalue>* result = nullptr;                                                                                                      \
        __asm__("\rmovq %%rax, %0" : "=r"(result));                                                                                                 \
        assert(result != nullptr);                                                                                                                  \
        std::vector<jvalue> arguments = std::vector<jvalue>(*result);                                                                               \
        delete result;                                                                                                                              \
        auto methode = getMethode<return_type, ##__VA_ARGS__>(STRINGIZE(methodeName));                                                              \
        assert(methode != nullptr);                                                                                                                 \
        return methode->invokeObject(arguments);                                                                                                    \
    }

#define DEFINE_CONSTRUCTOR(cppMethodeName, ...)                                                                                                     \
public:                                                                                                                                             \
    virtual void cppMethodeName(__VA_ARGS__){                                                                                                       \
        __asm__ ("\tmovq %0, %%rax;" ::"r"((long) JNIHelper::CPPJValueArgsConverter<void, ##__VA_ARGS__>::expendArgumentsI));                       \
        __asm__ ("\tcall %rax");                                                                                                                    \
        std::vector<jvalue>* result = nullptr;                                                                                                      \
        __asm__("\rmovq %%rax, %0" : "=r"(result));                                                                                                 \
        assert(result != nullptr);                                                                                                                  \
        std::vector<jvalue> arguments = std::vector<jvalue>(*result);                                                                               \
        delete result;                                                                                                                              \
        auto methode = getMethode<void, ##__VA_ARGS__>(std::string("<init>"));                                                                      \
        assert(methode != nullptr);                                                                                                                 \
        methode->invokeObject(arguments);                                                                                                           \
    }

#define DEFINE_DEFAULT_CONSTRUCTOR(cppMethodeName) DEFINE_CONSTRUCTOR(cppMethodeName)

#define DEFINE_STATIC_FIELD(fieldName, fieldType, cppMethodeName) \
public:\
    static JNIHelper::StaticJavaField<fieldType> cppMethodeName(){\
            return *(JNIHelper::StaticJavaField<fieldType>*) getClass()->getOrRegisterField<fieldType>(STRINGIZE(fieldName), true); \
            \
    }

#define DEFINE_FIELD(fieldName, fieldType) \
public:\
    const JNIHelper::InstanceJavaField<fieldType> fieldName = JNIHelper::InstanceJavaField<fieldType>(getClass()->getOrRegisterField<fieldType>(STRINGIZE(fieldName), false), this);

DEFINE_JAVA_CLASS(dev_wolveringer, Test, JNIHelper::JavaObject,
                  DEFINE_STATIC_METHODE(testMethode, void);
                  DEFINE_STATIC_METHODE(testMethode, void, int);
                  DEFINE_STATIC_METHODE(testMethode, void, double, void*);

                  DEFINE_DEFAULT_CONSTRUCTOR(constructor1);
                  DEFINE_CONSTRUCTOR(constructor2, int)

                  DEFINE_METHODE(instanceTestMethode, void, long, double);

);
DEFINE_JAVA_CLASS(dev_wolveringer, Test1, dev_wolveringer::Test,);
