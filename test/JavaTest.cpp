#include <jni.h>
#include <iostream>
#include "../include/converter/TypeConverter.h"
#include "../include/signature/Signature.h"
#include "../include/ClassDefinitions.h"
#include "../include/JNIHelper.h"


DEFINE_JAVA_CLASS(dev_wolveringer_JNIHelper, TestClass, JNIHelper::JavaObject,
                  DEFINE_STATIC_METHODE(sayHelloWorld, void);
                  DEFINE_STATIC_METHODE(return11, uint64_t);
                  DEFINE_STATIC_METHODE(return12I, uint32_t);
                  DEFINE_STATIC_METHODE(buildSum, uint64_t, int, long, short, char);
                  DEFINE_STATIC_METHODE(printTestFieldValue, void);

                  DEFINE_STATIC_FIELD(testField1, int, testField1);

                  DEFINE_CONSTRUCTOR(init, int, int, int, int);
                  DEFINE_DEFAULT_CONSTRUCTOR(constructor);

                  DEFINE_FIELD(instanceTestField, int);
                  DEFINE_FIELD(instanceTestPtrField, void*);
                  DEFINE_FIELD(testString, std::string);
                  DEFINE_FIELD(clsInstance, dev_wolveringer_JNIHelper::TestClass);

                  DEFINE_METHODE(sayHello, void, int);
                  DEFINE_METHODE(printInstanceTestField, void);
                  DEFINE_METHODE(printTestString, void);

);

DEFINE_JAVA_CLASS(dev_wolveringer_JNIHelper, TestSubClass, dev_wolveringer_JNIHelper::TestClass,
                  DEFINE_DEFAULT_CONSTRUCTOR(constructor01);
                  DEFINE_METHODE(sayHello, void, int);
);

DEFINE_JAVA_CLASS(java_util, List, JNIHelper::JavaObject,
            DEFINE_DEFAULT_CONSTRUCTOR(init);

            DEFINE_METHODE(size, int);
            DEFINE_METHODE(isEmpty, bool);
            DEFINE_METHODE(add, bool, jobject);
            DEFINE_METHODE(add, bool, JNIHelper::JavaObject*);
            DEFINE_METHODE(add, bool, JNIHelper::JavaObject&);
);

DEFINE_JAVA_CLASS(java_util, ArrayList, java_util::List, )

PREDEFINE_JAVA_CLASS(dev_wolveringer_test_instance, BasicClass);
PREDEFINE_JAVA_CLASS(dev_wolveringer_test_instance, InformationHolder);

DEFINE_JAVA_CLASS(dev_wolveringer_test_instance, BasicClass, JNIHelper::JavaObject,
                  DEFINE_FIELD(handle, dev_wolveringer_test_instance::InformationHolder);

                  DEFINE_FIELD(name, std::string);
                  DEFINE_FIELD(value, std::string);

                  DEFINE_CONSTRUCTOR(init, dev_wolveringer_test_instance::InformationHolder&, std::string, std::string);
);

DEFINE_JAVA_CLASS(dev_wolveringer_test_instance, InformationHolder, JNIHelper::JavaObject,
                  DEFINE_DEFAULT_CONSTRUCTOR(init);

                  DEFINE_FIELD(basicInfo, dev_wolveringer_test_instance::BasicClass);
                  DEFINE_FIELD(infos, java_util::ArrayList);

                  DEFINE_METHODE(print, void);
);

DEFINE_JAVA_CLASS(dev_wolveringer_test_instance, TestArray, JNIHelper::JavaObject,
                  DEFINE_STATIC_FIELD(elements, jintArray, elements);

                  DEFINE_STATIC_METHODE(print, void);
);

using namespace std;

void runHelloMethods(dev_wolveringer_JNIHelper::TestClass cls){
    cls.sayHello(2222);
}

extern "C" {
    jint JNI_OnLoad(JavaVM *vm, void *reserved) {
        printf("initialising EasyJNITest libary!\n");
        JNIHelper::Debug::setDebugMask(JNIHelper::Debug::Type::NONE);
        JNIHelper::initializeLibrary(vm);

        /*
        auto env = EasyJNI::Utils::getJNIEnvAttach();
        if(!env){
            printf("Failed to load EasyJNI env attachment!\n");
            return 0;
        }

        dev_wolveringer_EasyJNI_Main::runNativeTests()->bindNative((uintptr_t) &testMethode);
        dev_wolveringer_EasyJNI_Main::crashVMFunction()->bindNative(&crashVM);
         */

        /*
        dev_wolveringer_JNIHelper::TestClass::sayHelloWorld();
        cout << "1+2+3+4 -> " << dev_wolveringer_JNIHelper::TestClass::buildSum(1, 2, 3, 4) << "/" << (1+2+3+4) << endl;
        cout << "11 -> " << dev_wolveringer_JNIHelper::TestClass::return11() << endl;
        cout << "12 -> " << dev_wolveringer_JNIHelper::TestClass::return12I() << endl;
        printf("initialising EasyJNITest libary successfully!\n");
        cout << "Type: " << typeid(JNIHelper::CppToJniConverter<dev_wolveringer_JNIHelper::TestClass>::TargetType).name() << endl;
         */
        /*
        dev_wolveringer_JNIHelper::TestSubClass classInstance;
        cout << "Class this: " << &classInstance << endl;
        dev_wolveringer_JNIHelper::TestClass superClass = (dev_wolveringer_JNIHelper::TestClass) classInstance;
        classInstance.constructor01(22);

        classInstance.sayHello(22);
        superClass.sayHello(33);
        runHelloMethods(classInstance);
        */


        //dev_wolveringer_JNIHelper::TestClass classInstance;
        //classInstance.init(2, 3, 4, 5);

        /*
        auto value = dev_wolveringer_JNIHelper::TestClass::testField1();
        cout << "Field value: " <<  (int) value << endl;
        value = 1212;
        dev_wolveringer_JNIHelper::TestClass::printTestFieldValue();
        cout << "Field value: " << (int) value << endl;

        dev_wolveringer_JNIHelper::TestClass instance;
        instance.constructor();

        cout << "Instance field: " << (int) instance.instanceTestField << endl;
        instance.instanceTestField = 22;
        instance.instanceTestPtrField = nullptr;
        instance.printInstanceTestField();
*/


        dev_wolveringer_JNIHelper::TestClass instance;
        instance.constructor();

        instance.printTestString();
        cout << "Test string: " << (string) instance.testString << endl;

        instance.testString = "Hello world #121212";

        instance.printTestString();
        cout << "Test string: " << (string) instance.testString << endl;

        instance.clsInstance = instance;



        dev_wolveringer_test_instance::InformationHolder holder;
        holder.init();
        holder.print();

        dev_wolveringer_test_instance::BasicClass basicInfo;
        basicInfo.init(holder, "Header", "Line value");
        holder.basicInfo = basicInfo;

        dev_wolveringer_test_instance::BasicClass line1 = dev_wolveringer_test_instance::BasicClass();
        line1.init(holder, "line 1", "line 1 value");
        holder.infos.getValue().add(line1);

        holder.print();


        dev_wolveringer_test_instance::TestArray::print();

        //vector<jint> elms = dev_wolveringer_test_instance::TestArray::elements();
        dev_wolveringer_test_instance::TestArray::elements() = nullptr;
        jintArray arr = dev_wolveringer_test_instance::TestArray::elements();
        auto env = JNIHelper::getAttachedEnv();

        /*
        jint elm = 11;
        env->SetIntArrayRegion(arr, 0, 1, &elm);
        */

        dev_wolveringer_test_instance::TestArray::print();
        return JNI_VERSION_1_6;
    }

    void JNI_OnUnload(JavaVM *vm, void *reserved){
        JNIHelper::finalizeLibrary(vm);
    }
}