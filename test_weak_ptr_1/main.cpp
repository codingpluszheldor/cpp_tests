#include <iostream>
#include <memory>

#define USE_WEAK 1

using Id = uint64_t;
enum class TypeId : Id {
    Uint,
    Float,
    String,
    Vector
};

class Parent;

class Child
{
public:    
#ifndef USE_WEAK 
    std::shared_ptr<Parent> _parent;
#else
    std::weak_ptr<Parent> _parent;
#endif    

    Child() {
        std::cout << "Child()" << std::endl;
    }

    ~Child() {
        std::cout << "~Child()" << std::endl;
    }
};

class Parent
{
public:    
    std::shared_ptr<Child> _child;

    Parent() {
        std::cout << "Parent()" << std::endl;
    }

    ~Parent() {
        std::cout << "~Parent()" << std::endl;
    }

    void foo() {}
};

int main()
{
    auto parent = std::make_shared<Parent>(); // Счётчик ссылок у parent равен 1
    auto child = std::make_shared<Child>(); // Счётчик ссылок у child равен 1

    // Это классическая перекрёстная ссылка с утечкой памяти
    parent->_child = child; // Счётчик ссылок у child равен 2
    child->_parent = parent; // Счётчик ссылок у parent равен 2


    // Так делать нельзя
    std::weak_ptr<Parent> pw = child->_parent;
/*    pw->foo();
    if (pw != nullptr) {

    } 
*/    
   
    return 0;
}