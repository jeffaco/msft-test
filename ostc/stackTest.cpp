#include <iostream>
#include <sstream>
#include <stdlib.h>

class A
{
public:
    A(int val)
    {
        std::cout << "Constructing class A with value: " << val << std::endl;

        m_data = val;
    }
    ~A() { std::cout << "Destructing class A with value: " << m_data << std::endl; }

    int getData() { return m_data; }

private:
    int m_data;
};


std::string aSub(int val)
{
    A bar(val);

    std::ostringstream myStream;
    myStream << bar.getData();

    // std::string myString = myStream.str();
    // return &myString[0];

    return &myStream.str()[0];
}

int bSub(const char *myStr)
{
    return atoi(myStr);
}

int main()
{
    A foo(42);

    std::cout << "Performing test: " << bSub(aSub(84).c_str()) << " " << bSub(aSub(85).c_str()) << " " << bSub(aSub(86).c_str()) << std::endl;

    std::cout << "Program exit, data = " << foo.getData() << std::endl;
}
