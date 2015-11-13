#include <iostream>

class UserClass;

namespace SCXCore
{
    class NameResolverInternal {
        friend class ::UserClass;

    private:
        int foo;
    };
}

class UserClass
{
public:
    void TestFun()
    {
        SCXCore::NameResolverInternal nri;

        nri.foo = 1;
        std::cout << "nri.foo has been reassigned ..." << std::endl;
    }
};

int main()
{
    UserClass a;
    a.TestFun();
}
