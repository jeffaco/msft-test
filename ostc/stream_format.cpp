#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

int main()
{
    wstringstream foo;

    foo << std::hex << std::setw(2) << std::setfill(L'0') << 2
        << L":" << std::hex << std::setw(2) << std::setfill(L'0') << 4
        << L":" << 6 << L":" << 8 << L":" << 10 << L":" << 12 << L":" << 14;

    wcout << "Result: " << foo.str() << std::endl;

    return 0;
}
