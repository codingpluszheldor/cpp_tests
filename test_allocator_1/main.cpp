/*
* Сборка с отладкой: g++ -g -std=c++20 main.cpp
* Сборка с оптимизацией: g++ -O2 -std=c++20 main.cpp
* Запуск: ./a.out
*/
#include <chrono>
#include <list>
#include <map>
#include <iostream>
#include <boost/pool/pool_alloc.hpp>

int main()
{
    const int N = 10'000'000;
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        {
            std::list<int> list;
            // std::list<int, boost::fast_pool_allocator<int>> list;
            // std::list<int, boost::fast_pool_allocator<int,
            // boost::default_user_allocator_new_delete,
            // boost::details::pool::null_mutex>> list;

            std::map<int, int> map;
            // std::map<int, int, std::less<>,
            // boost::fast_pool_allocator<std::pair<const int, int>,
            // boost::default_user_allocator_new_delete,
            // boost::details::pool::null_mutex>> map;

            auto hint = map.begin();
            for (int i = 0; i < N; i++) {
                // list.emplace_back(i);
                // map.insert(std::make_pair(i, i));
                hint = map.insert(hint, std::make_pair(i, i));
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> ms = std::chrono::duration<double, std::milli>(end - start);
        std::cout << "Time: " << ms.count() * 1000.0 << " ms" << std::endl;
        auto diff_in_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch() - start.time_since_epoch());
        std::cout << "Time: " << diff_in_microseconds.count() / 1000.0 << " ms" << std::endl;
    }
}