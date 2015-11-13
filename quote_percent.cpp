#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

string QuotePercent(const string& s)
{
    string s2(s);
    size_t cursor = s2.find('%', 0);
                                       
    const unsigned int cReplaceLen = 1;
    //assert(string("%").length() == cReplaceLen);
                                                               
    while (cursor != s2.npos)
    {
        s2.replace(cursor, cReplaceLen, "%%");
        cursor = s2.find('%', cursor + cReplaceLen +1); // one beyond just replaced
    }
    return s2;
}

main()
{
    string foo("Now %% is % the %%% time % % %");
    string res;
    char buffer[256];

    res = QuotePercent(foo);
    sprintf(buffer, res.c_str());

    cout << "Source: " << foo << endl << "Result: " << res << endl
         << "Final:  " << buffer << endl;

    return 0;
}
