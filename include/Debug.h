#pragma once

#include <string>

namespace JNIHelper {
    namespace Debug {
        namespace Type {
            enum : uint8_t{
                NONE                = 0b00000000,
                GENERAL             = 0b00000001,
                CLASS_INSTANCE      = 0b00000010,
                RESOLVE             = 0b00000100,
                INVOKE              = 0b00001000
            };

            extern const char* type[];
        }

        extern void setDebugMask(uint8_t);
        extern uint8_t getDebugMask();
        extern void debug(int type, std::string msg);
    }
}