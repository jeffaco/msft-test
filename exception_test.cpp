#include <iostream>
#include <cstdlib>


wchar_t const DERIVED_TEXT[] = L"Derived";

class Base
{
public:
    Base () {}
    virtual ~Base () {}

    virtual wchar_t const* what () = 0;
};

class Derived : public Base
{
public:
    Derived () {}
    ~Derived () {}

    wchar_t const* what () {return DERIVED_TEXT;}
};

void bar ()
{
    //throw Derived ();
    throw new Derived ();
    //throw EXIT_FAILURE;
}


class ExceptionSentinel
{
public:
    ExceptionSentinel () : m_Ready (false) {}
    ~ExceptionSentinel () { if (!m_Ready) throw EXIT_FAILURE; }
    void clear () { m_Ready = true; }
private:
    bool m_Ready;
};


int main ()
{
    ExceptionSentinel sentinel;
    std::wcout << L"Hello, World!" << std::endl;

    try
    {
        bar ();
        sentinel.clear ();
    }
    // the catch (Base) version doesn't work because you can not instantiate
    // Base as a class due to its pure virtual method what.
    //catch (Base base)
    //{
    //    std::wcout << L"Caught: " << base.what () << std::endl;
    //}
    // you can however return a pointer to a Base object, but you need to create
    // the exception on the heap, which is not ideal in an out of memory
    // situation.  if you did something like that you could also use smart
    // pointers for clean up.
    catch (Base* pEx)
    {
        std::wcout << L"Caught pointer to: " << pEx->what () << std::endl;
        delete pEx;
        sentinel.clear ();
    }
    catch (...)
    {
        std::wcout << L"Caught something else." << std::endl;
        // this case doesn't have sentinel.clear and will core dump.
        // change void bar () to throw a different exception to try.
    }

    return EXIT_SUCCESS;
}
