#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;

bool bothSpaces(char c1, char c2)
{
	return c1 == ' ' && c2 == ' ';
}
int main()
{
	cin.unsetf(ios::skipws);
	unique_copy(istream_iterator<char>(cin), istream_iterator<char>(),
		ostream_iterator<char>(cout), bothSpaces);
}