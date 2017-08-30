#include <mutex>
#include <cassert>
#include <sstream>
#include <iomanip>
#include "../include/ClassDefinitions.h"

using namespace JNIHelper;
using namespace std;

JavaClass::JavaClass(std::string package, std::string name, JavaClass *superClass) : package(package), name(name), superClass(superClass) {
    if (superClass) resolveClassInfo();
}

JavaClass *JavaObject::buildObjectClass() {
    JavaClass *cls = new JavaClass("java.lang", "Object", nullptr);
    cls->superClass = cls;
    return cls;
}

std::mutex classListLock;
std::deque<JavaClass *> JavaObject::classes = {buildObjectClass()};

JavaClass *JavaObject::getDescription(std::string package, std::string name, JavaClass *superClass) {
    string javaPackage = package;
    while (javaPackage.find('_') != string::npos) javaPackage[javaPackage.find('_')] = '.';

    classListLock.lock();
    for (auto descriptor : classes) {
        if (descriptor->package.compare(javaPackage) == 0 && descriptor->name.compare(name) == 0) {
            classListLock.unlock();
            return descriptor;
        }
    }

    assert(superClass != nullptr);
    classListLock.unlock();
    JavaClass *descriptor = new JavaClass(javaPackage, name, superClass);
    classListLock.lock();
    classes.push_back(descriptor);
    classListLock.unlock();
    return descriptor;
}

JavaObject::JavaObject() {}


template <typename T>
inline std::string toHex(T val){
    stringstream ss;
    ss << hex << setfill('0') << setw(8) << val;
    return ss.str();
}

void JavaObject::constructNewObject() {
    auto env = JNIHelper::getAttachedEnv();
    jobject object = env->AllocObject(getObjectClass()->getJavaClass());
    object = env->NewGlobalRef(object);
    Debug::debug(Debug::Type::CLASS_INSTANCE, "Creating new object instance of type " + this->getObjectClass()->getJavaSignature() + " -> " + toHex(object));
    this->javaInstance = shared_ptr<void *>((void **) object, [](void *obj) {
        if (!obj) return;
        Debug::debug(Debug::Type::CLASS_INSTANCE, "Deleting object instance " + toHex(obj));
        JNIHelper::getAttachedEnv()->DeleteGlobalRef((jobject) obj);
    });
}

JavaObject::JavaObject(const JavaObject &ref) : javaInstance(ref.javaInstance), methods(ref.methods) {}

JavaObject::JavaObject(std::shared_ptr<void *> javaInstance) : javaInstance(javaInstance) {
}

JavaObject::~JavaObject() {
    this->javaInstance = nullptr;
}