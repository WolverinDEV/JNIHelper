#include <iostream>
#include "include/signature/Signature.h"
#include "include/ClassDefinitions.h"
#include "include/MethodeInfo.h"
#include "include/converter/TypeConverter.h"
#include <type_traits>
#include <vector>

using namespace std;
using namespace JNIHelper;

template <typename Arg1>
static void invoke(Arg1, double){
    __asm__ ("\tmovq %0, %%rax;" ::"r"((long) MethodeInfoBase<void, Arg1, double>::expendArgumentsS));
    __asm__ ("\tcall %rax");
    register vector<jvalue>* result;
    __asm__("\rmovq %%rax, %0" : "=r"(result));
    vector<jvalue> arguments = vector<jvalue>(*result);
    delete result;

    cout << "Res: " << arguments.size() << endl;
    //cout << "Result: " << result->size() << endl;
    //generalMethodeFn fn = MethodeInfoBase<void, int, double>::expendArguments;
    //fn();
    ;
    /*
    buildFn()();
    */
}

class Field {
    public:
        Field& operator=(int val){

            return *this;
        }

        template <typename T>
        operator T(){
            return 22;
        }
};

class Test {
    public:
        template <typename T>
        operator T(){
            return 22;
        }

        Field f1;
};

int main() {
    /*
    Test test;
    test.f1 = 22;
    int f1val = test.f1;

    int i = test;
    cout << "I: " << BaseClass::bstring << i << endl;
     */

    int var = 22;

    cout << "Var: " << var << endl;

    var = 33;

    if(var == 33) return 1;
    if(true) return 0;
    std::cout << "Hello, World!" << std::endl;
    std::cout << "-> " << Signature::TypeSignature<double>::signature() << endl;
    std::cout << "-> " << Signature::TypeSignature<int>::signature() << endl;
    std::cout << "-> " << Signature::TypeSignature<void>::signature() << endl;
    std::cout << "-> " << Signature::MethodeSignature<int, double, jobject>::signature() << endl;


    dev_wolveringer::Test::testMethode();
    dev_wolveringer::Test::testMethode(1);
    dev_wolveringer::Test::testMethode(1, nullptr);
    dev_wolveringer::Test::getClass()->findMethode<void, double, void*>("testMethode")->signature();

    dev_wolveringer::Test instance;
    instance.instanceTestMethode(-1, 22);
    cout << &instance << endl;

    std::cout << dev_wolveringer::Test::getClass()->name << endl;
    std::cout << dev_wolveringer::Test1::getClass()->getSuperClass()->name << endl;

    dev_wolveringer::Test::getClass()->registerMethode<void>("test", false);
    dev_wolveringer::Test::getClass()->registerMethode<int>("test", false);

    cout << "void -> " << dev_wolveringer::Test::getClass()->findMethode<bool>("test") << endl;
    cout << "void -> " << dev_wolveringer::Test::getClass()->findMethode<void>("test") << endl;
    cout << "void -> " << dev_wolveringer::Test::getClass()->findMethode<int>("test") << endl;

    cout << "dev_wolveringer::Test -> Methods: " << endl;
    for(auto m : dev_wolveringer::Test::getClass()->getMethods())
        cout << " -> " << m->name() << m->signature() << " (Static: " << m->isStatic() << ")\n";
    return 0;
}