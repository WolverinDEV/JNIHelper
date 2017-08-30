#pragma once

#include "signature/Signature.h"
#include "converter/TypeConverter.h"
#include "Helper.h"
#include "Debug.h"

namespace JNIHelper {
    class JavaClass;
    class JavaObject;

    class AbstractMethodeInfo {
        public:
            virtual JavaClass* ownerClass() = 0;
            virtual bool isStatic() = 0;
            virtual std::string name() = 0;
            virtual std::string signature() = 0;
    };

    template <typename ReturnType, typename... Arguments>
    class MethodeInfoBase : public AbstractMethodeInfo {
        public:
            MethodeInfoBase(JavaClass* owner, std::string name, bool _static) : _ownerClass(owner), _name(name), _static(_static), _signature(Signature::MethodeSignature<ReturnType, Arguments...>::signature()){
                resolveMethodeInfo();
            }

            JavaClass* ownerClass() override { return _ownerClass; }

            bool isStatic() override { return _static; }

            std::string name() override { return _name; }

            std::string signature() override {
                return _signature;
            }

            jmethodID methodeId(){
                return _methodeId;
            }

            virtual ReturnType invokeStatic(Arguments... args){
                std::vector<jvalue> arguments;
                CppParmsToJniParms::convertArguments(&arguments, args...);
                return invokeStatic(arguments);
            }
            virtual ReturnType invokeObject(jobject obj, Arguments... args){
                std::vector<jvalue> arguments;
                CppParmsToJniParms::convertArguments(&arguments, args...);
                return invokeObject(obj, arguments);
            }

            virtual ReturnType invokeStatic(std::vector<jvalue> args){
                assert(_static == true);
                return invokeStaticImpl(args);
            }
            virtual ReturnType invokeObject(jobject obj, std::vector<jvalue> args){
                assert(_static == false);
                return invokeObjectImpl(obj, args);
            }

        protected:
            MethodeInfoBase(JavaClass* owner, std::string name, bool _static, std::string signature, jmethodID id) : _ownerClass(owner), _name(name), _static(_static), _signature(signature), _methodeId(id){ }

            virtual ReturnType invokeStaticImpl(std::vector<jvalue>) { throw Exceptions::Exception("Abstract call!"); };
            virtual ReturnType invokeObjectImpl(jobject, std::vector<jvalue>) { throw Exceptions::Exception("Abstract call!"); };

            jmethodID _methodeId = nullptr;
        private:
            void resolveMethodeInfo(){
                auto env = JNIHelper::getAttachedEnv();
                Debug::debug(Debug::Type::RESOLVE, "Resolving methodID for methode " + name() + signature() + " for class " + Helper::getJavaClassSignature(_ownerClass));
                auto javaClass = Helper::getJavaClass(_ownerClass);
                assert(javaClass != nullptr);
                if(_static)
                    _methodeId = env->GetStaticMethodID(javaClass, name().c_str(), signature().c_str());
                else
                    _methodeId = env->GetMethodID(javaClass, name().c_str(), signature().c_str());
                if(!_methodeId) throw Exceptions::Exception("Could not resolve methode id for " + name() + signature()+" (Static: " + (_static ? "true" : "false") + ")");
            }

            JavaClass* _ownerClass;
            bool _static;
            std::string _name;
            std::string _signature;
    };

    template <typename ReturnType, typename... Arguments>
    class StaticMethodeInfo : public MethodeInfoBase<ReturnType, Arguments...> {
        public:
            StaticMethodeInfo() = delete;
            StaticMethodeInfo(StaticMethodeInfo<ReturnType, Arguments...>& old) : StaticMethodeInfo(old.ownerClass(), old.name()) {
                assert(old.isStatic() == true);
            }
            StaticMethodeInfo(JavaClass *owner, std::string name) : MethodeInfoBase<ReturnType, Arguments...>(owner, name, true) {}

            ReturnType operator()(Arguments... args){ return invokeStatic(args...); }
        protected:
            virtual ReturnType invokeStaticImpl(std::vector<jvalue> vector) override {
                Debug::debug(Debug::Type::INVOKE, "[STATIC] Invoking ret type: " + this->name() + this->signature()+" with " + std::to_string(vector.size()) + " args");
                auto env = JNIHelper::getAttachedEnv();
                auto response = env->CallStaticObjectMethodA(Helper::getJavaClass(this->ownerClass()), this->methodeId(), &vector[0]);
                auto jniType = TypeConverter<jvalue, typename CppToJniConverter<ReturnType>::TargetType>::convert(*(jvalue*) &response);
                return JniToCppConverter<decltype(jniType), ReturnType>::convert(jniType);
            }

            virtual ReturnType invokeObjectImpl(jobject jobject1, std::vector<jvalue> vector) override {
                throw Exceptions::Exception("Static invoke only!");
            }
    };

    template <typename... Arguments>
    class StaticMethodeInfo<void, Arguments...> : public MethodeInfoBase<void, Arguments...> {
        public:
            StaticMethodeInfo() = delete;
            StaticMethodeInfo(StaticMethodeInfo<void, Arguments...>& old) : StaticMethodeInfo(old.ownerClass(), old.name()) {
                assert(old.isStatic() == true);
            }
            StaticMethodeInfo(JavaClass *owner, std::string name) : MethodeInfoBase<void, Arguments...>(owner, name, true) {}

            void operator()(Arguments... args){ invokeStatic(args...); }
        protected:
            virtual void invokeStaticImpl(std::vector<jvalue> vector) override {
                Debug::debug(Debug::Type::INVOKE, "[STATIC] Invoking void type: " + this->name() + this->signature()+" with " + std::to_string(vector.size()) + " args");
                auto env = JNIHelper::getAttachedEnv();
                env->CallStaticVoidMethodA(Helper::getJavaClass(this->ownerClass()), this->methodeId(), &vector[0]);
            }

            virtual void invokeObjectImpl(jobject jobject1, std::vector<jvalue> vector) override {
                throw Exceptions::Exception("Static invoke only!");
            }
    };

    template <typename ReturnType, typename... Arguments>
    class InstanceMethodeInfo : public MethodeInfoBase<ReturnType, Arguments...> {
        public:
            InstanceMethodeInfo() = delete;
            InstanceMethodeInfo(InstanceMethodeInfo<ReturnType, Arguments...>& old) : InstanceMethodeInfo(&old, old.instance) {
                assert(old.isStatic() == false);
            }
            InstanceMethodeInfo(MethodeInfoBase<ReturnType, Arguments...>* methode, jobject object) : MethodeInfoBase<ReturnType, Arguments...>(methode->ownerClass(), methode->name(), methode->isStatic(), methode->signature(), methode->methodeId()), instance(object) {
                assert(methode->isStatic() == false);
            }

            ReturnType operator()(Arguments... args){ return MethodeInfoBase<void, Arguments...>::invokeObject(instance, args...); }
            virtual ReturnType invokeObject(std::vector<jvalue> args){ return invokeObjectImpl(instance, args); }

        protected:
            virtual ReturnType invokeStaticImpl(std::vector<jvalue> vector) override {
                throw Exceptions::Exception("Instance invoke only!");
            }

            virtual ReturnType invokeObjectImpl(jobject jobject1, std::vector<jvalue> vector) override {
                Debug::debug(Debug::Type::INVOKE, "[Instance] Invoking ret type: " + this->name() + this->signature()+" with " + std::to_string(vector.size()) + " args");
                auto env = JNIHelper::getAttachedEnv();
                auto response = env->CallObjectMethodA(jobject1, this->methodeId(), &vector[0]);
                auto jniType = TypeConverter<jvalue, typename CppToJniConverter<ReturnType>::TargetType>::convert(*(jvalue*) &response);
                return JniToCppConverter<decltype(jniType), ReturnType>::convert(jniType);

            }
        private:
            jobject instance;
    };

    template <typename... Arguments>
    class InstanceMethodeInfo<void, Arguments...> : public MethodeInfoBase<void, Arguments...> {
        public:
            InstanceMethodeInfo() = delete;
            InstanceMethodeInfo(InstanceMethodeInfo<void, Arguments...>& old) : InstanceMethodeInfo(&old, old.instance) {
                assert(old.isStatic() == false);
            }
            InstanceMethodeInfo(MethodeInfoBase<void, Arguments...>* methode, jobject object) : MethodeInfoBase<void, Arguments...>(methode->ownerClass(), methode->name(), methode->isStatic(), methode->signature(), methode->methodeId()), instance(object) {
                assert(methode->isStatic() == false);
            }

            void operator()(Arguments... args){ MethodeInfoBase<void, Arguments...>::invokeObject(instance, args...); }
            virtual void invokeObject(std::vector<jvalue> args){ invokeObjectImpl(instance, args); }

        protected:
            virtual void invokeStaticImpl(std::vector<jvalue> vector) override {
                throw Exceptions::Exception("Instance invoke only!");
            }

            virtual void invokeObjectImpl(jobject jobject1, std::vector<jvalue> vector) override {
                Debug::debug(Debug::Type::INVOKE, "[Instance] Invoking void type: " + this->name() + this->signature()+" with " + std::to_string(vector.size()) + " args");
                auto env = JNIHelper::getAttachedEnv();
                env->CallVoidMethodA(jobject1, this->methodeId(), &vector[0]);
            }
        private:
            jobject instance;
    };
}