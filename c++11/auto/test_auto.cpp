#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;

class foo
{
public:
	char id;
	
	foo() : id ('c')
	{}
};

template <typename T1, typename T2>
auto compose(T1 t1, T2 t2) -> decltype(t1 + t2)
{
	return t1+t2;
}

int main()
{
	{
		auto i = 42;        // i is an int
		auto l = 42LL;      // l is an long long
		auto p = new foo(); // p is a foo*
		
		cout << "i = " << i << endl;
		cout << "l = " << l << endl;
		cout << "p = " << p->id << endl;
		
		delete p;
	}

	{	
		std::map<std::string, std::vector<int>> map;
		std::vector<int> v;
		v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		map["one"] = v;

		for(const auto & kvp : map) 
		{
			std::cout << kvp.first << std::endl;

			for(auto v : kvp.second)
			{
				std::cout << v << std::endl;
			}
		}
		

		int arr[] = {1,2,3,4,5};
		for (int & e : arr) 
		{
			e = e*e;
		}
	}
	
	{
		auto v = compose(2, 3.14); // v's type is double
		cout << "v = " << v << endl;
	}
}
