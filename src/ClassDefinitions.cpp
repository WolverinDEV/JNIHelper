#include <mutex>
#include <cassert>
#include <sstream>
#include <iomanip>
#include "../include/ClassDefinitions.h"

using namespace JNIHelper;
using namespace std;

JavaClass::JavaClass(std::string package, std::string name, const std::shared_ptr<JavaClass>& superClass) : package(package), name(name), superClass(superClass) {
    if (superClass) resolveClassInfo();
}

shared_ptr<JavaClass> JavaClass::buildObjectClass() {
    shared_ptr<JavaClass> cls (new JavaClass("java.lang", "Object", nullptr));
    cls->superClass = cls; //Referrer to itself (No delete)
    return cls;
}

std::mutex classListLock;
std::mutex classCreateLock;
std::deque<shared_ptr<JavaClass>> JavaObject::classes = {JavaClass::buildObjectClass()};

std::shared_ptr<JavaClass> JavaObject::getDescription(const std::string& package, const std::string& name, const std::shared_ptr<JavaClass>& superClass) {
    string javaPackage = package;
    while (javaPackage.find('_') != string::npos) javaPackage[javaPackage.find('_')] = '.';

    auto result = find_description(javaPackage, name);
    if(!result) {
        lock_guard<mutex> lock(classCreateLock);
        result = find_description(javaPackage, name);
        if(!result) {
            assert(!!superClass);
            result.reset(new JavaClass(javaPackage, name, superClass));
            lock_guard<mutex> list_lock(classListLock);
            classes.push_back(result);
        }
    }
    return result;
}

std::shared_ptr<JavaClass> JavaObject::find_description(const std::string &package, const std::string &name) {
    lock_guard<mutex> lock(classListLock);
    for (auto descriptor : classes) {
        if (descriptor->package.compare(package) == 0 && descriptor->name.compare(name) == 0) {
            return descriptor;
        }
    }
    return nullptr;
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

JavaObject::JavaObject(std::shared_ptr<void *> javaInstance) : javaInstance(javaInstance) {
}

JavaObject::~JavaObject() {
    this->javaInstance = nullptr;
}