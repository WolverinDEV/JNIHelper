cmake_minimum_required(VERSION 3.6)
project(JNIHelper)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -masm=intel") # -fpermissive

include_directories(/usr/lib/jvm/java-8-openjdk-amd64/include/ jni/)
include_directories(/usr/lib/jvm/java-8-openjdk-amd64/include/linux/ jni/)

set(HEADERS include/signature/SignatureDefinitions.h include/signature/DefaultSignatures.h include/exceptions.h include/signature/Signature.h include/ClassDefinitions.h include/converter/TypeConverterDefinitions.h
		include/converter/TypeConverter.h include/converter/DefaultConverter.h include/converter/MethodeParameterConverter.h include/MethodeInfo.h include/Helper.h include/JNIHelper.h include/FieldInfo.h include/FieldSetter.h include/Debug.h)

set(SOURCE_FILES main.cpp src/ClassDefinitions.cpp src/HelperImpl.cpp src/JNIHelper.cpp src/DefaultConverterFunctions.cpp src/Debug.cpp)
add_executable(JNIHelper ${SOURCE_FILES} ${HEADERS})
target_link_libraries(JNIHelper pthread)
add_library(JavaJNI SHARED ${SOURCE_FILES} ${HEADERS} test/JavaTest.cpp)