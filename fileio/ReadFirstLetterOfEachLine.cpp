#include <iostream>
#include <string>
#include <bitset>
#include <limits>
using namespace std;


int main()
{
	while (1)
	{
		
		char test = cin.get();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "[" << test << "]" << endl;
	}
	
	return 0;
}