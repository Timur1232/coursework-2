#include <iostream>
#include <utils/ArenaAllocator.h>

int main()
{
    CW::ArenaAllocator<int, 15> allocator_int;
    int* a = allocator_int.allocate(6);
    int* arr = allocator_int.allocate(10);

    for (int i = 0; i < 6; ++i)
    {
        a[i] = i;
    }
    for (int i = 0; i < 10; ++i)
    {
        arr[i] = i + 6;
    }

    std::cout << "a: " << std::endl;
    for (int i = 0; i < 6; ++i)
    {
        std::cout << a[i] << ' ';
    }
    std::cout << std::endl << std::endl;
    std::cout << "arr: " << std::endl;
    for (int i = 0; i < 10; ++i)
    {
        std::cout << arr[i] << ' ';
    }
    std::cout << std::endl << std::endl;

}