#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

class C {};

class A 
{ 
public: 
	A() {} 
	A(int in_a) : a(in_a) {} 
	A(C c) {} 
private: 
	int a = 4;
	int b = 2; 
	string h = "text1"; 
	string s = "text2";
};

int main()
{
	
	return 0;
}
