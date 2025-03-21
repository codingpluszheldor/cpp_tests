#include <memory>
#include <iostream>

template<typename T>
class SharedPtr
{
private:
    T* ptr;
    size_t* count; // Указатель на счетчик ссылок

public:
    // Конструктор
    explicit SharedPtr(T* p)
        : ptr(p),
          count(new size_t(1))
    {}

    // Конструктор копирования
    SharedPtr(const SharedPtr<T>& other)
        : ptr(other.ptr),
          count(other.count)
    {
        if (ptr != nullptr) {
            ++(*count); // Увеличение счетчика ссылок
        }
    }

    // Конструктор перемещения
    SharedPtr(SharedPtr<T>&& other) noexcept
        : ptr(std::move(other.ptr)),
          count(std::move(other.count))
    {
        other.ptr = nullptr;
        other.count = nullptr;
    }

    // Оператор присваивания копированием
    SharedPtr& operator=(const SharedPtr<T>& other)
    {
        if (this != &other) {
            if (ptr != nullptr && --(*count) == 0) { // Проверка на удаление последнего владельца
                delete ptr;
                delete count;
            }

            ptr = other.ptr;
            count = other.count;

            if (ptr != nullptr) {
                ++(*count); // Увеличение счетчика ссылок
            }
        }
        return *this;
    }

    // Оператор присваивания перемещением
    SharedPtr& operator=(SharedPtr<T>&& other) noexcept
    {
        if (this != &other) {
            if (ptr != nullptr && --(*count) == 0) { // Проверка на удаление последнего владельца
                delete ptr;
                delete count;
            }

            ptr = std::move(other.ptr);
            count = std::move(other.count);

            other.ptr = nullptr;
            other.count = nullptr;
        }
        return *this;
    }

    // Деструктор
    ~SharedPtr()
    {
        if (ptr != nullptr && --(*count) == 0) { // Если последний владелец
            delete ptr;
            delete count;
        }
    }

    // Разыменование указателя
    T& operator*() const
    {
        return *ptr;
    }

    // Доступ к указателю
    T* get() const
    {
        return ptr;
    }
};

int main()
{
    SharedPtr<int> sp1(new int(5)); // Создаем SharedPtr
    std::cout << "sp1 = " << *sp1.get() << std::endl; // Выводим значение через разыменование

    return 0;
}
