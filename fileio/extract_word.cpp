#include <iostream>
#include <string>
using namespace std;


int main()
{
	
	const string delims(" \t,.;?");
	string line;

	while (getline(cin, line))
	{
		string::size_type begIdx, endIdx;

		begIdx = line.find_first_not_of(delims);

		while (begIdx != string::npos)
		{
			endIdx = line.find_first_of(delims, begIdx);
			if (endIdx == string::npos)
			{
				endIdx = line.length();
			}

			for (size_t i = static_cast<int>(begIdx); i < endIdx; i++)
			{
				cout << line[i];
			}
			cout << ' ';

			begIdx = line.find_first_not_of(delims, endIdx);
		}
		cout << endl;
	}
	
	return 0;
}