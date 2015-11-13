#include <iostream>

class ctor_test
{
public:
    ctor_test() { std::cout << "Constructor says howdy!" << std::endl; }
};

ctor_test foo;


int main()
{
    std::cout << "Main says: Good Day!" << std::endl;

    return 0;
}
