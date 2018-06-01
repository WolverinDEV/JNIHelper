#pragma once

#include <sstream>
#include <assert.h>
#include <jni.h>
#include <cstring>
#include "signature/Signature.h"
#include "converter/TypeConverter.h"
#include "Helper.h"
#include "Debug.h"

#ifndef WIN32

#include <sys/mman.h>
#include <zconf.h>

#else
#include <Windows.h>
#include <Psapi.h>
#endif


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
                this->resolveMethodInfo();
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

    template <typename T>
    inline std::string os_print(T obj) {
        std::stringstream ss;
        ss << obj;
        return ss.str();
    }

    template <typename ReturnType, typename... Arguments>
    class StaticMethodInfo : public MethodInfoBase<ReturnType, Arguments...> {
        public:
            StaticMethodInfo() = delete;
            StaticMethodInfo(const StaticMethodInfo&) = delete;
            StaticMethodInfo(StaticMethodInfo&&) = delete;

            StaticMethodInfo(JavaClass *owner, const std::string& name) : MethodInfoBase<ReturnType, Arguments...>(owner, name, true) {}

            ReturnType operator()(Arguments... args){ return invokeStatic(args...); }

            template <typename T, typename std::enable_if<std::is_same<T, void>::value, int>::type = 1>
            static void c(void*) {};
            template <typename T, typename std::enable_if<!std::is_same<T, void>::value, int>::type = 1>
            static void* c(void* val) {
                return (void*) CppToJniConverter<ReturnType>::convert(*(T*) (void*) &val);
            };

            static typename CppToJniConverter<ReturnType>::TargetType proxy(JNIEnv*, jclass, typename CppToJniConverter<Arguments>::TargetType... arguments) {
                std::function<ReturnType(Arguments...)>* fn;
                asm("\rmovq %%rax, %0" : "=r"(fn));
                //Debug::debug(Debug::Type::BIND, "[STATIC] Got proxied call! Arg count: " + os_print(sizeof...(Arguments)));
                if(typeid(ReturnType) != typeid(void)) { //RAX would be overridden
                    (*fn)(JniToCppConverter<typename CppToJniConverter<Arguments>::TargetType, Arguments>::convert(arguments)...);
                    void* result;
                    asm("\rmov %%rax, %0" : "=r"(result));
                    c<ReturnType>(result); //Result already stored in rax
                    //return CppToJniConverter<ReturnType>::convert(*result);
                } else {
                    (*fn)(JniToCppConverter<typename CppToJniConverter<Arguments>::TargetType, Arguments>::convert(arguments)...);
                }
            };

        bool make_exe(uintptr_t address, size_t length) {
            uintptr_t pageAddress = address;

    #ifdef WIN32
                DWORD old;
                if(!VirtualProtect((PBYTE) address, length, PAGE_EXECUTE_READWRITE, &old)) {
                    //TODO better error handling
                    cerr << "Could not change address access! (" << pageAddress << ")" << endl;
                }
    #else
                do {
                    pageAddress -= pageAddress % sysconf(_SC_PAGE_SIZE);
                    auto result = mprotect((void *) pageAddress, length, PROT_EXEC | PROT_READ | PROT_WRITE);
                    if (result != 0) {
                        //TODO better error handling
                        std::cerr << "Could not change page access! (" << pageAddress << ")" << std::endl;
                        return false;
                    }
                    pageAddress += sysconf(_SC_PAGE_SIZE);
                } while (pageAddress < address + length);
    #endif
                return true;
            }


        bool bind(const std::function<ReturnType(Arguments...)>& target) {
                Debug::debug(Debug::Type::BIND, "[STATIC] Binding " + this->name() + this->signature()+" to c++ std::function<...>");
                auto env = JNIHelper::getAttachedEnv();

                JNINativeMethod native{};
                native.name = (char *) this->name().c_str();
                native.signature = (char *) this->signature().c_str();

                auto alloc = new std::function<ReturnType(Arguments...)>(target);
                size_t jmp_target = (size_t) &proxy;
                size_t function_target = (size_t) alloc;
                u_char* buffer = new u_char[24]{
                        0x48, 0xb8,                             //movabs %rax, $address
                        static_cast<u_char>((function_target >> 0) & 0xFF),
                        static_cast<u_char>((function_target >> 8) & 0xFF),
                        static_cast<u_char>((function_target >> 16) & 0xFF),
                        static_cast<u_char>((function_target >> 24) & 0xFF),
                        static_cast<u_char>((function_target >> 32) & 0xFF),
                        static_cast<u_char>((function_target >> 40) & 0xFF),
                        static_cast<u_char>((function_target >> 48) & 0xFF),
                        static_cast<u_char>((function_target >> 56) & 0xFF),

                        /*
                        0x50, //push %rax

                        0x48, 0xb8,                             //movabs %rax, $address
                        static_cast<u_char>((jmp_target >> 0) & 0xFF),
                        static_cast<u_char>((jmp_target >> 8) & 0xFF),
                        static_cast<u_char>((jmp_target >> 16) & 0xFF),
                        static_cast<u_char>((jmp_target >> 24) & 0xFF),
                        static_cast<u_char>((jmp_target >> 32) & 0xFF),
                        static_cast<u_char>((jmp_target >> 40) & 0xFF),
                        static_cast<u_char>((jmp_target >> 48) & 0xFF),
                        static_cast<u_char>((jmp_target >> 56) & 0xFF),

                        //0xff, 0xe0,                           //jmp %rax
                        0xff, 0xe0,
                        */
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,                // jmp qword ptr [$+6]
                        static_cast<u_char>((jmp_target >> 0) & 0xFF),     // ptr
                        static_cast<u_char>((jmp_target >> 8) & 0xFF),
                        static_cast<u_char>((jmp_target >> 16) & 0xFF),
                        static_cast<u_char>((jmp_target >> 24) & 0xFF),
                        static_cast<u_char>((jmp_target >> 32) & 0xFF),
                        static_cast<u_char>((jmp_target >> 40) & 0xFF),
                        static_cast<u_char>((jmp_target >> 48) & 0xFF),
                        static_cast<u_char>((jmp_target >> 56) & 0xFF),
                };
                native.fnPtr = (void*) buffer;
                make_exe((uintptr_t) buffer, 23);


                Debug::debug(Debug::Type::BIND, "[STATIC] Allocated generated method at " + os_print((void*) buffer) + " target = " + os_print(alloc));
                //native.fnPtr = (void*) getAddress(*alloc);

                if(!env->RegisterNatives(JNIHelper::Helper::getJavaClass(this->ownerClass()), &native, 1));
                //TODO exception check

                return true;
            }
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

            std::shared_ptr<std::function<ReturnType(Arguments...)>> native;
    };

    template <typename ReturnType, typename... Arguments>
    class InstanceMethodInfo : public MethodInfoBase<ReturnType, Arguments...> {
        public:
            InstanceMethodInfo() = delete;
            InstanceMethodInfo(const InstanceMethodInfo<ReturnType, Arguments...>& old) = delete;
            InstanceMethodInfo(InstanceMethodInfo<ReturnType, Arguments...>&&) = delete;

            InstanceMethodInfo(const std::unique_ptr<MethodInfoBase<ReturnType, Arguments...>>& method, jobject object) : MethodInfoBase<ReturnType, Arguments...>(method->ownerClass(), method->name(), method->isStatic(), method->signature(),
                                                                                                                                              method->methodId()), instance(object) {
                assert(!method->isStatic());
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