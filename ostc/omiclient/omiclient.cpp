#include <omiclient/client.h>

#include <iostream>

using namespace std;
using namespace mi;

bool Connect(Client& c)
{
    const unsigned long long timeout = 30000000;

    String locator("/var/opt/microsoft/omi/run/omiserver.sock");
    String username;
    String password;

    if (!c.Connect(locator, username, password, timeout))
    {
        cerr << "Failure connecting to OMI server (connect failure)" << endl;
        return false;
    }

    return true;
}

bool Enumerate(Client& c)
{
    const String nameSpace = "root/omi";
    const String className = "OMI_Identify";
    const bool deepInheritance = true;
    const Uint64 timeout = 2000000;
    Array<DInstance> instances;
    const String queryLanguage;
    const String queryExpression;
    MI_Result result;

    if (!c.EnumerateInstances(nameSpace, className, deepInheritance, timeout, instances, queryLanguage, queryExpression, result))
    {
        cerr << "Failure executing EnumerateInstances in client" << endl;
        return false;
    }

    if (result != MI_RESULT_OK)
    {
        cerr << "EnumerateInstances returned failure, result: " << result << endl;
        return false;
    }

    for (Uint32 i = 0; i < instances.GetSize(); i++)
        instances[i].Print();

    return true;
}

int main()
{
    Client c;

    Connect(c);
    Enumerate(c);

    string buffer;
    wcout << "Waiting for input ...";
    getline(cin, buffer);
    wcout << "Enumerating again ..." << endl;

    Enumerate(c);

    return 0;
}
