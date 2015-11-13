// g++ constit.cpp -o constit && ./constit

#include <vector>
#include <map>
#include <string>
#include <iostream>

using namespace std;

typedef map<string, string> conf;

class Config {
	
	public:
	Config(){
		m_config["a"] = "b";
		m_config["b"] = "c";
		m_config["c"] = "d";
	}

	conf::const_iterator Begin() const
	{
		return m_config.begin();
	}

	conf::const_iterator End() const
	{
		return m_config.end();
	}

	private:
		conf m_config;

};


int main(void)
{
	Config c;
	for (conf::const_iterator it = c.Begin(); it != c.End(); ++it )
	{
		cout << it->first << endl;
	}


	return 0;
}