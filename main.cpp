#include <iostream>
#include "include/signature/Signature.h"

using namespace std;
using namespace JNIHelper;

class A {
public:

    struct {
        void operator()(){

        }

    } test;
};

int main() {
    /*
    Test test;
    test.f1 = 22;
    int f1val = test.f1;

    int i = test;
    cout << "I: " << BaseClass::bstring << i << endl;
     */
    __asm__("nop");
    __asm__("push %rax");
    __asm__("nop");
    A a;
    a.test();
    return 0;
}