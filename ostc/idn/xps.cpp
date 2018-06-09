//
// Compile with a command like:
//   g++ -o xps xps.cpp -lcidn
//

#include <iostream>
#include <locale.h>
// #include "idna.h"

extern "C" int idna_to_ascii_lz(const char *, char **, int);

using namespace std; 

int main (int argc, char ** argv)
{
    if(argc < 2)
        return 1; 

    setlocale(LC_ALL, "");

    char * s = 0; 
    int nRet = idna_to_ascii_lz(argv[1], &s, 0);
    cout << "In: " << argv[1] << ", Result: " <<  (s ? s : "(null)") << " Returned:" << nRet << endl; 
    
    return nRet;
}
