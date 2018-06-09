/*
 * Build instructions:
 *
 * Linux:       g++ -o gethostbyname gethostbyname.cpp
 * SunOS:       CC -o gethostbyname gethostbyname.cpp -lnsl
 */

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

    /**
       Use sysconf() to get the maximum buffer size of machine names.

       \returns The system returned size if available, otherwise 257;

       sysconf(_SC_HOST_NAME_MAX) returns the max size of the host string
       minus 1 byte for the null byte.

       Docs across Linux, Solaris, and HPUX say that the maximum buffer
       size is 256 bytes + 1. 
    */
    size_t GetNameBufSize()
    {
        size_t size = 257;
#if defined(sun) && (PF_MAJOR==5) && (PF_MINOR<=9)
        // WI 7933: SC_HOST_NAME_MAX does not exist on Solaris 8
        // Since documentation indicates 256+1 (according to comment) I think it is 
        // OK to just skip the call to sysconf since there os no good alternative. 
#else
        long sysSize = sysconf(_SC_HOST_NAME_MAX);
        if((-1 != sysSize) && (size < (size_t)sysSize ))
        {
            size = sysSize + 1;
        }
#endif
        return size;
    }

#if defined(linux)

    /**
       Get the DNS entry for \a name.
       \param name Name to lookup.
       \returns The name found in DNS or the empty string.
    */
    std::string GetHostByName(const std::string & name)
    {
        const size_t bufsize = 1024;
        char  abuf[1024];
        struct hostent h;
        struct hostent * ph = 0;
        int herrno;
        int rc = gethostbyname_r(name.c_str(), &h, &abuf[0], bufsize,
                                 &ph, &herrno);
        if((0 == rc) && (0 != ph)) 
        {
            return h.h_name;
        }
        else if(ERANGE == rc)
        {
            std::cerr << "Buffer size was too small for the name." << std::endl;
        }
        return "";
    }


#elif defined(sun)

    /**
       Get the DNS entry for \a name.

       \param name Name to lookup.
       \returns The name found in DNS or the empty string.
    */
    std::string GetHostByName(const std::string & name)
    {
        const size_t bufsize = GetNameBufSize();
        std::vector<char> abuf; 

        // length must be positive number
        assert ( bufsize > 0 );

        abuf.resize( bufsize + 1, 0 ); // just to make sure string has '\0' at the end
        
        struct hostent h;
        int herrno;
        struct hostent *ph = gethostbyname_r(name.c_str(),
                                             &h,
                                             &abuf[0],
                                             bufsize,
                                             &herrno);
        if(0 != ph)
        {
            return h.h_name;
        }
        return "";
    }


#elif defined(hpux) || defined(macos)

    /**
       Get the domain name using gethostbyname() under HPUX.

       According to the man page, the non _r version provides
       thread support.
    */
    string GetHostByName(const string & name)
    {
        struct hostent *ph = gethostbyname(name.c_str());
        if(0 != ph)
        {
            return ph->h_name;
        }
        return "";
    }

#elif defined(aix)

    /**
       Get the DNS entry for \a name.

       \param name Name to lookup.
       \returns The name found in DNS or the empty string.
    */
    std::string GetHostByName(const std::string & name)
    {
        struct hostent h;
        struct hostent_data h_data;
        int res = gethostbyname_r(name.c_str(),
                                  &h,
                                  &h_data);
        if(0 == res)
        {
            return h.h_name;
        }
        return "";
    }


#else
#  error GetHostByName() was not implemented for this platform.
#endif

main(int argc, char *argv[])
{
    if (argc == 2) {
        std::cout << "Lookup up host name: " << argv[1] << std::endl;
        std::string host = GetHostByName(argv[1]);
        std::cout << "  Result: " << host << std::endl;
    }
    else {
        std::cerr << "Incorrect argument count: " << argc << std::endl;
        return -1;
    }

    return 0;
}
