#pragma once

#include <string>

namespace JNIHelper {
    namespace Debug {
        namespace Type {
            enum : uint8_t{
                NONE                = 0b00000000,
                FULL                = 0b11111111,
                GENERAL             = 0b00000001,
                CLASS_INSTANCE      = 0b00000010,
                RESOLVE             = 0b00000100,
                INVOKE              = 0b00001000,
                BIND                = 0b00010000,
            };

            extern const char* type[];
        }

        extern void setDebugMask(uint8_t);
        extern uint8_t getDebugMask();
        extern void debug(int type, const std::string& msg);
    }
}