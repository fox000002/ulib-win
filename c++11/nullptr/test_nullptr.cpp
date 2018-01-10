#include <iostream>
#include <memory>

using namespace std;

void foo(int* p)
{
	cout << "foo is called" << endl;
}

void bar(std::shared_ptr<int> p)
{
	cout << "bar is called" << endl;
}

int main()
{
	int * p1 = NULL;
	int * p2 = nullptr;   
	if(p1 == p2)
	{
		cout << "p1 == p2" << endl;
	}

	foo(nullptr);
	bar(nullptr);

	bool f = nullptr;
	//int i = nullptr; // error: A native nullptr can only be converted to bool or, using reinterpret_cast, to an integral type
	
	return 0;
}
