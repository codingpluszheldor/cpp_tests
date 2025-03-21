/*
* Сборка с отладкой: g++ -g -std=c++20 main.cpp
* Сборка с оптимизацией: g++ -O2 -std=c++20 main.cpp
* Запуск: ./a.out
*/
#include <vector>
#include <thread>
#include <iostream>
#include <mutex>
#include <atomic>
#include <chrono>

constexpr int range = 10000000;
int shared_counter { 0 };
// std::atomic<int> shared_counter { 0 };
std::mutex counter_mutex;  // Мьютекс для защиты 

void inc_counter()
{
    for (int i = 0; i < range; ++i) {
        // Захватываем мьютекс перед увеличением счетчика
        std::lock_guard<std::mutex> lock(counter_mutex);
        shared_counter++;
    }
}

int main()
{
    std::vector<std::thread> threads;

    // Создаем четыре потока
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(std::thread(inc_counter));
    }
    
    using namespace std::chrono;
    auto start_time = high_resolution_clock::now();  // Начало замера времени

    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();  // Дожидаемся завершения каждого потока
    }

    auto end_time = high_resolution_clock::now();  // Конец замера времени

    // Теперь вектор потоков можно очистить
    threads.clear();

    std::cout << "Ожидаемое значение: " << 4 * range << std::endl;
    std::cout << "Реальное значение: "  << shared_counter << std::endl;

    // Вывод времени выполнения
    auto duration = duration_cast<milliseconds>(end_time - start_time).count();
    std::cout << "Время выполнения программы: " << duration << " миллисекунд" << std::endl;

    return 0;
}