//1018:Big Number

//Problem Description
//In many applications very large integers numbers are required.Some of these applications are using keys for secure transmission of data, encryption, etc.In this problem you are given a number, you have to determine the number of digits in the factorial of the number.
//
//
//Input
//Input consists of several lines of integer numbers.The first line contains an integer n, which is the number of cases to be tested, followed by n lines, one integer 1 ¡Ü n ¡Ü 107 on each line.
//
//
//Output
//The output contains the number of digits in the factorial of the integers appearing in the input.
//
//
//Sample Input
//2
//10
//20
//
//
//Sample Output
//7
//19
//
//
//Source
//Asia 2002, Dhaka(Bengal)
//
//
//Recommend
//JGShining | We have carefully selected several similar problems for you:  1071 1061 1060 1089 1091

#include <cmath>
#include <iostream>
using namespace std;

int main()
{
	int length; //case_number
	cin >> length;
	for (size_t i = 0; i < length; i++)
	{
		int number;
		double sum = 0;
		cin >> number;
		for (size_t j = 1; j <= number; j++)
		{
			sum += log10(j);
		}
		cout << (int)ceil(sum) << endl;
	}
	return 0;
}