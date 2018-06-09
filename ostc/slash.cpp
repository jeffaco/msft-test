#include <iostream>
#include <string>

int main()
{
    size_t index;
    std::string orig("\\\\\\\\\\this\\\\is\\some\\path");

    // Convert all '\\' bytes to '/' bytes
    while ((index = orig.find_first_of('\\')) != std::string::npos)
    {
        orig[index] = '/';
    }

    // Compress multiple '/' sequences (i.e. '///') into a single forward slash
    while ((index = orig.find("//")) != std::string::npos)
    {
        orig.replace(index, 2, "/");
    }

    std::cout << orig << std::endl;
}
