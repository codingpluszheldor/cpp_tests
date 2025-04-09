/*
* Сборка с отладкой: g++ -g -std=c++20 main.cpp
* Сборка с оптимизацией: g++ -O2 -std=c++20 main.cpp
* Запуск: ./a.out
*/
#include <iostream>
#include <vector>
#include <list>

template <class T>
class allocator
{
public:
    using value_type = T;

    T* allocate(size_t n)
    {
        std::cout << "Items: " << n << " allocate: " << n * sizeof(T) << " bytes\n";
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* ptr, size_t n)
    {
        std::cout << " Deallocate: " << n * sizeof(T) << " bytes\n";
        ::operator delete(ptr);
    }
};

template <class T, class U>
bool operator==(allocator<T> const&, allocator<U> const&) noexcept
{
    return true;
}

int main()
{
    // std::vector<int, allocator<int>> vec;
    // std::vector<int, allocator<int>> vec2;
    // vec.reserve(100);
    // vec2 = vec;

    std::list<int, allocator<int>> list;
    list.push_back(1);

    return 0;
}