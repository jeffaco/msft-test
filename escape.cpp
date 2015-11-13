#include <iostream>
#include <sstream>
#include <string>

using namespace std;

string quoted(string& in)
{
    string::iterator it;
    ostringstream outStr;

    for (it = in.begin(); it < in.end(); it++ )
    {
        switch (*it)
        {
            // Quote any quote characters (\) or quotes (") themselves
            case '\\':
            case '"':
                outStr << "\\";
                break;
        }

        outStr << *it;
    }

    return outStr.str();
}

int main()
{
    string input;
    cout << "Please enter line to quote: ";
    getline( cin, input );
    cout << "Line received: " << input << endl;
    cout << "Quoted result: " << quoted(input) << endl;

    return 0;
}
