#pragma once

#include <vector>

namespace JNIHelper {
    struct CppParmsToJniParms {
        public:
            static void convertArguments(std::vector<jvalue>*){};

            template <typename T, typename... Left>
            static void convertArguments(std::vector<jvalue>* output,T argument, Left... args){
                //std::cout << "Having: " << "" << " -> " << typeid(argument).name() << std::endl;
                output->push_back(TypeConverter<typename CppToJniConverter<T>::TargetType, jvalue>::convert(CppToJniConverter<T>::convert(argument)));
                convertArguments(output, args...);
            };
    };

    template <typename RetType, typename... Arguments>
    struct CPPJValueArgsConverter {
        static std::vector<jvalue>* expendArgumentsS(Arguments... args){ //static assambler methode foreward call
            std::vector<jvalue>* vec = new std::vector<jvalue>();
            CppParmsToJniParms::convertArguments(vec, args...);
            return vec;
        }
        static std::vector<jvalue>* expendArgumentsI(void* _thisPtr, Arguments... args){ //instance assambler methode foreward call
            return expendArgumentsS(args...);
        }
    };
}