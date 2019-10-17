#include "gc_pointer.h"
#include "LeakTester.h"

int main()
{
    // Test operator=(T *t)
    Pointer<int> p = new int(19);
    p = new int(21);
    p = new int(28);

    // Test operator=(T &rv)
    Pointer<int> p3;
    p3 = p;

    // Test Copy Constructor
    Pointer<int> p1 = new int(19);
    Pointer<int> p2(p1);

    // Test Array creation
    Pointer<int, 3> arr_p1 = new int[3];
    Pointer<int, 3> arr_p2(arr_p1);

    // Test Copy Constructor for Array
    Pointer<double, 5>  arr_p3 = new double[5];
    Pointer<double, 5> arr_p4(arr_p3);

    return 0;
}
