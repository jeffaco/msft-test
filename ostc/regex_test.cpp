// Simple program to test native C-style regular expressions
//
// Compile with something like:
//
//      make regex_test
//
// and use it like
//
//		sudo cat /var/log/messages | ./regex_test '\s(\S+/)?sudo:\s+\S+ : TTY=.* ; USER=\S+ ;'

#include <sys/types.h>
#include <regex.h>

#include <iostream>
#include <string>

using namespace std;

class regex_cleaner_t
{
public:
    regex_cleaner_t(regex_t* p) : m_p(p) { }
    ~regex_cleaner_t() { regfree(m_p); }
private:
    regex_t* m_p;
    regex_cleaner_t(const regex_cleaner_t&);
    regex_cleaner_t& operator=(const regex_cleaner_t&);
};

main(int argc, char *argv[])
{
    if (argc != 2) {
		cerr << "usage: " << argv[0] << " pattern" << endl;
		cerr << "will read stdin and print the lines that match the specified pattern" << endl;
		exit(1);
    }

    regex_t m_Preq;
    int errcode = regcomp(&m_Preq, argv[1], REG_EXTENDED|REG_NOSUB);
    if (errcode) {
		size_t length = regerror(errcode, &m_Preq, NULL, 0);
		char* str = new char[length + 1];
		(void)regerror(errcode, &m_Preq, str, length);
		cerr << "error in regex: " << str << endl;
		delete [] str;
		exit(1);
    }

    regex_cleaner_t _c(&m_Preq);

    while (true) {
        string text;
        getline(cin, text);
		if (cin.eof()) {
			break;
		}

		if (0 == regexec(&m_Preq, text.c_str(), 0, 0, 0)) {
			cout << "***";
		}
		cout << "\t" << text << endl;
    }
}
