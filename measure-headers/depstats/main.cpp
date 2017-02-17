#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

/* Key=dependency Value=Count */
typedef map<string, unsigned long> Map;
typedef pair<string, unsigned long> Pair;

static Map _map;

/* Load a file into memory */
static int _LoadFile(
    const char* path, 
    vector<char>& data)
{
    FILE* is;

    /* Open the file */
    if (!(is = fopen(path, "rb")))
        return -1;

    /* Read file into memory */
    for (;;)
    {
        char buf[4096];
        size_t n;

        n = fread(buf, 1, sizeof(buf), is);

        if (n <= 0)
            break;

        data.insert(data.end(), buf, buf + n);
    }

    /* Close the file */
    fclose(is);

    data.push_back('\0');

    /* Success! */
    return 0;
}

static bool _GetNextLine(
    const char*& p,
    string& line)
{
    line.clear();

    while (isspace(*p))
        p++;

    if (*p == '\0')
        return false;

    /* Skip comment lines */
    if (p[0] == '#')
    {
        while (*p && *p != '\n')
            p++;

        while (isspace(*p))
            p++;
    }

    while (*p && *p != '\n')
    {
        if (p[0] == '\\' && p[1] == '\n')
        {
            p += 2;
        }
        else
        {
            line.push_back(*p);
            p++;
        }
    }

    return true;
}

static bool _ParseNextLine(
    const string& line,
    string& target,
    vector<string>& deps)
{
    const char* p = line.c_str();

    target.clear();
    deps.clear();

    // Skip whitespace:
    while (isspace(*p))
        p++;

    // Get the target:
    {
        while (*p && *p != ':' && !isspace(*p))
        {
            target.push_back(*p);
            p++;
        }
    }

    // Expect ':'
    if (*p++ != ':')
        return false;

    // For each dependency:
    while (*p)
    {
        string dep;

        // Skip whitespace:
        while (isspace(*p))
            p++;

        // Skip dependency characters:
        while (*p && !isspace(*p))
            dep.push_back(*p++);

#if 0
        // Skip system headers:
        if (dep[0] != '/')
#endif
            deps.push_back(dep);

        // Skip whitespace:
        while (isspace(*p))
            p++;
    }

    return true;
}

/* Process a single depends file, updating the stats */
static int _ProcessDependsFile(
    const char* path)
{
    vector<char> data;

    if (_LoadFile(path, data) != 0)
        return -1;

    const char* p = &data[0];
    string line;

    while (_GetNextLine(p, line))
    {
        string target;
        vector<string> deps;

        if (line.size() == 0)
            continue;

        if (!_ParseNextLine(line, target, deps))
        {
            return -1;
        }

        for (size_t i = 0; i < deps.size(); i++)
        {
            const string& dep = deps[i];

            if (_map.find(dep) == _map.end())
            {
                _map.insert(Pair(dep, 1));
            }
            else
            {
                _map[dep]++;
            }
        }
    }

    return 0;
}


struct Greater 
{
    bool operator() (const Pair& x, const Pair& y) const 
    {
        return x.second > y.second;
    }
};

int main(int argc, const char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (_ProcessDependsFile(argv[i]) != 0)
        {
            fprintf(stderr, "%s: failed: %s\n", argv[0], argv[i]);
            exit(1);
        }
    }

    Map::const_iterator p = _map.begin();
    Map::const_iterator end = _map.end();

    vector<Pair> pairs;

    while (p != end)
        pairs.push_back(*p++);

    sort(pairs.begin(), pairs.end(), Greater());

    for (size_t i = 0; i < pairs.size(); i++)
    {
        string dep = pairs[i].first;
        unsigned long count = pairs[i].second;

        printf("%08lu %s\n", count, dep.c_str());
        p++;
    }

    return 0;
}
