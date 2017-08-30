#pragma once

#include <string>
#include <typeinfo>
#include "../exceptions.h"
#include "../converter/TypeConverter.h"

namespace JNIHelper {
    namespace Signature {
        /**
     * Type converters
     */
        template<typename T>
        struct TypeSignature {
            static bool avariable() { return false; }
            static std::string signature() {
                throw Exceptions::MissingSignatureException("Missing signature for type '" + std::string(typeid(T).name()) + "'");
            }
        };


        #define DEFINE_TYPE_SIGNATURE(type, _signature)                 \
        template <>                                                     \
        struct TypeSignature<type> {                                    \
            static bool avariable() { return true; }                    \
            static std::string signature(){ return _signature; }        \
        };


        /**
         * Methode converters
         */
        template<typename...elements>
        struct MethodeSignatureParameterConnector {
        };

        template<typename elm, typename ...other>
        struct MethodeSignatureParameterConnector<elm, other...> {
            static std::string signature() {
                if(TypeSignature<elm>::avariable()) return TypeSignature<elm>::signature() + MethodeSignatureParameterConnector<other...>::signature();
                return TypeSignature<typename CppToJniConverter<elm>::TargetType>::signature() + MethodeSignatureParameterConnector<other...>::signature();
            }
        };

        template<>
        struct MethodeSignatureParameterConnector<> {
            static std::string signature() {
                return "";
            }
        };

        template<typename ReturnType, typename ... Args>
        struct MethodeSignature {
            static std::string signature() {
                std::string output;
                output += "(";

                output += MethodeSignatureParameterConnector<Args...>::signature();

                output += ")";
                if(TypeSignature<ReturnType>::avariable())
                    output += TypeSignature<ReturnType>::signature();
                else
                    output += TypeSignature<typename CppToJniConverter<ReturnType>::TargetType>::signature();
                return output.c_str();
            }
        };

        template<typename ... Args>
        struct MethodeSignature<void, Args...> {
            static std::string signature() {
                    std::string output;
                    output += "(";

                    output += MethodeSignatureParameterConnector<Args...>::signature();

                    output += ")V";
                    return output.c_str();
            }
        };
    }
}