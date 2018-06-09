#include <iostream>

void GetLock(const int allowRecursion);
void GetLock(void) { GetLock(false); }
void GetLock(const std::wstring& r, bool allowRecursion);
void GetLock(const std::wstring& r) { GetLock(r, true); }

void GetLock(const int allowRecursion)
{
    std::cout << "Value is: " << allowRecursion << std::endl;
}

void GetLock(const std::wstring& r, bool allowRecursion)
{
    std::wcout << L"Value is: " << allowRecursion << L" from lock: " << r << std::endl;
}

int main()
{
    std::ios::sync_with_stdio(false);

//    GetLock("ABCD");
    GetLock(L"ABCD");
    GetLock();
    GetLock(L"NamedLock", true);
    GetLock(5);

    return 0;
}
