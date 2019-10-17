#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"

// Macro for printf debugging
#ifdef DEBUG
#define DEBUG_PRINT(x) x 
#else 
#define DEBUG_PRINT(x)
#endif

/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template <class T, int size = 0>
class Pointer{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T> > refContainer;
    // addr points to the allocated memory to which
    // this Pointer pointer currently points.
    T *addr;
    /*  isArray is true if this Pointer points
        to an allocated array. It is false
        otherwise.
    */
    bool isArray;
    // true if pointing to array
    // If this Pointer is pointing to an allocated
    // array, then arraySize contains its size.
    unsigned arraySize; // size of the array
    static bool first; // true when first Pointer is created
    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);
public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;
    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments
    // this is why constructor is designed like this:
    Pointer(){
        Pointer(NULL);
    }
    Pointer(T*);
    // Copy constructor.
    Pointer(const Pointer &);
    // Destructor for Pointer.
    ~Pointer();
    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect();
    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);
    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*(){
        return *addr;
    }
    // Return the address being pointed to.
    T *operator->() { return addr; }
    // Return a reference to the object at the
    // index specified by i.
    T &operator[](int i){ return addr[i];}
    // Conversion function to T *.
    operator T *() { return addr; }
    // Return an Iter to the start of the allocated memory.
    Iter<T> begin(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }
    // Return an Iter to one past the end of an allocated array.
    Iter<T> end(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize() { return refContainer.size(); }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size>
Pointer<T,size>::Pointer(T *t){
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " In copy constructor Pointer(T *t)\n");
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;

    // TODO: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(t);
    // increment ref count, if t is in list
    if (p != refContainer.end()) {
        p->refcount++;
    } else {
        // add a new entry to the list  
        PtrDetails<T> ptrDetails(t, size);
        refContainer.emplace_back(ptrDetails);
    }
    addr = t;
    arraySize = size;
    // decide whether it is an array
    if (arraySize > 0) isArray = true;
    else isArray = false;
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " Mem address: " << t << "\n");
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " refcount: " << p->refcount << "\n");
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " Array size: " << arraySize << "\n");
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " Is Array: " << isArray << "\n");
    DEBUG_PRINT(std::cout << "------------\n");
}
// Copy constructor.
template< class T, int size>
Pointer<T,size>::Pointer(const Pointer &ob){

    // TODO: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " In copy constructor Pointer(const Pointer &ob)\n");
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    // increment ref count
    p->refcount++;
    addr = ob.addr;
    arraySize = ob.arraySize;
    // decide whether it is an array
    if (arraySize > 0) isArray = true;
    else isArray = false;

    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " Array size: " << arraySize << "\n");
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " Is Array: " << isArray << "\n");
    DEBUG_PRINT(std::cout << "------------\n");
}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer(){

    // TODO: Implement Pointer destructor
    // Lab: New and Delete Project Lab
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " In ~Pointer()\n");
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " Mem address: " << addr << "\n");
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " refcount before: " << p->refcount << "\n");
    if(p->refcount > 0) p->refcount--;
    
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " refcount after: " << p->refcount << "\n");
    DEBUG_PRINT(std::cout << "------------\n");
    collect();
}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect(){

    // TODO: Implement collect function
    // LAB: New and Delete Project Lab
    // Note: collect() will be called in the destructor
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " In collect()\n");
    bool memfreed = false;
    typename std::list<PtrDetails<T>>::iterator p;
    do
    {
        // Scan refContainer looking for unreferenced pointers.
        for (p = refContainer.begin(); p != refContainer.end(); p++)
        {
            // TODO: Implement collect()
            // Remove unused entry from refContainer.
            DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " Mem address to remove: " << p->memPtr << "\n");
            refContainer.remove(*p);
            // Free memory unless the Pointer is null.
            if (p->memPtr) {
                if (p->isArray) delete[] p->memPtr;
                else delete p->memPtr;
            }
            memfreed = true;
            // Restart the search.
            break;
        }
    } while (p != refContainer.end());
    DEBUG_PRINT(std::cout << "------------\n");
    return memfreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t){

    // TODO: Implement operator==
    // LAB: Smart Pointer Project Lab
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " In operator=(T *t)\n");
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " Mem address: " << t << "\n");
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    // First, decrement the reference count
    // for the memory currently being pointed to.
    p->refcount--;

    // Then, increment the reference count of
    // the new address.
    p = findPtrInfo(t);
    // increment ref count, if t is in list
    if (p != refContainer.end()) {
        p->refcount++;
    } else {
        // add a new entry to the list  
        PtrDetails<T> ptrDetails(t, size);
        refContainer.emplace_back(ptrDetails);
    }
    // store the address.
    addr = t;
    DEBUG_PRINT(std::cout << "------------\n");
    return t;
}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){

    // TODO: Implement operator==
    // LAB: Smart Pointer Project Lab
    DEBUG_PRINT(std::cout << __FILE__ << ":" << __LINE__ << " In operator=(Pointer &rv)\n");
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    // First, decrement the reference count
    // for the memory currently being pointed to.
    p->refcount--;

    // Then, increment the reference count of
    // the new address.
    p = findPtrInfo(rv.addr);
    p->refcount++;
    // store the address.
    addr = rv.addr;
    return rv;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
             << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}
