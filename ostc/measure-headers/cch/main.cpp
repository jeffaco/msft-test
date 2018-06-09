#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

using namespace std;

const char* arg0;

unsigned long long TimeStampMilliseconds()
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
        return 0;

    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int Exec(
    vector<const char*>& args_,
    int* status,
    vector<char>& output)
{
    pid_t pid;
    int fd[2];
    int rc = -1;

    if (status)
        *status = 0;

    if (!status)
        goto done;

    if (pipe(fd) != 0)
	goto done;

    if ((pid = fork()) < 0)
	goto done;

    /* Parent */
    if (pid > 0)
    {
        ssize_t n;
        char buf[1024];

	close(fd[1]);

        /* Read stdin and stdout of child */
        while ((n = read(fd[0], buf, sizeof(buf))) > 0)
        {
            output.insert(output.end(), buf, buf + n);
        }

        output.push_back('\0');

	close(fd[0]);

        {
            int tmp;

            if (wait(&tmp) != pid)
                goto done;

            *status = WEXITSTATUS(tmp);
        }
    }

    /* Child */
    if (pid == 0)
    {
        /* Send standard input and output to the parent */
	dup2(fd[1], STDERR_FILENO);
	dup2(fd[1], STDOUT_FILENO);
	close(fd[0]);

        vector<const char*> args = args_;
        args.push_back(NULL);

	execv(args[0], (char**)&args[0]);
	exit(1);
    }

    rc = 0;

done:
    return rc;
}

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

bool GetNextLine(
    const vector<char>& text, 
    size_t& pos,
    string& line)
{
    line.clear();

    if (pos >= text.size())
        return false;

    for (; pos < text.size(); pos++)
    {
        if (text[pos] == '\0')
        {
            pos = text.size();
            break;
        }

        if (text[pos] == '\n')
        {
            pos++;
            break;
        }

        line.push_back(text[pos]);
    }

    return true;
}

void FindIncludes(
    const vector<char>& text,
    vector<string>& includes)
{
    string line;
    Pairs pairs;
    size_t pos = 0;

    // Read line by line:
    while (GetNextLine(text, pos, line))
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
        i++;

        if (!contains(includes, path))
            includes.push_back(path);

        for (; i < pairs.size(); i++)
        {
            if (pairs[i].first <= level)
                break;
        }
    }
}

bool CompileForIncludes(
    const vector<const char*>& args_,
    vector<string>& includes)
{
    int status = 0;
    vector<char> out;
    vector<const char*> args;

    // Build a command with -H option:
    {
        for (size_t i = 0; i < args_.size(); i++)
            args.push_back(args_[i]);

        args.push_back("-H");
        args.push_back("-fsyntax-only");
    }

    // Execute the command:
    if (Exec(args, &status, out) != 0 || status != 0)
        return false;

    // Find the first level includes:
    FindIncludes(out, includes);

    return true;
}

void DumpArgs(const vector<const char*>& args)
{
    for (size_t i = 0; i < args.size(); i++)
        cout << args[i] << " ";
    cout << endl;
}

size_t FindSourceFile(
    vector<const char*>& args)
{
    for (size_t i = 0; i < args.size(); i++)
    {
        string arg = args[i];

        size_t pos = arg.rfind(".cpp");

        if (pos != string::npos)
        {
            if (pos < arg.size() && pos + 4 == arg.size())
                return i;
        }

        pos = arg.rfind(".hpp");

        if (pos != string::npos)
        {
            if (pos < arg.size() && pos + 4 == arg.size())
                return i;
        }

        pos = arg.rfind(".h");

        if (pos != string::npos)
        {
            if (pos < arg.size() && pos + 2 == arg.size())
                return i;
        }
    }

    // Not found!
    return (size_t)-1;
}

bool Compile(
    vector<const char*>& args,
    unsigned long long& msec)
{
    int status = 0;
    vector<char> out;
    unsigned long long t0 = TimeStampMilliseconds();

    msec = 0;

#if 0
    size_t pos = FindSourceFile(args);

    if (pos != (size_t)-1)
        cout << "compiling " << args[pos] << "..." << endl;
#endif

#if 0
    DumpArgs(args);
#endif

    // Execute the command:
    if (Exec(args, &status, out) != 0 || status != 0)
        return false;

    unsigned long long t1 = TimeStampMilliseconds();
    msec = t1 - t0;

    return true;
}

void Indent(size_t level)
{
    for (size_t i = 0; i < level; i++)
        cout << "  ";
}

bool CompileRecursive(
    const vector<const char*>& args_,
    size_t source_file_pos,
    unsigned long long msec,
    vector<string>& stack,
    size_t level)
{
    // Form argument list:
    vector<const char*> args = args_;
    vector<string> includes;

    // Save so we don't compile this again.
    stack.push_back(args[source_file_pos]);

    // Print the name of the file and how long it took to compile:
    Indent(level);
    double secs = double(msec) / 1000.0;
    cout << args[source_file_pos] << ": " << secs << " (" << (secs * level) << ")" << endl;
    Indent(level);
    cout << "{" << endl;

    if (!CompileForIncludes(args, includes))
        return false;

    for (size_t i = 0; i < includes.size(); i++)
    {
        const string& path = includes[i];

        if (contains(stack, path))
            continue;

        args[source_file_pos] = path.c_str();

        if (path.substr(0, 5) == "/usr/" &&
            path.substr(0, 16) != "/usr/include/c++")
        {
            continue;
        }

        unsigned long long msec;

        if (Compile(args, msec))
        {
            if (!CompileRecursive(args, source_file_pos, msec, stack, level + 1))
            {
#if 1
                Indent(level + 1);
                cerr << path << " error:1" << endl;
#endif
            }
        }
        else
        {
#if 1
            Indent(level + 1);
            cerr << path << " error:2" << endl;
#endif
        }
    }

    Indent(level);
    cout << "}" << endl;

#if 0
    stack.pop_back();
#endif

    return true;
}

int main(int argc, const char* argv[])
{
    arg0 = argv[0];

    // Check arguments:
    if (argc == 1)
    {
        cerr << "Usage: " << argv[0] << " ARGS..." << endl;
        exit(1);
    }

    // Form argument list:
    vector<const char*> args;
    for (int i = 1; i < argc; i++)
        args.push_back(argv[i]);

    size_t source_file_pos = FindSourceFile(args);
    if (source_file_pos == (size_t)-1)
    {
        cerr << argv[0] << "failed to find source file from args" << endl;
        exit(1);
    }

    vector<string> stack;

    if (!CompileRecursive(args, source_file_pos, 0, stack, 0))
    {
        cerr << argv[0] << "compiling recursively failed" << endl;
        exit(1);
    }

    return 0;
}
