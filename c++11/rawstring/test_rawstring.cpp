#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

int main()
{
	{
		string test = R"(C:\A\B\C\D\file1.txt)";
		cout << test << endl;
	}

	{
		string test;
		test = R"(First Line.\nSecond line.\nThird Line.\n)";
		cout << test << endl;
	}

	{
		string test =
R"(First Line.
Second line.
Third Line.)";
		cout << test << endl;
	}
	
	return 0;
}
