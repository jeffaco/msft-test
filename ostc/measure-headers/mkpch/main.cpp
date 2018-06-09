#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

typedef pair<size_t, string> Pair;
typedef vector<Pair> Pairs;

bool HandleNext(
    const Pairs& pairs, 
    size_t& pos,
    string& path)
{
    if (pos >= pairs.size())
        return false;

    size_t level = pairs[pos].first;
    path = pairs[pos].second;

    for (pos++; pos < pairs.size(); pos++)
    {
        if (pairs[pos].first == level)
            break;
    }

    return true;
}

bool contains(const vector<string>& v, const string& s)
{
    for (size_t i = 0; i < v.size(); i++)
    {
        if (v[i] == s)
            return true;
    }

    return false;
}

int main(int argc, const char*  argv[])
{
    string line;
    ifstream is;
    Pairs pairs;
    vector<string> prefixes;
    vector<string> matches;

    // Check arguments:
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " filename path..." << endl;
        exit(1);
    }

    for (int i = 2; i < argc; i++)
        prefixes.push_back(argv[i]);

    // Open the file:
    is.open(argv[1]);
    if (!is)
    {
        cerr << argv[0] << ": failed to open file" << endl;
        exit(1);
    }

    // Read line by line:
    while (getline(is, line))
    {
        if (line[0] != '.')
            continue;

        size_t pos = line.find(' ');

        if (pos == string::npos)
            continue;

        string lhs = line.substr(0, pos);
        string rhs = line.substr(pos + 1);
        pairs.push_back(Pair(lhs.size(), rhs));
    }

    for (size_t i = 0; i < pairs.size(); )
    {
        size_t level = pairs[i].first;
        const string& path = pairs[i].second;
        bool match = false;

        for (size_t j = 0; j < prefixes.size(); j++)
        {
            const string& prefix = prefixes[j];

            if (path.substr(0, prefix.size()) == prefix)
            {
                match = true;
                break;
            }
        }

        if (prefixes.size() == 0)
            match = true;

#if 0
        cout << (i+1) << ": " << level << ": " << path << endl;
#endif
        i++;

        if (match)
        {
            if (!contains(matches, path))
            {
                cout << "#include \"" << path << "\"" << endl;
                matches.push_back(path);
            }

            for (; i < pairs.size(); i++)
            {
                if (pairs[i].first <= level)
                    break;
            }
        }
    }

    return 0;
}
