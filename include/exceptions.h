#pragma once

#include <exception>
#include <string>


namespace JNIHelper {
    namespace Exceptions {
        class Exception : public std::exception {
            public:
                Exception(std::string message) : message(message) {}
                Exception(const Exception& handle) : message(handle.message) {}

                const char *what() const throw() {
                    return message.c_str();
                }

            private:
                std::string message;
        };

        class LibraryNotInitializedException : public Exception {
            public:
                LibraryNotInitializedException(std::string message) : Exception(message) {}
        };

        class MissingSignatureException : public Exception {
            public:
                MissingSignatureException(std::string message) : Exception(message) {}
        };

        class TypeConversationNotSupportedException : public Exception {
            public:
                TypeConversationNotSupportedException(std::string message) : Exception(message) {}
        };

        class ClassRegisterException : public Exception {
            public:
                ClassRegisterException(std::string message) : Exception(message) {}
        };
    }
}