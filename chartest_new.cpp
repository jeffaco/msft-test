#include <stdio.h>
#include <ctype.h>

#undef isdigit
#define        isdigit(ch) ((ch >= '0') && (ch <= '9'))

int main()
{
	char c = '\xC2';
	int i = -1;

	printf("%x\n\n", isdigit(c));
}
