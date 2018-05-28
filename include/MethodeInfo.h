#pragma once

#include <assert.h>
#include "signature/Signature.h"
#include "converter/TypeConverter.h"
#include "Helper.h"
#include "Debug.h"

namespace JNIHelper {
    class JavaClass;
    class JavaObject;

    class AbstractMethodInfo {
        public:
            virtual JavaClass* ownerClass() = 0;
            virtual bool isStatic() = 0;
            virtual std::string name() = 0;
            virtual std::string signature() = 0;
    };

    template <typename ReturnType, typename... Arguments>
    class MethodInfoBase : public AbstractMethodInfo {
        public:
            MethodInfoBase(JavaClass* owner, std::string name, bool _static) : _ownerClass(owner), _name(name), _static(_static), _signature(Signature::MethodeSignature<ReturnType, Arguments...>::signature()){
                resolveMethodInfo();
            }

            JavaClass* ownerClass() override { return _ownerClass; }

            bool isStatic() override { return _static; }

            std::string name() override { return _name; }

            std::string signature() override {
                return _signature;
            }

            jmethodID methodId(){
                return _methodId;
            }

            virtual ReturnType invokeStatic(Arguments... args){
                std::vector<jvalue> arguments;
                CppParmsToJniParms::convertArguments(arguments, args...);
                return invokeStatic(arguments);
            }
            virtual ReturnType invokeObject(jobject obj, Arguments... args){
                std::vector<jvalue> arguments;
                CppParmsToJniParms::convertArguments(arguments, args...);
                return invokeObject(obj, arguments);
            }

            virtual ReturnType invokeStatic(const std::vector<jvalue>& args){
                assert(_static);
                return invokeStaticImpl(args);
            }
            virtual ReturnType invokeObject(jobject obj, const std::vector<jvalue>& args){
                assert(!_static);
                return invokeObjectImpl(obj, args);
            }

        protected:
            MethodInfoBase(JavaClass* owner, std::string name, bool _static, std::string signature, jmethodID id) : _ownerClass(owner), _name(name), _static(_static), _signature(signature), _methodId(id){ }

            virtual ReturnType invokeStaticImpl(const std::vector<jvalue>&) { throw Exceptions::Exception("Abstract call!"); };
            virtual ReturnType invokeObjectImpl(jobject, const std::vector<jvalue>&) { throw Exceptions::Exception("Abstract call!"); };

            jmethodID _methodId = nullptr;
        private:
            void resolveMethodInfo(){
                auto env = JNIHelper::getAttachedEnv();
                Debug::debug(Debug::Type::RESOLVE, "Resolving methodID for method " + name() + signature() + " for class " + Helper::getJavaClassSignature(_ownerClass));
                auto javaClass = Helper::getJavaClass(_ownerClass);
                assert(javaClass != nullptr);
                if(_static)
                    _methodId = env->GetStaticMethodID(javaClass, name().c_str(), signature().c_str());
                else
                    _methodId = env->GetMethodID(javaClass, name().c_str(), signature().c_str());
                if(!_methodId) throw Exceptions::Exception("Could not resolve method id for " + name() + signature()+" (Static: " + (_static ? "true" : "false") + ")");
            }

            JavaClass* _ownerClass;
            bool _static;
            std::string _name;
            std::string _signature;
    };

    template <typename ReturnType, typename... Arguments>
    class StaticMethodInfo : public MethodInfoBase<ReturnType, Arguments...> {
        public:
            StaticMethodInfo() = delete;
            StaticMethodInfo(JavaClass *owner, std::string name) : MethodInfoBase<ReturnType, Arguments...>(owner, name, true) {}
            StaticMethodInfo(StaticMethodInfo<ReturnType, Arguments...>& old) : StaticMethodInfo(old.ownerClass(), old.name()) {
                assert(old.isStatic());
            }

            ReturnType operator()(Arguments... args){ return invokeStatic(args...); }
        protected:
            virtual ReturnType invokeStaticImpl(const std::vector<jvalue>& vector) override {
                Debug::debug(Debug::Type::INVOKE, "[STATIC] Invoking ret type: " + this->name() + this->signature()+" with " + std::to_string(vector.size()) + " args");
                auto env = JNIHelper::getAttachedEnv();
                auto response = env->CallStaticObjectMethodA(Helper::getJavaClass(this->ownerClass()), this->methodId(), &vector[0]);
                auto jniType = TypeConverter<jvalue, typename CppToJniConverter<ReturnType>::TargetType>::convert(*(jvalue*) &response);
                return JniToCppConverter<decltype(jniType), ReturnType>::convert(jniType);
            }

            virtual ReturnType invokeObjectImpl(jobject jobject1, const std::vector<jvalue>& vector) override {
                throw Exceptions::Exception("Static invoke only!");
            }
    };

    template <typename ReturnType, typename... Arguments>
    class InstanceMethodeInfo : public MethodInfoBase<ReturnType, Arguments...> {
        public:
            InstanceMethodeInfo() = delete;
            InstanceMethodeInfo(InstanceMethodeInfo<ReturnType, Arguments...>& old) : InstanceMethodeInfo(&old, old.instance) {
                assert(!old.isStatic());
            }
            InstanceMethodeInfo(MethodInfoBase<ReturnType, Arguments...>* methode, jobject object) : MethodInfoBase<ReturnType, Arguments...>(methode->ownerClass(), methode->name(), methode->isStatic(), methode->signature(),
                                                                                                                                              methode->methodId()), instance(object) {
                assert(methode->isStatic() == false);
            }

            ReturnType operator()(Arguments... args){ return MethodInfoBase<void, Arguments...>::invokeObject(instance, args...); }
            virtual ReturnType invokeObject(std::vector<jvalue> args){ return invokeObjectImpl(instance, args); }

        protected:
            virtual ReturnType invokeStaticImpl(const std::vector<jvalue>& vector) override {
                throw Exceptions::Exception("Instance invoke only!");
            }

            virtual ReturnType invokeObjectImpl(jobject jobject1, const std::vector<jvalue>& vector) override {
                Debug::debug(Debug::Type::INVOKE, "[Instance] Invoking ret type: " + this->name() + this->signature()+" with " + std::to_string(vector.size()) + " args");
                auto env = JNIHelper::getAttachedEnv();
                auto response = env->CallObjectMethodA(jobject1, this->methodId(), &vector[0]);
                auto jniType = TypeConverter<jvalue, typename CppToJniConverter<ReturnType>::TargetType>::convert(*(jvalue*) &response);
                return JniToCppConverter<decltype(jniType), ReturnType>::convert(jniType);

            }
        private:
            jobject instance;
    };
}