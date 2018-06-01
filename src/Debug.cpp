#include "../include/Debug.h"
#include "../include/exceptions.h"

#include <iostream>
using namespace std;
namespace JNIHelper {
    namespace Debug {
        namespace Type {
            const char* type[] = {
                "GENERAL",
                "CLASS_INSTANCE",
                "RESOLVE",
                "INVOKE",
                "BIND"
            };
        }

        const int MAX_BITS = sizeof(Type::GENERAL) * 8;
        static uint8_t debugMask = 0xFF;

        void debug(int type, const std::string& msg){
            if(!(type & debugMask)) return;

            int index = 0;
            while(index < MAX_BITS && (type & (1 << index)) == 0) index++;

            if(index == MAX_BITS){
                cout << "[DEBUG][UNKNOWN] " << msg << endl;
                return;
            }
            if(type & (1 << index) == 0) throw Exceptions::Exception("Invalid debug type!");

            cout << "[DEBUG][" << Type::type[index] << "] " << msg << endl;
        }

        void setDebugMask(uint8_t mask){
            debugMask = mask;
        }

        uint8_t getDebugMask(){
            return debugMask;
        }
    }
}