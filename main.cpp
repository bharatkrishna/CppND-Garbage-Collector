#include "gc_pointer.h"
#include "LeakTester.h"

int main()
{
    // Test operator=(T *t)
    Pointer<int> p = new int(19);
    p = new int(21);
    p = new int(28);

    // Test Copy Constructor
    Pointer<int> p1 = new int(19);
    Pointer<int> p2(p1);

    // Test operator=(T &rv)
    Pointer<int> p3;
    p3 = p2;

    return 0;
}
