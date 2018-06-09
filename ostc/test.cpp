//#include <string>
#include <sstream>
#include <iostream>

main()
{
    std::wstringstream SS;

    SS << L"Hello World!" << std::endl;
    std::wcout << SS.str();

    return 0;
}
