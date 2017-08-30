#pragma once

#include "signature/Signature.h"
#include "converter/TypeConverter.h"
#include "FieldSetter.h"
#include "Helper.h"

namespace JNIHelper {
    class JavaClass;
    class JavaField {
        public:
            JavaField(JavaClass* ownerClass, std::string name, std::string _signature, bool _static) : _ownerClass(ownerClass), _name(name), _signature(_signature), _static(_static) {
                resolveFieldId();
            }

            JavaField(const JavaField& ref) : _ownerClass(ref._ownerClass), _name(ref._name), _signature(ref._signature), _static(ref._static), _fieldID(ref._fieldID){}

            JavaClass* ownerClass(){
                return _ownerClass;
            }

            std::string name(){
                return _name;
            }

            std::string signature(){
                return _signature;
            }

            bool isStatic(){
                return _static;
            }

            jfieldID fieldId() const {
                return _fieldID;
            }
        protected:
            void resolveFieldId(){
                auto env = JNIHelper::getAttachedEnv();
                if(_static)
                    _fieldID = env->GetStaticFieldID(Helper::getJavaClass(_ownerClass), _name.c_str(), _signature.c_str());
                else
                    _fieldID = env->GetFieldID(Helper::getJavaClass(_ownerClass), _name.c_str(), _signature.c_str());
                if(!_fieldID) throw Exceptions::Exception("Could not resolve field id (Name: " + _name + ", Signature: " + _signature + ", Static: " + (_static ? "true" : "false") + ")");
            }

            jfieldID _fieldID = nullptr;
        private:
            JavaClass* _ownerClass;
            std::string _name;
            std::string _signature;
            bool _static;
    };

    template <typename Type>
    class StaticJavaField : public JavaField {
        public:
            StaticJavaField(JavaClass* owner, std::string name, std::string sig) : JavaField(owner, name, sig, true){}

            Type getValue(){
                auto env = JNIHelper::getAttachedEnv();
                if(FieldHelper::FieldActor<typename CppToJniConverter<Type>::TargetType>::supported()){
                    auto jniType = FieldHelper::FieldActor<typename CppToJniConverter<Type>::TargetType>::getStatic(env, Helper::getJavaClass(ownerClass()), fieldId());
                    return JniToCppConverter<decltype(jniType), Type>::convert(jniType);
                }
                auto response = env->GetStaticObjectField(Helper::getJavaClass(ownerClass()), fieldId());
                auto jniType = TypeConverter<jvalue, typename CppToJniConverter<Type>::TargetType>::convert(*(jvalue*) &response);
                return JniToCppConverter<decltype(jniType), Type>::convert(jniType);
            }

            void setValue(Type val){
                auto jniType = CppToJniConverter<Type>::convert(val);
                auto env = JNIHelper::getAttachedEnv();

                if(FieldHelper::FieldActor<decltype(jniType)>::supported()){
                    FieldHelper::FieldActor<decltype(jniType)>::setStatic(env, Helper::getJavaClass(ownerClass()), fieldId(), jniType);
                } else {
                    auto jval = TypeConverter<decltype(jniType), jvalue>::convert(jniType);
                    env->SetStaticObjectField(Helper::getJavaClass(ownerClass()), fieldId(), *(jobject*) &jval);
                }
            }

            operator Type(){
                return getValue();
            }

            StaticJavaField<Type> operator=(Type value){
                setValue(value);
                return *this;
            }
    };

    template <typename Type>
    class InstanceJavaField : public JavaField {
        public:
            InstanceJavaField(const InstanceJavaField<Type>& ref) : InstanceJavaField(&ref, ref.instance){}

            InstanceJavaField(const JavaField* owner,const JavaObject* handle) : JavaField(*owner), instance(handle){}

            Type getValue() const {
                auto env = JNIHelper::getAttachedEnv();
                if(FieldHelper::FieldActor<typename CppToJniConverter<Type>::TargetType>::supported()){
                    auto jniType = FieldHelper::FieldActor<typename CppToJniConverter<Type>::TargetType>::getInstance(env, Helper::getJavaObject(instance), fieldId());
                    return JniToCppConverter<decltype(jniType), Type>::convert(jniType);
                }
                auto response = env->GetObjectField(Helper::getJavaObject(instance), fieldId());
                auto jniType = TypeConverter<jvalue, typename CppToJniConverter<Type>::TargetType>::convert(*(jvalue*) &response);
                return JniToCppConverter<decltype(jniType), Type>::convert(jniType);
            }

            void setValue(Type val) const {
                auto jniType = CppToJniConverter<Type>::convert(val);
                auto env = JNIHelper::getAttachedEnv();

                if(FieldHelper::FieldActor<decltype(jniType)>::supported()){
                    FieldHelper::FieldActor<decltype(jniType)>::setInstance(env, Helper::getJavaObject(instance), fieldId(), jniType);
                } else {
                    auto jval = TypeConverter<decltype(jniType), jvalue>::convert(jniType);
                    env->SetObjectField(Helper::getJavaObject(instance), fieldId(), *(jobject*) &jval);
                }
            }

            operator Type() const {
                return getValue();
            }

            InstanceJavaField<Type> operator=(Type value) const {
                setValue(value);
                return *this;
            }
        private:
            const JavaObject* instance;
    };
}