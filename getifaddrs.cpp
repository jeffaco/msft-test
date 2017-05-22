// If you want to use "nulptr" (below), then compile with:
//
//   g++ -std=c++11 -o getifaddrs getifaddrs.cpp
//
// Otherwise, the -std=c++11 qualifier is not needed.

#include <sys/types.h>
#include <ifaddrs.h>

#include <string>
#include <set>
#include <iostream>

int main()
{
    struct ifaddrs* ifaddr = NULL; // nullptr;
    if (getifaddrs(&ifaddr) == -1) {
        std::cout << "Error: getifaddrs failed" << std::endl;
        return 1;
    }

    std::set<std::string> names;
    for (struct ifaddrs* ifa = ifaddr; ifa != NULL /* nullptr */; ifa = ifa->ifa_next) {
        if (ifa->ifa_name != NULL /* nullptr */) {
            std::cout << "Interface: " << ifa->ifa_name << std::endl;
        }
        std::cout << "Netmask: " << ifa->ifa_netmask << std::endl;
    }

    freeifaddrs(ifaddr);
    return 0;
}
